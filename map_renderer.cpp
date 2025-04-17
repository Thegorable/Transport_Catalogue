#include "map_renderer.h"
#include <vector>
#include <array>

using namespace std;

namespace MapRenderer {

class CoordinatesIt {
public:
    using value_type = const Geo::Coordinates;
    using reference = const Geo::Coordinates&;
    using pointer = const Geo::Coordinates*;
    using difference_type = std::ptrdiff_t;
    using iterator_category = std::forward_iterator_tag;

    CoordinatesIt() = default;
    explicit CoordinatesIt(std::set<const Stop*>::const_iterator it) : it_(it) {}

    reference operator*() const { return (*it_)->coords_; }
    pointer operator->() const { return &(*it_)->coords_; }

    CoordinatesIt operator ++(int) {auto prev = *this; it_++; return prev;}
    CoordinatesIt operator ++() {it_++; return *this;}

    bool operator ==(const CoordinatesIt& other) {return it_ == other.it_; }
    bool operator !=(const CoordinatesIt& other) {return !(*this == other); }

private:
    std::set<const Stop*>::const_iterator it_;
};

    
void RouteMap::Draw(svg::ObjectContainer &container) const {
    Geo::SphereProjector projector(CoordinatesIt(stops_ptrs_.begin()), CoordinatesIt(stops_ptrs_.end()),
    map_size_.width_, map_size_.height_, padding_);

    DrawRoutesLines(container, projector);
    DrawRoutesNames(container, projector);
    for (auto& stop_ptr : stops_ptrs_) {
        DrawStopCircles(container, *stop_ptr, projector);   
    }
    for (auto& stop_ptr : stops_ptrs_) {
        DrawStopName(container, *stop_ptr, projector);
    }
}

RouteMap::RouteMap() = default;

const Route& RouteMap::AddRoute(const Bus *bus_ptr) {
    Route new_route;
    new_route.bus_ptr_ = bus_ptr;
    new_route.color_ = color_palette_.at(Utility::GetCycleInterger(size_t(0), color_palette_.size() - 1, routes_.size()));
    for (auto& stop : bus_ptr->route_) {
        stops_ptrs_.insert(stop);
    }
    auto result = routes_.insert(new_route);
    if (!result.second) {
        throw runtime_error("The adding new route was unsucsessful"s);
    }
    return *result.first;
}

RouteMap &RouteMap::SetMapSize(MapSize map_size) {
    map_size_ = map_size;
    return *this;
}

RouteMap &RouteMap::SetPadding(double padding) {
    padding_ = padding;
    return *this;
}

RouteMap &RouteMap::SetLineWidth(double width) {
    line_width_ = width;
    return *this;
}

RouteMap& RouteMap::SetStopsRadius(double radius) {
    stop_radius_ = radius;
    return *this;
}

RouteMap &RouteMap::SetStopLabelFontSize(int size) {
    stop_label_font_size_ = size;
    return *this;
}

RouteMap &RouteMap::SetBusLabelFontSize(int size) {
    bus_label_font_size_ = size;
    return *this;
}

RouteMap &RouteMap::SetStopLabelOffset(double x, double y) {
    stop_label_offset_.x = x;
    stop_label_offset_.y = y;
    return *this;
}

RouteMap& RouteMap::SetBusLabelOffset(double x, double y) {
    bus_label_offset_.x = x;
    bus_label_offset_.y = y;
    return *this;
}

RouteMap& RouteMap::SetUnderLayerColor(const svg::Color& color) {
    underlayer_color_ = color;
    return *this;
}

RouteMap& RouteMap::SetUnderLayerWidth(double width) {
    underlayer_width_ = width;
    return *this;
}

void RouteMap::AddColorToPalette(const svg::Color& color) {
    color_palette_.push_back(color);
}

void RouteMap::DrawRoutesLines(svg::ObjectContainer &container, const Geo::SphereProjector& projector) const {
    for (auto& route : routes_) {
        svg::Polyline line;
        line.SetStrokeLineCap(svg::StrokeLineCap::ROUND).SetStrokeLineJoin(svg::StrokeLineJoin::ROUND).SetFillColor(svg::NoneColor).
        SetStrokeWidth(line_width_).SetStrokeColor(route.color_);

        for (auto& stop : route.bus_ptr_->route_) {
            line.AddPoint(projector.RescaleCoordinates(stop->coords_));
        }

        container.AddObject(line);
    }
}

void RouteMap::DrawRoutesNames(svg::ObjectContainer& container, const Geo::SphereProjector& projector) const {
    for (auto& route : routes_) {
        if (route.bus_ptr_->route_.empty()) {
            continue;
        }

        vector<svg::Text> texts(2);
        if (!route.bus_ptr_->is_round_) {
            texts.resize(4);
        }

        for (int i = 0; i < texts.size(); i += 2) {
            for (int d = i; d < (2 + i); d++) {
                if (i == 0) {
                    texts[d].SetPosition(projector.RescaleCoordinates(route.bus_ptr_->route_.front()->coords_));
                }
                else {
                    auto& pre_end_bus = route.bus_ptr_->route_[route.bus_ptr_->route_.size() - 2];
                    texts[d].SetPosition(projector.RescaleCoordinates(pre_end_bus->coords_));
                }

                texts[d].SetOffset(bus_label_offset_).SetFontSize(bus_label_font_size_).SetFontFamily(font_family_).
                SetFontWeight(font_route_weight_).SetData(route.bus_ptr_->name_);
            }
            
            texts[i].SetFillColor(route.color_);
            texts[i+1].SetFillColor(underlayer_color_).SetStrokeColor(underlayer_color_).
            SetStrokeWidth(underlayer_width_).SetStrokeLineCap(svg::StrokeLineCap::ROUND).SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);

            container.AddObject(texts[i+1]);
            container.AddObject(texts[i]);
        }
    }
}

void RouteMap::DrawStopCircles(svg::ObjectContainer& container, const Stop& stop, const Geo::SphereProjector& projector) const {
    svg::Circle circle;
    circle.SetCenter(projector.RescaleCoordinates(stop.coords_)).SetRadius(stop_radius_).SetFillColor(stop_circle_color_);
    container.AddObject(circle);
}

void RouteMap::DrawStopName(svg::ObjectContainer &container, const Stop& stop, const Geo::SphereProjector& projector) const {
    array<svg::Text, 2> texts;

    for (int i = 0; i < 2; i++) {
        texts[i].SetData(string(stop.name_)).SetFontSize(stop_label_font_size_).SetOffset(stop_label_offset_).
        SetPosition(projector.RescaleCoordinates(stop.coords_)).SetFontFamily(font_family_);
    }

    texts[0].SetFillColor(stop_text_fill_);
    texts[1].SetFillColor(underlayer_color_).SetStrokeColor(underlayer_color_).SetStrokeWidth(underlayer_width_).
    SetStrokeLineCap(svg::StrokeLineCap::ROUND).SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);

    container.AddObject(texts[1]); container.AddObject(texts[0]);
}

bool Route::operator<(const Route &other) const {
    return bus_ptr_->name_ < other.bus_ptr_->name_;
}
bool Route::operator<=(const Route &other) const {
    return !(*this > other);
}
bool Route::operator>(const Route &other) const {
    return bus_ptr_->name_ > other.bus_ptr_->name_;
}
bool Route::operator>=(const Route &other) const {
    return !(*this < other);
}
}