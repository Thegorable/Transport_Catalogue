
#include <string>
#include <vector>

#include "geo.h"
#include "input_reader.h"

namespace Parser {

    Coordinates ParseCoordinates(std::string_view str);

    std::string_view Trim(std::string_view string);

    std::vector<std::string_view> Split(std::string_view string, char delim);

    std::vector<std::string_view> ParseRoute(std::string_view route);

    CommandDescription ParseCommandDescription(std::string_view line);

}