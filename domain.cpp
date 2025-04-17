#include "domain.h"

Stop::Stop() = default;

Stop::Stop(const std::string& name)
    : name_(name) {}

Stop::Stop(std::string&& name)
    : name_(std::move(name)) {}

Stop::Stop(const std::string& name, Geo::Coordinates coords)
    : name_(name), coords_(coords) {}

Stop::Stop(std::string&& name, Geo::Coordinates coords)
    : name_(std::move(name)), coords_(coords) {}

void Stop::AddNeighborStop(Stop* stop_ptr, uint32_t distance) {
    neighbor_stops_dist_[stop_ptr] = distance;
}

bool Stop::operator==(const Stop& other) const {
    return name_ == other.name_;
}

bool Stop::operator<(const Stop& other) const {
    return name_ < other.name_;
}

bool Stop::operator<=(const Stop& other) const {
    return name_ <= other.name_;
}

bool Stop::operator>(const Stop& other) const {
    return name_ > other.name_;
}

bool Stop::operator>=(const Stop& other) const {
    return name_ >= other.name_;
}

Bus::Bus() = default;

Bus::Bus(const std::string& name, const std::vector<Stop*>& route, bool is_round)
    : name_(name), route_(route), is_round_(is_round) {}

Bus::Bus(std::string&& name, std::vector<Stop*>&& route, bool is_round)
    : name_(std::move(name)), route_(std::move(route)), is_round_(is_round) {}

bool Bus::operator==(const Bus& other) const {
    return name_ == other.name_;
}

bool Bus::operator<(const Bus& other) const {
    return name_ < other.name_;
}

bool Bus::operator<=(const Bus& other) const {
    return name_ <= other.name_;
}

bool Bus::operator>(const Bus& other) const {
    return name_ > other.name_;
}

bool Bus::operator>=(const Bus& other) const {
    return name_ >= other.name_;
}