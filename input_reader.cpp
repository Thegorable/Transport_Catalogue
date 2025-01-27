#include "input_reader.h"
#include "parser.h"

#include <algorithm>
#include <cassert>
#include <iterator>

using namespace std;

void InputReader::ParseLine(string_view line) {
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