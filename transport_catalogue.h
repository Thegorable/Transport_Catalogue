#pragma once
#include <sstream>
#include <stdexcept>
#include <forward_list>
#include <set>
#include <optional>
#include"domain.h"

using namespace std::string_literals;

struct BusPtrsComparator {
	bool operator()(const Bus* l, const Bus* r) const {
		return std::less{}(l->name_, r->name_);
	}
};


using BusPtrsSet = std::set<Bus*, BusPtrsComparator>;

class TransportCatalogue {

public:
	TransportCatalogue() = default;

	[[maybe_unused]] const Stop& AddStop(const std::string_view& stop, Geo::Coordinates coordinates);
	void AddBus(const std::string_view& bus, const std::vector<std::string_view>& route);
	
	template<typename StringContain, typename Converter>
	void AddBus(const std::string_view& bus, const std::vector<StringContain>& route, const Converter& converter);

	void AddNeighborStopDistance(std::string_view stop_target, std::string_view stop_neighbor,
	uint32_t distance);
	std::optional<RouteStatistics> GetRouteStatistics(std::string_view bus) const;
	const Bus* FindBus(std::string_view bus) const;
	const Stop* FindStop(std::string_view stop) const;
	const BusPtrsSet* FindBuses(std::string_view stop) const;

protected:
	std::forward_list<Stop> stops_;
	std::forward_list<Bus> buses_;
	std::unordered_map<std::string_view, Stop*> stops_ptrs_;
	std::unordered_map<std::string_view, Bus*> buses_ptrs_;

	std::unordered_map<std::string_view, BusPtrsSet> stops_routes_ptrs_;

	double GetRouteLength(std::string_view bus) const;
	uint32_t GetRealRouteLength(std::string_view bus) const;

	Stop empty_stop_;
};

template <typename StringContain, typename Converter>
inline void TransportCatalogue::AddBus(const std::string_view& bus, const std::vector<StringContain> &route, const Converter &converter) {
	std::vector<std::string_view> route_sv;
	for (auto& stop : route) {
		route_sv.push_back(converter(stop));
	}

	AddBus(bus, route_sv);
}
