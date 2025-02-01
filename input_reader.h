#pragma once
#include <string>
#include <string_view>
#include <vector>

#include "geo.h"
#include "transport_catalogue.h"

struct CommandDescription {
    explicit operator bool() const {
        return !command.empty();
    }

    bool operator!() const {
        return !operator bool();
    }

    std::string command;
    std::string id;
    std::string description;
};

namespace Parser {
    Geo::Coordinates ParseCoordinates(std::string_view str);
    std::string_view TrimSapces(std::string_view string);
    std::vector<std::string_view> SplitToVector(std::string_view string, char devider);
    std::pair<std::string_view, std::string_view> SplitToPair(std::string_view text, 
    char devider);
    std::vector<std::string_view> ParseRoute(std::string_view route);
    CommandDescription ParseCommandDescription(std::string_view line);
}

class InputReader {
public:
    void ParseLine(std::string_view line);
    void ApplyCommands(TransportCatalogue& catalogue) const;

private:
    std::vector<CommandDescription> commands_;
};