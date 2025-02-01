#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>
#include <stdexcept>

#include "geo.h"

using namespace std::string_literals;

struct Stop {
	Stop() = default;
	Stop(const string& name, Geo::Coordinates coords) : name_(name), coords_(coords) {}
	Stop(string&& name, Geo::Coordinates coords) : name_(move(name)), coords_(coords) {}

	std::string name_;
	Geo::Coordinates coords_;
};

struct StringHasher {
	using is_transparent = int;

	std::size_t operator() (const std::string& str) const {
		return std::hash<std::string> {} (str);
	}

	std::size_t operator() (const std::string_view& str) const {
		return std::hash<std::string_view> {} (str);
	}
};

struct StringComparator {
	using is_transparent = int;

	bool operator () (const std::string& l, const std::string& r) const {
		return l == r;
	}

	bool operator () (const std::string_view& l, const std::string_view& r) const {
		return l == r;
	}

	bool operator () (const std::string& l, const std::string_view& r) const {
		return l == r;
	}

	bool operator () (const std::string_view& l, const std::string& r) const {
		return l == r;
	}
};

template <typename T>
std::string VectorToString(const std::vector<T>& v) {
    if (v.empty()) {
        return "";
    }

    std::stringstream ss;
    for (const auto& elem : v) {
        ss << elem << ", ";
    }

    std::string result = ss.str();
    result.resize(result.size() - 2);
    return result;
}

class TransportCatalogue {

public:
	TransportCatalogue() = default;

	void AddStop(const std::string& stop, Geo::Coordinates coordinates); // tested
	void AddStop(std::string&& stop, Geo::Coordinates coordinates); // tested

	void AddRoute(const std::string& bus, const std::vector<std::string>& route); // tested
	void AddRoute(const std::string& bus, const std::vector<std::string_view>& route);

	inline size_t GetCountStops(const std::string_view& bus) const {
		auto it = buses_.find(bus); 
		if (it == buses_.end()) {
			throw std::out_of_range("The element "s + std::string(bus) + " doesn't exists"s);
		}

		return it->second.size();
	}

	inline size_t GetCountAllStops() const {return stops_.size(); }
	inline size_t GetCountAllbuses() const {return buses_.size(); }

	size_t GetCountUniqueStops(const std::string_view& bus) const; // tested
	double GetRouteLength(const std::string_view& bus) const; // tested

	inline std::vector<std::string_view> FindRoute(const std::string_view& bus) const {
		auto it = buses_.find(bus); 
		if (it == buses_.end()) {
			return {};
		}

		return it->second;
	}

	inline bool IsContainsBus(const std::string_view& bus) const {
		return buses_.contains(bus);
	}

	inline Geo::Coordinates FindStop(const std::string_view& stop) const {
		auto it = stops_.find(stop); 
		if (it == stops_.end()) {
			return {};
		}

		return it->second;
	}
	inline Geo::Coordinates FindStop(const std::string& stop) const
	{ return FindStop(std::string_view(stop)); }

	inline bool IsContainsStop(const std::string_view& stop) const {
		return stops_.contains(stop);
	}

	std::vector<std::string_view> FindBuses(const std::string_view& bus) const;

protected:
	std::unordered_map<std::string, Geo::Coordinates, StringHasher, StringComparator> stops_;
	std::unordered_map<std::string, std::vector<std::string_view>, 
	StringHasher, StringComparator> buses_;

	std::pair<bool, std::vector<std::string_view>> IsContainsFullRoute(
		const std::vector<std::string>& route) const;
	std::pair<bool, std::vector<std::string_view>> IsContainsFullRoute(
		const std::vector<std::string_view>& route) const;
};