#include "transport_catalogue.h"
#include <unordered_set>

using namespace std;

void TransportCatalogue::AddStop(const string &stop, Coordinates coordinates) {
    if (stops_.contains(stop)) {
        throw invalid_argument("Attempt to add existing stop: "s + stop + '\n');
    }
    stops_[stop] = coordinates;
}

void TransportCatalogue::AddStop(string &&stop, Coordinates coordinates) {
    if (stops_.contains(stop)) {
        throw invalid_argument("Attempt to add existing stop: "s + stop + '\n');
    }
    stops_[move(stop)] = coordinates;
}

void TransportCatalogue::AddRoute(const string &bus, const vector<std::string_view> &route) {
    if (buses_.contains(bus)) {
        throw invalid_argument("Attempt to add existing bus: "s + bus + '\n');
    }

    auto result = IsContainsFullRoute(route); 
    if (!result.first) {
        throw invalid_argument("Attempt to add route with not existing stops: "s + 
        VectorToString(result.second) + '\n');
    }

    vector<string_view> new_route;
    new_route.reserve(route.size());
    for (auto& stop : route) {
        auto it = stops_.find(stop);
        new_route.push_back(it->first);
    }

    buses_[bus] = move(new_route);
}

void TransportCatalogue::AddRoute(const string &bus, const vector<std::string> &route) {
    AddRoute(bus, vector<string_view> (route.begin(), route.end()));
}

size_t TransportCatalogue::GetCountUniqueStops(const string_view &bus) const {
    auto it = buses_.find(bus);
    if (it == buses_.end()) {
        throw out_of_range("The element "s + string(bus) + " doesn't exists");
    }
    auto& route = it->second;
    unordered_set<string_view> unique_stops(route.begin(), route.end());
    return unique_stops.size();
}

size_t TransportCatalogue::GetCountUniqueStops(const string& bus) const {
    return GetCountUniqueStops(string_view(bus));
}

double TransportCatalogue::GetRouteLength(const string_view& bus) const {
    auto it = buses_.find(bus);
    if (it == buses_.end()) {
        throw out_of_range("The element "s + string(bus) + " doesn't exists");
    }

    const vector<string_view>& buses_str = it->second;
    double sum = 0.0;
    for (size_t i = 1; i < buses_str.size(); i++) {
        auto& prev = stops_.find(buses_str.at(i - 1))->second;
        auto& cur = stops_.find(buses_str.at(i))->second;
        sum += ComputeDistance(prev, cur);
    }

    return sum;
}

double TransportCatalogue::GetRouteLength(const string& bus) const {
    return GetRouteLength(string_view(bus));
}

pair<bool, vector<string_view>> TransportCatalogue::IsContainsFullRoute(
    const vector<string_view> &route) const {
        
        std::vector<std::string_view> result;
        for (auto& stop : route) {
            if (!stops_.contains(stop)) {
                result.push_back(stop);
            }
        }

    return {result.empty(), result};
}

pair<bool, std::vector<std::string_view>> TransportCatalogue::IsContainsFullRoute(
    const vector<string> &route) const {
        return IsContainsFullRoute(vector<string_view>(route.begin(), route.end()));
}