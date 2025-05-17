#include "json_reader.h"

using namespace std;

JsonReader::JsonReader() = default;

void JsonReader::ReadBaseJsonRequests(const json::Document& doc, RequestHander& handler) {
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

            handler.AddRequest(move(request_base_stop));
            continue;
        }

        RequestBaseBus request_base_bus(RequestType::Bus);
        request_base_bus.name_ = base_request.AsMap().at("name"s).AsString();
        request_base_bus.is_round_trip_ = base_request.AsMap().at("is_roundtrip"s).AsBool();
        
        const auto& stops = base_request.AsMap().at("stops"s).AsArray();
        for (const auto& stop_node : stops) {
            request_base_bus.route_.emplace_back(stop_node.AsString());
        }
        
        handler.AddRequest(move(request_base_bus));
    } 
}

void JsonReader::ReadStatJsonRequests(const json::Document& doc, RequestHander& handler) {
    auto& result = doc.GetRoot().AsMap().at("stat_requests").AsArray();

    for (const auto& stat_request : result) {
        Stat request_format;
        request_format.id_ = stat_request.AsMap().at("id"s).AsInt();

        auto name_it = stat_request.AsMap().find("name"s);
        if (name_it != stat_request.AsMap().end()) {
            request_format.name_ = name_it->second.AsString();
        }

        auto& type_request = stat_request.AsMap().at("type"s).AsString();
        
        if (type_request == "Stop"s) {
            request_format.type_ = RequestType::Stop;
        }

        else if (type_request == "Bus"s) {
            request_format.type_ = RequestType::Bus;
        }

        else if (type_request == "Map"s) {
            request_format.type_ = RequestType::Map;
        }

        handler.AddRequest(move(request_format));
    }
}

void JsonReader::ReadRenderSettingsJson(const json::Document& doc, MapRenderer::RouteMap& route_map) {
    const json::Dict& settings = doc.GetRoot().AsMap().at("render_settings"s).AsMap();

    route_map.SetMapSize({settings.at("width").AsDouble(), settings.at("height").AsDouble()}).
    SetPadding(settings.at("padding").AsDouble()).SetBusLabelFontSize(settings.at("bus_label_font_size").AsInt()).
    SetStopLabelFontSize(settings.at("stop_label_font_size").AsInt()).SetUnderLayerWidth(settings.at("underlayer_width").AsDouble()).
    SetStopsRadius(settings.at("stop_radius").AsDouble()).SetLineWidth(settings.at("line_width").AsDouble());

    const json::Array& bus_label_off = settings.at("bus_label_offset").AsArray();
    const json::Array& stop_label_off = settings.at("stop_label_offset").AsArray();

    route_map.SetBusLabelOffset(bus_label_off[0].AsDouble(), bus_label_off[1].AsDouble()).
    SetStopLabelOffset(stop_label_off[0].AsDouble(), stop_label_off[1].AsDouble());

    const auto& underlayer_color = settings.at("underlayer_color");
    route_map.SetUnderLayerColor(ReadColor(underlayer_color));

    const json::Array& color_palette = settings.at("color_palette").AsArray();
    for (auto& color : color_palette) {
        route_map.AddColorToPalette(ReadColor(color));
    }
}

json::Document JsonReader::BuildStatJsonOutput(const std::vector<std::shared_ptr<Stat>>& answers) {
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
            if (ptr_stat->buses_ != nullptr) {
                for (auto& bus : *ptr_stat->buses_) {
                    buses.emplace_back(bus->name_);
                }
            }
            stat["buses"s] = move(buses);
            arr.push_back(move(stat));
            break;
        }
        case RequestType::Map: {
            StatMap* ptr_stat = dynamic_cast<StatMap*>(answer.get());
            if (ptr_stat == nullptr) {
                throw logic_error("Dynamic cust to StatMap is failed"s);
            }
            
            stringstream stream_str;
            ptr_stat->map_.Render(stream_str);
            string svg_map(stream_str.str());

            stat["map"] = move(svg_map);
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

svg::Color JsonReader::ReadColor(const json::Node& node) {
    if (node.IsString()) {
        return {node.AsString()};
    }
    
    const json::Array& color_arr = node.AsArray();
    if (color_arr.size() == 3) {
        return (svg::Rgb{ static_cast<uint8_t>(color_arr[0].AsInt()), 
            static_cast<uint8_t> (color_arr[1].AsInt()), 
            static_cast<uint8_t>(color_arr[2].AsInt())});
    }
    else if (color_arr.size() == 4) {
        return(svg::Rgba{ static_cast<uint8_t>(color_arr[0].AsInt()), 
            static_cast<uint8_t> (color_arr[1].AsInt()), 
            static_cast<uint8_t>(color_arr[2].AsInt()), 
            color_arr[3].AsDouble()});
    }

    return {};
}