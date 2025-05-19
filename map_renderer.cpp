#include "map_renderer.h"
#include <vector>
#include <array>

using namespace std;

namespace map_renderer {

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
    CoordinatesIt& operator ++() {it_++; return *this;}

    bool operator ==(const CoordinatesIt& other) const {return it_ == other.it_; }
    bool operator !=(const CoordinatesIt& other) const {return !(*this == other); }

private:
    std::set<const Stop*>::const_iterator it_;
};

    
void MapRenderer::Draw(svg::ObjectContainer &container) const {
    Geo::SphereProjector projector(CoordinatesIt(props_.stops_ptrs_.begin()), CoordinatesIt(props_.stops_ptrs_.end()),
    props_.map_size_.width_, props_.map_size_.height_, props_.padding_);

    DrawRoutesLines(container, projector);
    DrawRoutesNames(container, projector);
    for (auto& stop_ptr : props_.stops_ptrs_) {
        DrawStopCircles(container, *stop_ptr, projector);   
    }
    for (auto& stop_ptr : props_.stops_ptrs_) {
        DrawStopName(container, *stop_ptr, projector);
    }
}

MapRenderer::MapRenderer() = default;

const Route& MapRenderer::AddRoute(const Bus *bus_ptr) {
    Route new_route;
    new_route.bus_ptr_ = bus_ptr;
    for (auto& stop : bus_ptr->route_) {
        props_.stops_ptrs_.insert(stop);
    }
    auto result = props_.routes_.insert({&(bus_ptr->name_), move(new_route)});
    if (!result.second) {
        throw runtime_error("The adding new route was unsucsessful"s);
    }
    return result.first->second;
}

void MapRenderer::ReorderRouteColors() {
    size_t i = 0;
    for (auto& [name_ptr, route] : props_.routes_) {
        route.color_ = props_.color_palette_.at(Utility::GetCycleInterger(size_t(0), props_.color_palette_.size(), i));
        i++;
    }
}

MapRenderer &MapRenderer::SetMapSize(MapSize map_size) {
    props_.map_size_ = map_size;
    return *this;
}

MapRenderer &MapRenderer::SetPadding(double padding) {
    props_.padding_ = padding;
    return *this;
}

MapRenderer &MapRenderer::SetLineWidth(double width) {
    props_.line_width_ = width;
    return *this;
}

MapRenderer& MapRenderer::SetStopsRadius(double radius) {
    props_.stop_radius_ = radius;
    return *this;
}

MapRenderer &MapRenderer::SetStopLabelFontSize(int size) {
    props_.stop_label_font_size_ = size;
    return *this;
}

MapRenderer &MapRenderer::SetBusLabelFontSize(int size) {
    props_.bus_label_font_size_ = size;
    return *this;
}

MapRenderer &MapRenderer::SetStopLabelOffset(double x, double y) {
    props_.stop_label_offset_.x = x;
    props_.stop_label_offset_.y = y;
    return *this;
}

MapRenderer& MapRenderer::SetBusLabelOffset(double x, double y) {
    props_.bus_label_offset_.x = x;
    props_.bus_label_offset_.y = y;
    return *this;
}

MapRenderer& MapRenderer::SetUnderLayerColor(const svg::Color& color) {
    props_.underlayer_color_ = color;
    return *this;
}

MapRenderer& MapRenderer::SetUnderLayerWidth(double width) {
    props_.underlayer_width_ = width;
    return *this;
}

void MapRenderer::AddColorToPalette(const svg::Color& color) {
    props_.color_palette_.push_back(color);
}

void MapRenderer::DrawRoutesLines(svg::ObjectContainer &container, const Geo::SphereProjector& projector) const {
    for (auto& [name_bus_ptr, route] : props_.routes_) {
        svg::Polyline line;
        line.SetStrokeLineCap(svg::StrokeLineCap::ROUND).SetStrokeLineJoin(svg::StrokeLineJoin::ROUND).SetFillColor(svg::NoneColor).
        SetStrokeWidth(props_.line_width_).SetStrokeColor(route.color_);

        for (auto& stop : route.bus_ptr_->route_) {
            svg::Point new_coords = projector.RescaleCoordinates(stop->coords_);
            line.AddPoint(new_coords);
        }

        container.AddObject(line);
    }
}

void MapRenderer::DrawRoutesNames(svg::ObjectContainer& container, const Geo::SphereProjector& projector) const {
    for (auto& [name_bus_ptr, route] : props_.routes_) {
        if (route.bus_ptr_->route_.empty()) {
            continue;
        }

        vector<svg::Text> texts(2);
        auto& bus = *route.bus_ptr_;
        size_t mid_id = bus.route_.size() / 2;
        if (!bus.is_round_ && (bus.route_.front() != bus.route_[mid_id])) {
            texts.resize(4);
        }

        for (int i = 0; i < texts.size(); i += 2) {
            for (int d = i; d < (2 + i); d++) {
                if (i == 0) {
                    texts[d].SetPosition(projector.RescaleCoordinates(route.bus_ptr_->route_.front()->coords_));
                }
                else {
                    auto& pre_end_bus = route.bus_ptr_->route_[route.bus_ptr_->route_.size() / 2];
                    texts[d].SetPosition(projector.RescaleCoordinates(pre_end_bus->coords_));
                }

                texts[d].SetOffset(props_.bus_label_offset_).SetFontSize(props_.bus_label_font_size_).SetFontFamily(props_.font_family_).
                SetFontWeight(props_.font_route_weight_).SetData(route.bus_ptr_->name_);
            }
            
            texts[i].SetFillColor(route.color_);
            texts[i+1].SetFillColor(props_.underlayer_color_).SetStrokeColor(props_.underlayer_color_).
            SetStrokeWidth(props_.underlayer_width_).SetStrokeLineCap(svg::StrokeLineCap::ROUND).SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);

            container.AddObject(texts[i+1]);
            container.AddObject(texts[i]);
        }
    }
}

void MapRenderer::DrawStopCircles(svg::ObjectContainer& container, const Stop& stop, const Geo::SphereProjector& projector) const {
    svg::Circle circle;
    circle.SetCenter(projector.RescaleCoordinates(stop.coords_)).SetRadius(props_.stop_radius_).SetFillColor(props_.stop_circle_color_);
    container.AddObject(circle);
}

void MapRenderer::DrawStopName(svg::ObjectContainer &container, const Stop& stop, const Geo::SphereProjector& projector) const {
    array<svg::Text, 2> texts;

    for (int i = 0; i < 2; i++) {
        texts[i].SetData(string(stop.name_)).SetFontSize(props_.stop_label_font_size_).SetOffset(props_.stop_label_offset_).
        SetPosition(projector.RescaleCoordinates(stop.coords_)).SetFontFamily(props_.font_family_);
    }

    texts[0].SetFillColor(props_.stop_text_fill_);
    texts[1].SetFillColor(props_.underlayer_color_).SetStrokeColor(props_.underlayer_color_).SetStrokeWidth(props_.underlayer_width_).
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