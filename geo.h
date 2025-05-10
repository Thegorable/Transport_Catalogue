#pragma once
#include <cmath>
#include <iostream>
#include <algorithm>
#include <optional>

#include "svg.h"

const double PI = 3.1415926535;
const int EARTH_RADIUS = 6371000;

namespace Utility
{

template<std::integral integer>
integer GetCycleInterger(integer start, integer end, integer input) {
    if (input == start) {
        return 0;
    }
    return (input - start) % (end - start);
}

template<std::integral integer>
class CycleIndex {
    public:
        explicit CycleIndex(integer max) : max_(max) {}
    
        CycleIndex& operator++() {
            if (++current_ >= max_) {
                current_ = 0;
            }
            return *this;
        }
    
        [[nodiscard]] inline integer Get() const {
            return current_;
        }
    
    private:
        integer max_;
        integer current_ = 0;
    };
    
}


namespace Geo {
    
    inline bool IsZero(double value) {
        return std::abs(value) < std::numeric_limits<double>::epsilon();
    }
    
    inline bool IsEqualDouble(double l, double r, int precision = 6) {
        return std::abs(l - r) < std::pow(10.0, -precision);
    }

    struct Coordinates {
        Coordinates();
        Coordinates(const double l, const double r);
        Coordinates(const Coordinates& coord);
        Coordinates(Coordinates&& coord);

        double lat = 0.0;
        double lng = 0.0;
        
        bool operator==(const Coordinates& other) const;
        bool operator!=(const Coordinates& other) const;
        Coordinates& operator=(const Coordinates& other);
    };

    inline std::ostream& operator <<(std::ostream& os, Coordinates coord) {
        os << coord.lat << ", " << coord.lng;
        return os;
    }

    inline double ComputeDistance(Coordinates from, Coordinates to) {
        using namespace std;
        if (from == to) {
            return 0.0;
        }
        static const double dr = PI / 180.;
        return acos(sin(from.lat * dr) * sin(to.lat * dr)
                    + cos(from.lat * dr) * cos(to.lat * dr) * cos(abs(from.lng - to.lng) * dr))
            * EARTH_RADIUS;
    }


    class SphereProjector {
    public:
        template <std::forward_iterator CoordinatesIt>
        SphereProjector(CoordinatesIt points_begin, CoordinatesIt points_end,
                        double max_width, double max_height, double padding);

        svg::Point RescaleCoordinates(Coordinates coords) const;

    private:
        double padding_ = 0.0;
        double min_lon_ = 0;
        double max_lat_ = 0;
        double zoom_coeff_ = 0;
    };

    template <std::forward_iterator CoordinatesIt>
    inline SphereProjector::SphereProjector(CoordinatesIt points_begin, CoordinatesIt points_end,
        double max_width, double max_height, double padding)
    : padding_(padding) {

    if (points_begin == points_end) {
        return;
    }

    const auto [left_it, right_it] = std::minmax_element(
    points_begin, points_end,
    [](auto lhs, auto rhs) { return lhs.lng < rhs.lng; });

    min_lon_ = left_it->lng;
    const double max_lon = right_it->lng;

    const auto [bottom_it, top_it] = std::minmax_element(
    points_begin, points_end,
    [](auto lhs, auto rhs) { return lhs.lat < rhs.lat; });

    const double min_lat = bottom_it->lat;
    max_lat_ = top_it->lat;

    std::optional<double> width_zoom;
    if (!IsZero(max_lon - min_lon_)) {
        width_zoom = (max_width - 2 * padding) / (max_lon - min_lon_);
    }

    std::optional<double> height_zoom;
    if (!IsZero(max_lat_ - min_lat)) {
        height_zoom = (max_height - 2 * padding) / (max_lat_ - min_lat);
    }

    if (width_zoom && height_zoom) {
        zoom_coeff_ = std::min(*width_zoom, *height_zoom);
    } else if (width_zoom) {
        zoom_coeff_ = *width_zoom;
    } else if (height_zoom) {
        zoom_coeff_ = *height_zoom;
    }

}

}