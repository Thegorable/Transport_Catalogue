#pragma once
#include <cmath>
#include <initializer_list>
#include <iostream>

inline bool IsEqualDouble(double l, double r, int precision = 6) {
    return std::abs(l - r) < std::pow(10.0, -precision);
}

struct Coordinates {
    Coordinates() = default;
    Coordinates(const double l, const double r) : lat(l), lng(r) {}
    Coordinates(const Coordinates& coord) : lat(coord.lat), lng(coord.lng) {}
    Coordinates(Coordinates&& coord) : lat(coord.lat), lng(coord.lng) {}
    Coordinates(std::initializer_list<double> list) :
    lat(*list.begin()), lng(*(list.begin() + 1)) {};

    double lat;
    double lng;
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
    static const double dr = 3.1415926535 / 180.;
    return acos(sin(from.lat * dr) * sin(to.lat * dr)
                + cos(from.lat * dr) * cos(to.lat * dr) * cos(abs(from.lng - to.lng) * dr))
        * 6371000;
}