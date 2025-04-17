#pragma once
#include "domain.h"
#include "json.h"
#include "request_handler.h"
#include "map_renderer.h"

class JsonReader {
public:
    JsonReader();
    void ReadBaseJsonRequests(const json::Document& doc, RequestHander& handler);
    void ReadStatJsonRequests(const json::Document& doc, RequestHander& handler);
    void ReadRenderSettingsJson(const json::Document& doc, MapRenderer::RouteMap& route_map);
    json::Document BuildStatJsonOutput(const std::vector<std::shared_ptr<Stat>>& answers);

private:
    svg::Color ReadColor(const json::Node& node);
};