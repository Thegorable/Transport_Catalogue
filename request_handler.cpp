#include "request_handler.h"

using namespace std;

Request::Request(RequestType type) : type_(type) {}

Stat::Stat(RequestType type, int id) : Request(type), id_(id) {}

StatStop::StatStop(RequestType type, int id) : Stat(type, id) {}

StatBus::StatBus(const RouteStatistics& parent, RequestType type, int id) 
: RouteStatistics(parent), Stat(type, id) {}

StatMap::StatMap(RequestType type, int id) : Stat(type, id) {}

RequestBaseStop::RequestBaseStop(RequestType type, Geo::Coordinates coords)
: Request(type), coords_(coords) {}

RequestBaseBus::RequestBaseBus(RequestType type, bool is_round_trip)
: Request(type), is_round_trip_(is_round_trip) {}

RequestHander::RequestHander() = default;

void RequestHander::AddRequest(Request& base_request) {
    base_request.MoveToHandler(*this);
}

void RequestHander::AddRequest(RequestBaseBus &&request_base_bus) {
    base_bus_requests_.push_back(move(request_base_bus));
}

void RequestHander::AddRequest(RequestBaseStop &&request_base_stop) {
    base_stop_requests_.push_back(move(request_base_stop));
}

void RequestHander::AddRequest(Stat &&request_stat) {
    stat_requests_.push_back(move(request_stat));
}

void RequestBaseBus::MoveToHandler(RequestHander &handler) {
    handler.AddRequest(move(*this));
}

void RequestBaseStop::MoveToHandler(RequestHander &handler) {
    handler.AddRequest(move(*this));
}

void StatBus::MoveToHandler(RequestHander &handler) {
    handler.AddRequest(move(*this));
}

void StatStop::MoveToHandler(RequestHander &handler) {
    handler.AddRequest(move(*this));
}

void Stat::MoveToHandler(RequestHander &handler) {
    handler.AddRequest(*this);
}

void RequestHander::ProvideInputRequests(TransportCatalogue &transport_c) {
    for (auto& stop_target : base_stop_requests_) {
        transport_c.AddStop(stop_target.name_, stop_target.coords_);
    }

    for (auto& stop_target : base_stop_requests_) {
        for (auto& stop_neighbor : stop_target.neighbor_stops_)
        transport_c.AddNeighborStopDistance(stop_target.name_, stop_neighbor.name_, stop_neighbor.distance_);
    }

    for (auto& bus_request : base_bus_requests_) {
        auto& route = bus_request.route_;
        if (!bus_request.is_round_trip_ && route.size() > 1) {
            int64_t start_point = static_cast<int64_t>(route.size()) - 2;
            for (int64_t i = start_point; i >= 0; i--) {
                route.push_back(route[i]);
            }
            transport_c.AddBus(bus_request.name_, route, bus_request.is_round_trip_);
            continue;
        }
        transport_c.AddBus(bus_request.name_, route, bus_request.is_round_trip_);
    }
}

vector<shared_ptr<Stat>> RequestHander::GetStats(const TransportCatalogue &transport_c, 
    const std::optional<MapRenderer::RouteMap>& route_map) const {
    vector<shared_ptr<Stat>> stats;

    for (auto& request : stat_requests_) {
        switch (request.type_) {
        case RequestType::Bus:
            PushBusStat(transport_c, request, stats);
            break;
        
        case RequestType::Stop:
            PushStopStat(transport_c, request, stats);
            break;

        case RequestType::Map:
            PushMapStat(route_map.value(), request, stats);
            break;

        default:
            break;
        }
    }
    
    return stats;
}

void RequestHander::PushBusStat(const TransportCatalogue &transport_c, 
    const Stat& stat, 
    std::vector<std::shared_ptr<Stat>> &container) const {

    auto statistics_opt = transport_c.GetRouteStatistics(stat.name_);
    if (!statistics_opt) {
        container.push_back(make_shared<Stat>(RequestType::Error, stat.id_));
        return;
    }
    container.push_back(make_shared<StatBus>(statistics_opt.value(), RequestType::Bus, stat.id_));
}

void RequestHander::PushStopStat(const TransportCatalogue &transport_c, 
    const Stat& stat,  
    std::vector<std::shared_ptr<Stat>> &container) const {
        
    StatStop stat_stop(RequestType::Stop, stat.id_);
    auto stop = transport_c.FindStop(stat.name_);
    if (stop == nullptr) {
        container.push_back(make_shared<Stat>(RequestType::Error, stat.id_));
        return;
    }
    stat_stop.buses_ = transport_c.FindBuses(stat.name_);
    container.push_back(make_shared<StatStop>(move(stat_stop)));
    return;
}

void RequestHander::PushMapStat(const MapRenderer::RouteMap& route_map, 
    const Stat &stat, 
    std::vector<std::shared_ptr<Stat>> &container) const {

    StatMap drawn_map(RequestType::Map, stat.id_);
    route_map.Draw(drawn_map.map_);
    container.push_back(make_shared<StatMap>(move(drawn_map)));
}
