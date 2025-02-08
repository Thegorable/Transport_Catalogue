#pragma once
#include <cmath>
#include <iostream>

const double PI = 3.1415926535;
const int EARTH_RADIUS = 6371000;

namespace Geo {

    inline bool IsEqualDouble(double l, double r, int precision = 6) {
        return std::abs(l - r) < std::pow(10.0, -precision);
    }

    struct Coordinates {
        Coordinates() = default;
        Coordinates(const double l, const double r) : lat(l), lng(r) {}
        Coordinates(const Coordinates& coord) : lat(coord.lat), lng(coord.lng) {}
        Coordinates(Coordinates&& coord) : lat(coord.lat), lng(coord.lng) {}

        double lat = 0.0;
        double lng = 0.0;
        
        bool operator==(const Coordinates& other) const {
            return IsEqualDouble(lat, other.lat) && IsEqualDouble(lng, other.lng);
        }
        bool operator!=(const Coordinates& other) const {
            return !(*this == other);
        }

        Coordinates& operator=(const Coordinates& other) {
            lat = other.lat;
            lng = other.lng;
            return *this;
        }
    };

    inline std::ostream& operator <<(std::ostream& os, Coordinates coord) {
        os << coord.lat << ", " << coord.lng;
        return os;
    }

    inline double ComputeDistance(Coordinates from, Coordinates to) {
        using namespace std;
        if (from == to) {
            return 0;
        }
        static const double dr = PI / 180.;
        return acos(sin(from.lat * dr) * sin(to.lat * dr)
                    + cos(from.lat * dr) * cos(to.lat * dr) * cos(abs(from.lng - to.lng) * dr))
            * EARTH_RADIUS;
    }

}