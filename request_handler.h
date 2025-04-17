#pragma once
#include <memory>
#include "transport_catalogue.h"

enum class RequestType {Bus, Stop, Error};

class RequestHander;

struct NeighborStop {
	std::string_view name_;
	int distance_;
};

struct Request {
    Request() = default;
    Request(RequestType type = RequestType::Bus);
    virtual void MoveToHandler(RequestHander& handler) = 0;

    RequestType type_;
    std::string_view name_;
};

struct RequestBaseStop : public Request {
    RequestBaseStop(RequestType type = RequestType::Bus, Geo::Coordinates coords = {0.0, 0.0});
    void MoveToHandler(RequestHander& handler) override;

    Geo::Coordinates coords_;
	std::vector<NeighborStop> neighbor_stops_;
};

struct RequestBaseBus : public Request {
    RequestBaseBus(RequestType type = RequestType::Bus, bool is_round_trip = false);
    void MoveToHandler(RequestHander& handler) override;

    std::vector<std::string_view> route_;
    bool is_round_trip_;
};

struct Stat : public Request {
    Stat(RequestType type = RequestType::Bus, int id = 0);
    void MoveToHandler(RequestHander& handler) override;
    int id_ = 0;
};

struct StatStop : public Stat {
    StatStop(RequestType type = RequestType::Bus, int id = 0);
    void MoveToHandler(RequestHander& handler) override;

    const BusPtrsSet* buses_;
};

struct StatBus : public RouteStatistics, public Stat {
    StatBus(const RouteStatistics& parent, RequestType type = RequestType::Bus, int id = 0);
    void MoveToHandler(RequestHander& handler) override;
};

class RequestHander {
public:
    RequestHander();
    void AddRequest(Request& base_request);
    void AddRequest(RequestBaseBus&& request_base_bus);
    void AddRequest(RequestBaseStop&&  request_base_stop);
    void AddRequest(Stat&& request_stat);
    void ProvideInputRequests(TransportCatalogue& transport_c);
    std::vector<std::shared_ptr<Stat>> GetStats(const TransportCatalogue& transport_c) const;
private:
    void PushBusStat(const TransportCatalogue& transport_c, 
        const Stat& stat, 
        std::vector<std::shared_ptr<Stat>>& container) const;
    void PushStopStat(const TransportCatalogue& transport_c, 
        const Stat& stat, 
        std::vector<std::shared_ptr<Stat>>& container) const;
    std::vector<RequestBaseStop> base_stop_requests_;
    std::vector<RequestBaseBus> base_bus_requests_;
    std::vector<Stat> stat_requests_;
};