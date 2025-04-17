#include "transport_catalogue.h"
#include <unordered_set>
#include <algorithm>

using namespace std;

const Stop& TransportCatalogue::AddStop(const string_view &stop, Geo::Coordinates coordinates) {
    string stop_str(stop);
    if (stops_ptrs_.contains(stop)) {
        throw invalid_argument("Attempt to add existing stop: "s + stop_str + '\n');
    }
    Stop& new_stop_ref = stops_.emplace_front(move(stop_str), coordinates);
    stops_ptrs_[new_stop_ref.name_] = &new_stop_ref;

    return new_stop_ref;
}

void TransportCatalogue::AddBus(const string_view &bus, const vector<std::string_view>& route, bool is_round) {
    string bus_str(bus);
    if (buses_ptrs_.contains(bus_str)) {
        throw invalid_argument("Attempt to add existing bus: "s + bus_str + '\n');
    }

    vector<Stop*> new_route;
    new_route.reserve(route.size());
    for (const string_view& stop : route) {
        Stop* stop_ptr = stops_ptrs_.at(stop);
        new_route.emplace_back(stop_ptr);
    }

    Bus& new_bus = buses_.emplace_front(move(bus_str), move(new_route), is_round);
    for (auto& stop : new_bus.route_) {
        stops_routes_ptrs_[stop->name_].insert(&new_bus);
    }
    buses_ptrs_[new_bus.name_] = &new_bus;
}

void TransportCatalogue::AddNeighborStopDistance(std::string_view stop_target_str, 
    std::string_view stop_neighbor_str, 
    uint32_t distance) {
        auto stop_target = stops_ptrs_.find(stop_target_str)->second;
        auto stop_neighbor = stops_ptrs_.find(stop_neighbor_str)->second;

        stop_target->AddNeighborStop(stop_neighbor, distance);

        if (!stop_neighbor->neighbor_stops_dist_.contains(stop_target)) {
            stop_neighbor->AddNeighborStop(stop_target, distance);
        }
}

optional<RouteStatistics> TransportCatalogue::GetRouteStatistics(string_view bus) const {
    auto it = buses_ptrs_.find(bus);
    if (it == buses_ptrs_.end()) {
        return {};
    }

    const vector<Stop*>& route = it->second->route_;
    unordered_set<Stop*> unique_stops_(route.begin(), route.end());
    uint32_t real_distance = GetRealRouteLength(bus);

    return optional(RouteStatistics{static_cast<int>(real_distance),
                                    static_cast<int>(route.size()), 
                                    static_cast<int>(unique_stops_.size()),
                                    static_cast<double> (real_distance / GetRouteLength(bus)) });
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

const BusPtrsSet* TransportCatalogue::FindBuses(string_view stop) const {
    auto it = stops_routes_ptrs_.find(stop);
    if (it == stops_routes_ptrs_.end()) {
        return nullptr;
    }

    return &it->second;
}

vector<const Bus*> TransportCatalogue::GetAllBuses() const {
    vector<const Bus*> result;
    for (auto& bus : buses_) {
        result.push_back(&bus);
    }
    return result;
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

uint32_t TransportCatalogue::GetRealRouteLength(std::string_view bus) const {
    const Bus* bus_ptr = buses_ptrs_.at(bus);
    const vector<Stop*>& stops = bus_ptr->route_;
    uint32_t total_distance = 0;

    for (int i = 0; i < static_cast<int>(stops.size()) - 1; i++) {
        const auto& curr_stop = stops[i];
        const auto& next_stop = stops[i+1];

        auto nightbor_stop_it = curr_stop->neighbor_stops_dist_.find(next_stop);
        if (nightbor_stop_it == curr_stop->neighbor_stops_dist_.end()) {
            return 0;
        }

        total_distance += curr_stop->neighbor_stops_dist_[next_stop];
    }

    return total_distance;
}
