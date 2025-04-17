#pragma once

#include "svg.h" 
#include "domain.h"

using namespace std::literals;

namespace MapRenderer
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

class RouteMap : public svg::Drawable
{
public:
    RouteMap();
    void Draw(svg::ObjectContainer& container) const override;
    RouteMap& SetMapSize(MapSize map_size);
    RouteMap& SetPadding(double padding);
    RouteMap& SetLineWidth(double width);
    RouteMap& SetStopsRadius(double radius);
    RouteMap& SetStopLabelFontSize(int size);
    RouteMap& SetBusLabelFontSize(int size);
    RouteMap& SetStopLabelOffset(double x, double y);
    RouteMap& SetBusLabelOffset(double x, double y);
    RouteMap& SetUnderLayerColor(const svg::Color& color);
    RouteMap& SetUnderLayerWidth(double width);
    void AddColorToPalette(const svg::Color& color);
    const Route& AddRoute(const Bus* bus_ptr);

private:
    void DrawRoutesLines(svg::ObjectContainer& container, const Geo::SphereProjector& projector) const;
    void DrawRoutesNames(svg::ObjectContainer& container, const Geo::SphereProjector& projector) const;
    void DrawStopCircles(svg::ObjectContainer& container, const Stop& stop, const Geo::SphereProjector& projector) const;
    void DrawStopName(svg::ObjectContainer& container, const Stop& stop, const Geo::SphereProjector& projector) const;

    std::set<Route> routes_;
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

}