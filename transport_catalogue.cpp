#include "transport_catalogue.h"
#include <unordered_set>
#include <algorithm>

using namespace std;

void TransportCatalogue::AddStop(const string &stop, Geo::Coordinates coordinates) {
    if (stops_ptrs_.contains(stop)) {
        throw invalid_argument("Attempt to add existing stop: "s + stop + '\n');
    }
    Stop& new_stop_ref = stops_.emplace_front(stop, coordinates);
    stops_ptrs_[new_stop_ref.name_] = &new_stop_ref;
}

void TransportCatalogue::AddStop(string &&stop, Geo::Coordinates coordinates) {
    if (stops_ptrs_.contains(stop)) {
        throw invalid_argument("Attempt to add existing stop: "s + stop + '\n');
    }
    Stop& new_stop_ref = stops_.emplace_front(move(stop), coordinates);
    stops_ptrs_[new_stop_ref.name_] = &new_stop_ref;
}

void TransportCatalogue::AddRoute(const string_view &bus, const vector<std::string_view> &route) {
    if (buses_ptrs_.contains(bus)) {
        throw invalid_argument("Attempt to add existing bus: "s + string(bus) + '\n');
    }

    auto result = IsContainsFullRoute(route); 
    if (!result.first) {
        throw invalid_argument("Attempt to add route with not existing stops: "s + 
        VectorToString(result.second) + '\n');
    }

    vector<Stop*> new_route;
    new_route.reserve(route.size());
    for (const string_view& stop : route) {
        Stop* stop_ptr = stops_ptrs_.at(stop);
        new_route.emplace_back(stop_ptr);
    }

    Bus& new_bus = buses_.emplace_front(string(bus), move(new_route));
    buses_ptrs_[new_bus.name_] = &new_bus;
}

void TransportCatalogue::AddRoute(const string_view &bus, const vector<std::string> &route) {
    AddRoute(bus, vector<string_view> (route.begin(), route.end()));
}

size_t TransportCatalogue::GetCountUniqueStops(const string_view &bus) const {
    const Bus* bus_ptr = buses_ptrs_.at(bus);
    auto& route = bus_ptr->route_;
    unordered_set<Stop*> unique_stops(route.begin(), route.end());
    return unique_stops.size();
}

double TransportCatalogue::GetRouteLength(const string_view& bus) const {
    const Bus* bus_ptr = buses_ptrs_.at(bus);
    const vector<Stop*>& route = bus_ptr->route_;
    double sum = 0.0;
    for (size_t i = 1; i < route.size(); i++) {
        auto& prev = stops_ptrs_.at(route.at(i - 1)->name_)->coords_;
        auto& cur = stops_ptrs_.at(route.at(i)->name_)->coords_;
        sum += ComputeDistance(prev, cur);
    }

    return sum;
}

std::vector<std::string_view> TransportCatalogue::FindBuses(const std::string_view& stop) const {
    std::vector<std::string_view> buses;
    
    for (auto& [bus_name, bus] : buses_ptrs_) {
        auto& route = bus->route_;
        auto predicat = [&stop](Stop* l) {return l->name_ == stop;};
        if (find_if(route.begin(), route.end(), predicat) != route.end()) {
            buses.push_back(bus_name);
        }
    }
    
    return buses;
}

pair<bool, vector<string_view>> TransportCatalogue::IsContainsFullRoute(
    const vector<string_view> &route) const {
        
        std::vector<std::string_view> result;
        for (auto& stop : route) {
            if (!stops_ptrs_.contains(stop)) {
                result.push_back(stop);
            }
        }

    return {result.empty(), result};
}

pair<bool, std::vector<std::string_view>> TransportCatalogue::IsContainsFullRoute(
    const vector<string> &route) const {
    return IsContainsFullRoute(vector<string_view>(route.begin(), route.end()));
}