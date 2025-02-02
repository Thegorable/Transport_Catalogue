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
        const Bus& bus = transport_catalogue.FindBus(data);
        output << "Bus "s << data << ": "s;
        if (bus.IsEmpty()) {
            output << "not found\n"s;
            return;
        }

        output << transport_catalogue.GetCountStops(data) << " stops on route, "s;
        output << transport_catalogue.GetCountUniqueStops(data) << " unique stops, "s;
        output << round(transport_catalogue.GetRouteLength(data) 
        * 1'000'000) / 1'000'000 << " route length\n"s;
        return;
    }

    output << "Stop "s << data << ": "s;
    if (!transport_catalogue.IsContainsStop(data)) {
        output << "not found\n"s;
            return;
    }

    vector<string_view> buses = transport_catalogue.FindBuses(data);
    if (buses.empty()) {
        output << "no buses\n"s;
            return;
    }
    sort(buses.begin(), buses.end());

    output << "buses"s;
    string buses_list;
    for (const string_view& bus : buses) {
        output << ' ' << bus;
    }
    output << "\n"s;
}