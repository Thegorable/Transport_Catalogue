#include "json_reader.h"

using namespace std;

vector<shared_ptr<Request>> ReadBaseJsonRequests(const json::Document& doc) {
    vector<shared_ptr<Request>> requests;
    const json::Array& base_requests = doc.GetRoot().AsMap().at("base_requests"s).AsArray();

    for (const auto& base_request : base_requests) {
        const auto& type = base_request.AsMap().at("type"s).AsString();
        
        if (type == "Stop"s) {
            RequestBaseStop request_base_stop(RequestType::Stop);
            request_base_stop.name_ = base_request.AsMap().at("name"s).AsString();
            request_base_stop.coords_ = Geo::Coordinates(base_request.AsMap().at("latitude"s).AsDouble(), 
                                                        base_request.AsMap().at("longitude"s).AsDouble());
            
            const auto& neighbor_stops = base_request.AsMap().at("road_distances"s).AsMap();

            for (const auto& [name_stop, dist_node] : neighbor_stops) {
                request_base_stop.neighbor_stops_.emplace_back(name_stop, dist_node.AsInt());
            }

            requests.emplace_back(make_shared<RequestBaseStop>(move(request_base_stop)));
            continue;
        }

        RequestBaseBus request_base_bus(RequestType::Bus);
        request_base_bus.name_ = base_request.AsMap().at("name"s).AsString();
        request_base_bus.is_round_trip_ = base_request.AsMap().at("is_roundtrip"s).AsBool();
        
        const auto& stops = base_request.AsMap().at("stops"s).AsArray();
        for (const auto& stop_node : stops) {
            request_base_bus.route_.emplace_back(stop_node.AsString());
        }
        
        requests.emplace_back(make_shared<RequestBaseBus>(move(request_base_bus)));
    }

    return requests;
}

std::vector<std::shared_ptr<Stat>> ReadStatJsonRequests(const json::Document &doc) {
    vector<shared_ptr<Stat>> requests;
    const json::Array& stat_requests = doc.GetRoot().AsMap().at("stat_requests"s).AsArray();

    for (const auto& stat_request : stat_requests) {
        Stat request_format;
        request_format.id_ = stat_request.AsMap().at("id"s).AsInt();
        request_format.name_ = stat_request.AsMap().at("name"s).AsString();
        
        const auto& type = stat_request.AsMap().at("type"s).AsString();
        if (type == "Stop"s) {
            request_format.type_ = RequestType::Stop;
            requests.emplace_back(make_shared<Stat>(move(request_format)));
            continue;
        }

        request_format.type_ = RequestType::Bus;
        requests.emplace_back(make_shared<Stat>(move(request_format)));
    }

    return requests;
}

json::Document BuildStatJsonOutput(const std::vector<std::shared_ptr<Stat>> &answers) {
    json::Array arr;

    for (auto& answer : answers) {
        json::Dict stat;
        stat["request_id"s] = answer->id_;

        switch (answer->type_) {
        
        case RequestType::Bus: {
            StatBus* ptr_stat = dynamic_cast<StatBus*>(answer.get());
            if (ptr_stat == nullptr) {
                throw logic_error("Dynamic cust to StatBus is failed"s);
            }
            stat["curvature"s] = ptr_stat->curvature_;
            stat["route_length"s] = ptr_stat->route_length_;
            stat["stop_count"s] = ptr_stat->stops_count_;
            stat["unique_stop_count"s] = ptr_stat->unique_stops_count_;
            arr.push_back(move(stat));
            break;
        }
        
        case RequestType::Stop: {
            StatStop* ptr_stat = dynamic_cast<StatStop*>(answer.get());
            if (ptr_stat == nullptr) {
                throw logic_error("Dynamic cust to StatStop is failed"s);
            }

            json::Array buses;
            for (auto& bus : *ptr_stat->buses_) {
                buses.emplace_back(bus->name_);
            }
            stat["buses"s] = move(buses);
            arr.push_back(move(stat));
            break;
        }

        case RequestType::Error: {
            stat["error_message"] = "not found"s;
            arr.push_back(move(stat));
            break;
        }

        }
    }

    return json::Document(json::Node(arr));
}
