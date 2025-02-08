#include "input_reader.h"

#include <algorithm>

using namespace std;

namespace Parser {
    Geo::Coordinates ParseCoordinates(string_view str) {
        static const double nan = std::nan("");

        auto not_space = str.find_first_not_of(' ');
        auto comma = str.find(',');

        if (comma == str.npos) {
            return {nan, nan};
        }

        auto not_space2 = str.find_first_not_of(' ', comma + 1);

        double lat = stod(string(str.substr(not_space, comma - not_space)));
        double lng = stod(string(str.substr(not_space2)));

        return {lat, lng};
    }

    string_view TrimSapces(string_view string) {
        const auto start = string.find_first_not_of(' ');
        if (start == string.npos) {
            return {};
        }
        return string.substr(start, string.find_last_not_of(' ') + 1 - start);
    }

    vector<string_view> SplitToVector(string_view string, char delim) {
        vector<string_view> result;

        size_t pos = 0;
        while ((pos = string.find_first_not_of(' ', pos)) < string.length()) {
            auto delim_pos = string.find(delim, pos);
            if (delim_pos == string.npos) {
                delim_pos = string.size();
            }
            if (auto substr = TrimSapces(string.substr(pos, delim_pos - pos)); !substr.empty()) {
                result.push_back(substr);
            }
            pos = delim_pos + 1;
        }

        return result;
    }

    vector<string_view> ParseRoute(string_view route) {
        if (route.find('>') != route.npos) {
            return SplitToVector(route, '>');
        }

        auto stops = SplitToVector(route, '-');
        vector<string_view> results(stops.begin(), stops.end());
        results.insert(results.end(), next(stops.rbegin()), stops.rend());

        return results;
    }

    CommandDescription ParseCommandDescription(string_view line) {
        auto colon_pos = line.find(':');
        if (colon_pos == line.npos) {
            return {};
        }

        auto space_pos = line.find(' ');
        if (space_pos >= colon_pos) {
            return {};
        }

        auto not_space = line.find_first_not_of(' ', space_pos);
        if (not_space >= colon_pos) {
            return {};
        }

        return {string(line.substr(0, space_pos)),
                string(line.substr(not_space, colon_pos - not_space)),
                string(line.substr(colon_pos + 1))};
    }
    std::pair<std::string_view, std::string_view> SplitToPair(std::string_view text, 
    char devider) {
        size_t pos_str = text.find_first_of(devider);
        string_view first = text.substr(0, pos_str);
        string_view second = text.substr(pos_str + 1, text.size());

        return {first, second};
    }
}

void InputReader::ParseLine(string_view line)
{
    auto command_description = Parser::ParseCommandDescription(line);
    if (command_description) {
        commands_.push_back(std::move(command_description));
    }
}

void InputReader::ApplyCommands([[maybe_unused]] TransportCatalogue& catalogue) const {
    vector<pair<string_view, vector<string_view>>> buses;

    for (const CommandDescription& com : commands_) {
        if (com.command == "Bus"s) {
            buses.push_back({com.id, Parser::ParseRoute(com.description)});
            continue;
        }
        catalogue.AddStop(com.id, Parser::ParseCoordinates(com.description));
    }

    for (auto& bus : buses) {
        catalogue.AddRoute(string(bus.first), bus.second);
    }
}

void ReadInputAndApply(std::istream& in, TransportCatalogue& catalogue) {
    int base_request_count;
    in >> base_request_count >> ws;

    {
        InputReader reader;
        for (int i = 0; i < base_request_count; ++i) {
            string line;
            getline(in, line);
            reader.ParseLine(line);
        }
        reader.ApplyCommands(catalogue);
    }
}