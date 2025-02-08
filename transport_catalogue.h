#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>
#include <stdexcept>
#include <forward_list>
#include <set>

#include "geo.h"

using namespace std::string_literals;

struct Stop {
	Stop() = default;
	Stop(const std::string& name) : name_(name) {}
	Stop(const std::string& name, Geo::Coordinates coords) : name_(name), coords_(coords) {}
	Stop(std::string&& name, Geo::Coordinates coords) : name_(move(name)), coords_(coords) {}

	bool IsEmpty() const {return name_.empty(); }

	std::string name_;
	Geo::Coordinates coords_;

	bool operator == (const Stop& other) {
		return (this->name_ == other.name_ && this->coords_ == other.coords_);
	}
};

struct Bus {
	Bus() = default;
	Bus(const std::string& name, const std::vector<Stop*>& route) : name_(name), route_(route) {}
	Bus(std::string&& name, std::vector<Stop*>&& route) : name_(move(name)), route_(move(route)) {}

	bool IsEmpty() const {return route_.empty(); }
	bool operator < (const Bus& other) const {return this->name_ < other.name_; }

	std::string name_;
	std::vector<Stop*> route_;
};

struct RouteStatistics {
	size_t stops_count_ = 0; 
	size_t unique_stops_count_ = 0;
	double route_length = 0.0;
};

template <typename T>
std::string VectorToString(const std::vector<T>& v) {
    if (v.empty()) {
        return "";
    }

    std::stringstream ss;
    for (const auto& elem : v) {
        ss << elem << ", ";
    }

    std::string result = ss.str();
    result.resize(result.size() - 2);
    return result;
}

class TransportCatalogue {

public:
	TransportCatalogue() = default;

	void AddStop(const std::string& stop, Geo::Coordinates coordinates);
	void AddRoute(const std::string& bus, const std::vector<std::string_view>& route);
	RouteStatistics GetRouteStatistics(std::string_view bus) const;
	const Bus* FindBus(std::string_view bus) const;
	const Stop* FindStop(std::string_view stop) const;
	const std::set<Bus*>* FindBuses(std::string_view stop) const;

protected:
	std::forward_list<Stop> stops_;
	std::forward_list<Bus> buses_;
	std::unordered_map<std::string_view, Stop*> stops_ptrs_;
	std::unordered_map<std::string_view, Bus*> buses_ptrs_;

	std::unordered_map<std::string_view, std::set<Bus*>> stops_routes_ptrs_;

	double GetRouteLength(std::string_view bus) const;

	Stop empty_stop_;
};