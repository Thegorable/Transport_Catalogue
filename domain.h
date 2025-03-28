#pragma once
#include<vector>
#include<string>
#include<unordered_map>
#include<map>
#include<cstdint>
#include"geo.h"

struct Stop {
	Stop() = default;
	Stop(const std::string& name) : name_(name) {}
	Stop(std::string&& name) : name_(move(name)) {}
	Stop(const std::string& name, Geo::Coordinates coords) : name_(name), coords_(coords) {}
	Stop(std::string&& name, Geo::Coordinates coords) : name_(move(name)), coords_(coords) {}

	void AddNeighborStop(Stop* stop_ptr, uint32_t distance);

	std::string name_;
	Geo::Coordinates coords_;
	std::unordered_map<Stop*, uint32_t> neighbor_stops_dist_;

	bool operator == (const Stop& other) {
		return (this->name_ == other.name_ && this->coords_ == other.coords_);
	}
};

struct Bus {
	Bus() = default;
	Bus(const std::string& name, const std::vector<Stop*>& route) : name_(name), route_(route) {}
	Bus(std::string&& name, std::vector<Stop*>&& route) : name_(move(name)), route_(move(route)) {}

	bool operator < (const Bus& other) const {return this->name_ < other.name_; }

	std::string name_;
	std::vector<Stop*> route_;
};

struct RouteStatistics {
	int route_length_ = 0;
	int stops_count_ = 0; 
	int unique_stops_count_ = 0;
	double curvature_ = 0.0;
};