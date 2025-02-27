#include "input_reader.h"

#include <algorithm>
#include <charconv>

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

    pair<string_view, string_view> SplitToPair(std::string_view text, 
        string_view divider, int num_divider) {
        size_t pos_str = text.find(divider);
        int iter_num = 1;
        
        while ((iter_num < num_divider) && (pos_str != text.npos)) {
            pos_str += divider.size();
            pos_str = text.find(divider, pos_str);
            ++iter_num;
        }

        if (pos_str == text.npos) {
            return {text, {}};
        }

        string_view first = text.substr(0, pos_str);
        string_view second = text.substr(pos_str + divider.size(), text.size());

        return {first, second};
    }

    pair<string_view, string_view> SplitToPair(const std::string_view& text, 
        char divider, int num_divider) {
        return SplitToPair(text, string_view(&divider, 1), num_divider);
    }

    std::vector<std::pair<string_view, uint32_t>> ParseDistances(string_view str) {
        std::vector<std::pair<string_view, uint32_t>> result;
        
        auto parts = SplitToVector(str, ',');
        
        for (auto& part : parts) {            
            auto [distance_part, stop_part] = SplitToPair(part, "m to "sv);          
            
            distance_part = TrimSapces(distance_part);
            stop_part = TrimSapces(stop_part);
            
            uint32_t distance = 0;
            auto conv_result = std::from_chars(distance_part.begin(), distance_part.end(), distance);
            if (conv_result.ec == std::errc()) {
                result.emplace_back(stop_part, distance);
            }
        }
        
        return result;
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
    using distances = vector<pair<string_view, uint32_t>>;

    vector<pair<string_view, vector<string_view>>> buses;
    vector<pair<string_view, distances>> neghbor_stops_;

    for (const CommandDescription& com : commands_) {
        if (com.command == "Bus"s) {
            buses.emplace_back(com.id, Parser::ParseRoute(com.description));
            continue;
        }

        auto [coords_text, neighbor_stops_text] = Parser::SplitToPair(com.description, ',', 2);
        neghbor_stops_.emplace_back(com.id, Parser::ParseDistances(neighbor_stops_text));
        catalogue.AddStop(com.id, Parser::ParseCoordinates(coords_text));
    }

    for (auto& [stop_target, distances] : neghbor_stops_) {
        for (auto& [stop_neighbor, distance] : distances) {
            catalogue.AddNeighborStopDistance(stop_target, stop_neighbor, distance);
        }
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