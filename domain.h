#pragma once
#include<vector>
#include<string>
#include<unordered_map>
#include<cstdint>
#include<set>
#include"geo.h"

struct Stop {
    Stop();
    Stop(const std::string& name);
    Stop(std::string&& name);
    Stop(const std::string& name, Geo::Coordinates coords);
    Stop(std::string&& name, Geo::Coordinates coords);

    void AddNeighborStop(Stop* stop_ptr, uint32_t distance);

    bool operator==(const Stop& other) const;
    bool operator<(const Stop& other) const;
    bool operator<=(const Stop& other) const;
    bool operator>(const Stop& other) const;
    bool operator>=(const Stop& other) const;

    std::string name_;
    Geo::Coordinates coords_;
    std::unordered_map<const Stop*, uint32_t> neighbor_stops_dist_;
};

struct Bus {
    Bus();
    Bus(const std::string& name, const std::vector<Stop*>& route, bool is_round = false);
    Bus(std::string&& name, std::vector<Stop*>&& route, bool is_round = false);

    bool operator==(const Bus& other) const;
    bool operator<(const Bus& other) const;
    bool operator<=(const Bus& other) const;
    bool operator>(const Bus& other) const;
    bool operator>=(const Bus& other) const;

    std::string name_;
    std::vector<Stop*> route_;
    bool is_round_ = false;
};

template <typename T>
struct PtrsComparator {
	bool operator()(const T* l, const T* r) const {
		return std::less{}(*l, *r);
	}
};


using BusPtrsSet = std::set<const Bus*, PtrsComparator<Bus>>;

struct RouteStatistics {
	int route_length_ = 0;
	int stops_count_ = 0; 
	int unique_stops_count_ = 0;
	double curvature_ = 0.0;
};