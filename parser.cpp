#include "parser.h"

using namespace std;

namespace Parser {
    Coordinates ParseCoordinates(string_view str) {
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

    string_view Trim(string_view string) {
        const auto start = string.find_first_not_of(' ');
        if (start == string.npos) {
            return {};
        }
        return string.substr(start, string.find_last_not_of(' ') + 1 - start);
    }

    vector<string_view> Split(string_view string, char delim) {
        vector<string_view> result;

        size_t pos = 0;
        while ((pos = string.find_first_not_of(' ', pos)) < string.length()) {
            auto delim_pos = string.find(delim, pos);
            if (delim_pos == string.npos) {
                delim_pos = string.size();
            }
            if (auto substr = Trim(string.substr(pos, delim_pos - pos)); !substr.empty()) {
                result.push_back(substr);
            }
            pos = delim_pos + 1;
        }

        return result;
    }

    vector<string_view> ParseRoute(string_view route) {
        if (route.find('>') != route.npos) {
            return Split(route, '>');
        }

        auto stops = Split(route, '-');
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
}