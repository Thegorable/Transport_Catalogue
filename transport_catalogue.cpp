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

void TransportCatalogue::AddRoute(const string &bus, const vector<std::string_view> &route) {
    if (buses_ptrs_.contains(bus)) {
        throw invalid_argument("Attempt to add existing bus: "s + bus + '\n');
    }

    vector<Stop*> new_route;
    new_route.reserve(route.size());
    for (const string_view& stop : route) {
        Stop* stop_ptr = stops_ptrs_.at(stop);
        new_route.emplace_back(stop_ptr);
    }

    Bus& new_bus = buses_.emplace_front(bus, move(new_route));
    for (auto& stop : new_bus.route_) {
        stops_routes_ptrs_[stop->name_].insert(&new_bus);
    }
    buses_ptrs_[new_bus.name_] = &new_bus;
}

RouteStatistics TransportCatalogue::GetRouteStatistics(string_view bus) const {
    const vector<Stop*>& route = buses_ptrs_.at(bus)->route_;
    unordered_set<Stop*> unique_stops_(route.begin(), route.end());

    return {route.size(), 
        unique_stops_.size(), 
        GetRouteLength(bus) };
}

const Bus* TransportCatalogue::FindBus(string_view bus) const {
    auto result = buses_ptrs_.find(bus);
    if (result == buses_ptrs_.end()) {
        return nullptr;
    }
    
    return result->second;
}

const Stop* TransportCatalogue::FindStop(string_view stop) const {
    auto result = stops_ptrs_.find(stop);
    if (result == stops_ptrs_.end()) {
        return nullptr;
    }

    return result->second;
}

const set<Bus*>* TransportCatalogue::FindBuses(string_view stop) const {
    auto it = stops_routes_ptrs_.find(stop);
    if (it == stops_routes_ptrs_.end()) {
        return nullptr;
    }

    return &it->second;
}

double TransportCatalogue::GetRouteLength(string_view bus) const {
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