#include "stat_reader.h"
#include <iomanip>
#include <cmath>

using namespace std;

void ParseAndPrintStat(const TransportCatalogue& tansport_catalogue, string_view request,
                       ostream& output) {
    request = request.substr(4);
    
    vector<string_view> route = tansport_catalogue.FindRoute(request);
    output << "Bus "s <<  request << ": "s;
    if (route.empty()) {
        output << "not found\n"s;
        return;
    }

    output << tansport_catalogue.GetCountStops(request) << " stops on route, "s;
    output << tansport_catalogue.GetCountUniqueStops(request) << " unique stops, "s;
    output << round(tansport_catalogue.GetRouteLength(request) * 1'000'000) / 1'000'000 
    << " route length\n"s;
}