#include "stat_reader.h"
#include "input_reader.h"
#include <iomanip>
#include <cmath>
#include <algorithm>

using namespace std;

void ParseAndPrintStat(const TransportCatalogue &transport_catalogue, string_view request,
                       ostream &output)
{
    auto [command, data] = Parser::SplitToPair(request, ' ');
    
    if (command == "Bus") {
        const Bus* bus = transport_catalogue.FindBus(data);
        output << "Bus "s << data << ": "s;
        if (bus == nullptr) {
            output << "not found"s;
            return;
        }

        RouteStatistics statistics = transport_catalogue.GetRouteStatistics(data);
        output << statistics.stops_count_ << " stops on route, "s;
        output << statistics.unique_stops_count_ << " unique stops, "s;
        output << statistics.route_length_ << " route length, "s;
        output << statistics.curvature_ << " curvature"s;
        return;
    }

    output << "Stop "s << data << ": "s;
    if (transport_catalogue.FindStop(data) == nullptr) {
        output << "not found"s;
            return;
    }

    const auto& buses = transport_catalogue.FindBuses(data);
    if (buses == nullptr) {
        output << "no buses"s;
            return;
    }

    output << "buses"s;
    string buses_list;
    for (auto& bus_ptr : *buses) {
        output << ' ' << bus_ptr->name_;
    }
}

void ParseFullRequest(const TransportCatalogue &tansport_catalogue, istream& in, 
    ostream &output) {

    int stat_request_count;
    in >> stat_request_count >> ws;
    for (int i = 0; i < stat_request_count; ++i) {
        string line;
        getline(in, line);
        ParseAndPrintStat(tansport_catalogue, line, output);
        if (i < stat_request_count - 1) {
            output << '\n';
        }
    }
}
