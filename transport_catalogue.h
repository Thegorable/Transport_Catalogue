#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>
#include <stdexcept>
#include <forward_list>

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

	std::string name_;
	std::vector<Stop*> route_;
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
	void AddStop(std::string&& stop, Geo::Coordinates coordinates);

	void AddRoute(const std::string_view& bus, const std::vector<std::string>& route);
	void AddRoute(const std::string_view& bus, const std::vector<std::string_view>& route);

	inline size_t GetCountStops(const std::string_view& bus) const {
		return buses_ptrs_.at(bus)->route_.size();
	}

	inline size_t GetCountAllStops() const {return stops_ptrs_.size(); }
	inline size_t GetCountAllbuses() const {return buses_ptrs_.size(); }

	size_t GetCountUniqueStops(const std::string_view& bus) const;
	double GetRouteLength(const std::string_view& bus) const;

	inline const Bus& FindBus(const std::string_view& bus) const {
		if (!buses_ptrs_.contains(bus)) {
			return empty_bus_;
		}
		
		return *buses_ptrs_.at(bus);
	}

	inline bool IsContainsBus(const std::string_view& bus) const {
		return buses_ptrs_.contains(bus);
	}

	inline const Stop& FindStop(const std::string_view& stop) const {
		if (!stops_ptrs_.contains(stop)) {
			return empty_stop_;
		}

		return *stops_ptrs_.at(stop);
	}
	inline const Stop& FindStop(const std::string& stop) const
	{ return FindStop(std::string_view(stop)); }

	inline bool IsContainsStop(const std::string_view& stop) const {
		return stops_ptrs_.contains(stop);
	}

	std::vector<std::string_view> FindBuses(const std::string_view& stop) const;

protected:
	std::forward_list<Stop> stops_;
	std::forward_list<Bus> buses_;
	std::unordered_map<std::string_view, Stop*> stops_ptrs_;
	std::unordered_map<std::string_view, Bus*> buses_ptrs_;

	Bus empty_bus_;
	Stop empty_stop_;

	std::pair<bool, std::vector<std::string_view>> IsContainsFullRoute(
		const std::vector<std::string>& route) const;
	std::pair<bool, std::vector<std::string_view>> IsContainsFullRoute(
		const std::vector<std::string_view>& route) const;
};