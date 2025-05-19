#pragma once

#include "svg.h" 
#include "domain.h"
#include <map>

using namespace std::literals;

namespace map_renderer
{

struct MapSize {
    double width_ = 0.0;
    double height_ = 0.0;
};

struct Route {
    const Bus* bus_ptr_;
    svg::Color color_;

    bool operator <(const Route& other) const;
    bool operator <=(const Route& other) const;
    bool operator >(const Route& other) const;
    bool operator >=(const Route& other) const;
};

struct MapRendererProps {
    std::map<const std::string*, Route, PtrsComparator<std::string>> routes_;
    std::set<const Stop*, PtrsComparator<Stop>> stops_ptrs_;
    MapSize map_size_;
    double padding_ = 0.0;
    double line_width_ = 0.0;
    double stop_radius_ = 0.0;
    int bus_label_font_size_ = 0;
    int stop_label_font_size_ = 0;
    svg::Point bus_label_offset_;
    svg::Point stop_label_offset_;
    std::string font_family_ = "Verdana"s;
    std::string font_route_weight_ = "bold"s;
    svg::Color underlayer_color_;
    double underlayer_width_ = 0.0;
    std::vector<svg::Color> color_palette_;
    svg::Color stop_circle_color_ = "white"s;
    svg::Color stop_text_fill_ = "black"s;
};

class MapRenderer : public svg::Drawable {
public:
    MapRenderer();
    void Draw(svg::ObjectContainer& container) const override;
    MapRenderer& SetMapSize(MapSize map_size);
    MapRenderer& SetPadding(double padding);
    MapRenderer& SetLineWidth(double width);
    MapRenderer& SetStopsRadius(double radius);
    MapRenderer& SetStopLabelFontSize(int size);
    MapRenderer& SetBusLabelFontSize(int size);
    MapRenderer& SetStopLabelOffset(double x, double y);
    MapRenderer& SetBusLabelOffset(double x, double y);
    MapRenderer& SetUnderLayerColor(const svg::Color& color);
    MapRenderer& SetUnderLayerWidth(double width);
    void AddColorToPalette(const svg::Color& color);
    const Route& AddRoute(const Bus* bus_ptr);
    void ReorderRouteColors();

private:
    void DrawRoutesLines(svg::ObjectContainer& container, const Geo::SphereProjector& projector) const;
    void DrawRoutesNames(svg::ObjectContainer& container, const Geo::SphereProjector& projector) const;
    void DrawStopCircles(svg::ObjectContainer& container, const Stop& stop, const Geo::SphereProjector& projector) const;
    void DrawStopName(svg::ObjectContainer& container, const Stop& stop, const Geo::SphereProjector& projector) const;

    MapRendererProps props_;
};

}