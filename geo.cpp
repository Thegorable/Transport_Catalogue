#include "geo.h"

using namespace std;

namespace Geo {
    Coordinates::Coordinates() = default;
    Coordinates::Coordinates(const double l, const double r) : lat(l), lng(r) {}
    Coordinates::Coordinates(const Coordinates& coord) : lat(coord.lat), lng(coord.lng) {}
    Coordinates::Coordinates(Coordinates&& coord) : lat(coord.lat), lng(coord.lng) {}

    bool Coordinates::operator==(const Coordinates& other) const {
        return IsEqualDouble(lat, other.lat) && IsEqualDouble(lng, other.lng);
    }
    bool Coordinates::operator!=(const Coordinates& other) const {
        return !(*this == other);
    }

    Coordinates& Coordinates::operator=(const Coordinates& other) {
        lat = other.lat;
        lng = other.lng;
        return *this;
    }

    svg::Point SphereProjector::RescaleCoordinates(Coordinates coords) const {
        return {
            (coords.lng - min_lon_) * zoom_coeff_ + padding_,
            (max_lat_ - coords.lat) * zoom_coeff_ + padding_
        };
    }
}