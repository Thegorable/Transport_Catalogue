#include "request_handler.h"

using namespace std;

Request::Request(RequestType type) : type_(type) {}

Stat::Stat(RequestType type, int id) : Request(type), id_(id) {}

StatStop::StatStop(RequestType type, int id) : Stat(type, id) {}

StatBus::StatBus(const RouteStatistics& parent, RequestType type, int id) 
: RouteStatistics(parent), Stat(type, id) {}

RequestBaseStop::RequestBaseStop(RequestType type, Geo::Coordinates coords)
: Request(type), coords_(coords) {}

RequestBaseBus::RequestBaseBus(RequestType type, bool is_round_trip)
: Request(type), is_round_trip_(is_round_trip) {}

void ProvideInputRequests(const std::vector<std::shared_ptr<Request>> &input_requests, TransportCatalogue &transport_c) {    
    vector<RequestBaseStop*> stops_requests;
    vector<RequestBaseBus*> buses_requests;
    
    for (auto& request : input_requests) {
        switch (request->type_) {
        case RequestType::Stop: {
            RequestBaseStop* stop_request = dynamic_cast<RequestBaseStop*>(request.get());
            if (stop_request == nullptr) {
                throw logic_error("Dynamic cast from Rquest to RequestBaseStop is failed");
            }
            stops_requests.push_back(stop_request);
            transport_c.AddStop(stop_request->name_, stop_request->coords_);
            break;
        }

        case RequestType::Bus: {
            RequestBaseBus* bus_request = dynamic_cast<RequestBaseBus*>(request.get());
            if (bus_request == nullptr) {
                throw logic_error("Dynamic cast from Rquest to RequestBaseBus is failed");
            }
            buses_requests.push_back(bus_request);
            break; 
        }

        default:
            break;
        }
    }


    for (auto& stop_target : stops_requests) {
        for (auto& stop_neighbor : stop_target->neighbor_stops_)
        transport_c.AddNeighborStopDistance(stop_target->name_, stop_neighbor.name_, stop_neighbor.distance_);
    }

    for (auto& bus_request : buses_requests) {
        auto& route = bus_request->route_;
        if (bus_request->is_round_trip_ && route.size() > 1) {
            int64_t start_point = static_cast<int64_t>(route.size()) - 2;
            for (int64_t i = start_point; i >= 0; i--) {
                route.push_back(route[i]);
            }
            transport_c.AddBus(bus_request->name_, route);
            continue;
        }
        route.push_back(route[0]);
        transport_c.AddBus(bus_request->name_, route);
    }
}

std::vector<std::shared_ptr<Stat>> GetStats(const vector<shared_ptr<Stat>> &out_requests, const TransportCatalogue &transport_c) {
    std::vector<std::shared_ptr<Stat>> stats;
    
    for (auto& request : out_requests) {
        switch (request->type_) {
        case RequestType::Bus: {
            auto statistics_opt = transport_c.GetRouteStatistics(request->name_);
            if (!statistics_opt) {
                stats.push_back(make_shared<Stat>(RequestType::Error, request->id_));
                break;
            }
            
            stats.push_back(make_shared<StatBus>(statistics_opt.value(), RequestType::Bus, request->id_));
            break;
        }
        
        case RequestType::Stop: {
            StatStop stat_stop(RequestType::Stop, request->id_);
            stat_stop.buses_ = transport_c.FindBuses(request->name_);
            if (stat_stop.buses_ == nullptr) {
                stats.push_back(make_shared<Stat>(RequestType::Error, request->id_));
                break;
            }
            stats.push_back(make_shared<StatStop>(move(stat_stop)));
            break;
        }

        default:
            break;
        }
    }

    return stats;
}