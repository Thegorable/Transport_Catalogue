#include "main_tests.h"
#ifdef DEBUG

#include <boost\test\unit_test.hpp>
#include "../transport_catalogue.h"

using namespace std;
using namespace Geo;

class TransportCatalogue_Testing : public TransportCatalogue {
    public:
    using TransportCatalogue::GetRouteLength;
};

BOOST_AUTO_TEST_CASE(AddStop_Testing) {

    TransportCatalogue_Testing catalogue;
    Coordinates coord_1(0.1, 0.1);
    Coordinates coord_2(0.2, 0.2);
    Coordinates coord_3(0.3, 0.3);

    string name_1("Stop_1");
    string name_2("Stop_2");
    string name_3("Stop_3");

    BOOST_CHECK_NO_THROW(catalogue.AddStop(name_1, coord_1));
    BOOST_TEST((*catalogue.FindStop(name_1) == Stop(name_1, Coordinates(0.1, 0.1)) ));

    catalogue.AddStop(name_2, coord_2);
    catalogue.AddStop(name_3, coord_3);

    BOOST_TEST((*catalogue.FindStop(name_2) == Stop(name_2, Coordinates(0.2, 0.2)) ));
    BOOST_TEST((*catalogue.FindStop(name_3) == Stop(name_3, Coordinates(0.3, 0.3)) ));
};

BOOST_AUTO_TEST_CASE(AddBus_Testing) {
    TransportCatalogue_Testing catalogue;

    catalogue.AddStop("Stop_1"s, {0.1, 0.1});
    catalogue.AddStop("Stop_2"s, {0.2, 0.2});
    catalogue.AddStop("Stop_3"s, {0.3, 0.3});

    vector<string> route_1({"Stop_1"s, "Stop_2"s, "Stop_1"s});
    vector<string> route_2({"Stop_1"s, "Stop_2"s, "Stop_3"s});

    BOOST_CHECK_NO_THROW(catalogue.AddRoute("Bus_1"s, 
    vector<string_view>(route_1.begin(), route_1.end())));

    BOOST_CHECK_NO_THROW(catalogue.AddRoute("Bus_2"s, 
    vector<string_view>(route_2.begin(), route_2.end())));

    const Bus* bus_1 = catalogue.FindBus("Bus_1"s);
    const Bus* bus_2 = catalogue.FindBus("Bus_2"s);
    
    BOOST_CHECK(bus_1->name_ == "Bus_1");
    BOOST_CHECK(bus_2->name_ == "Bus_2");

    BOOST_CHECK(( VecPtrToVecNames(bus_1->route_) == 
    vector<string>({"Stop_1"s, "Stop_2"s, "Stop_1"s})));
    BOOST_CHECK(( VecPtrToVecNames(bus_2->route_) == 
    vector<string>({"Stop_1"s, "Stop_2"s, "Stop_3"s})));
}

BOOST_AUTO_TEST_SUITE(GetCountUniqueStops_Testing);

BOOST_AUTO_TEST_CASE(GetCountUniqueStops_UniqueStopsOnly)
{
    TransportCatalogue_Testing tc;

    tc.AddStop("Stop1", {55.7522, 37.6156});
    tc.AddStop("Stop2", {59.9343, 30.3351});
    tc.AddStop("Stop3", {40.7128, -74.0060});
    vector<string> str({"Stop1", "Stop2", "Stop3"});
    
    tc.AddRoute("Bus1", 
    vector<string_view>(str.begin(), str.end()));

    RouteStatistics stat = tc.GetRouteStatistics("Bus1"s);

    BOOST_CHECK_EQUAL(stat.unique_stops_count_, (size_t)3);
}

BOOST_AUTO_TEST_CASE(GetCountUniqueStops_RepeatedStops)
{
    TransportCatalogue_Testing tc;

    tc.AddStop("Stop1", {55.7522, 37.6156});
    tc.AddStop("Stop2", {59.9343, 30.3351});
    vector<string> str({"Stop1", "Stop2", "Stop1", "Stop2"});

    tc.AddRoute("Bus1", 
    vector<string_view>(str.begin(), str.end()));

    RouteStatistics stat = tc.GetRouteStatistics("Bus1"s);

    BOOST_CHECK_EQUAL(stat.unique_stops_count_, (size_t)2);
}

BOOST_AUTO_TEST_CASE(GetCountUniqueStops_SingleStop)
{
    TransportCatalogue_Testing tc;

    tc.AddStop("Stop1", {55.7522, 37.6156});
    vector<string> str({"Stop1", "Stop1", "Stop1"});
    tc.AddRoute("Bus1", vector<string_view>(str.begin(), str.end()));

    RouteStatistics stat = tc.GetRouteStatistics("Bus1"s);

    BOOST_CHECK_EQUAL(stat.unique_stops_count_, (size_t)1);
}

BOOST_AUTO_TEST_CASE(GetCountUniqueStops_EmptyRoute)
{
    TransportCatalogue_Testing tc;

    tc.AddRoute("Bus1", vector<string_view>());

    RouteStatistics stat = tc.GetRouteStatistics("Bus1"s);

    BOOST_CHECK_EQUAL(stat.unique_stops_count_, (size_t)0);
}

BOOST_AUTO_TEST_SUITE_END();

BOOST_AUTO_TEST_SUITE(GetRouteLength_Testing);

BOOST_AUTO_TEST_CASE(GetRouteLength_SimpleRoute)
{
    TransportCatalogue_Testing tc;

    tc.AddStop("Stop1", {1.0, 1.0});
    tc.AddStop("Stop2", {1.0, 2.0});
    tc.AddStop("Stop3", {2.0, 2.0});
    vector<string> str({"Stop1", "Stop2", "Stop3"});
    tc.AddRoute("Bus1", vector<string_view>(str.begin(), str.end()));

    double route_length = tc.GetRouteLength("Bus1"s);

    double computed_dist = ComputeDistance({1.0, 1.0}, {1.0, 2.0});
    computed_dist += ComputeDistance({1.0, 2.0}, {2.0, 2.0});

    BOOST_CHECK(IsEqualDouble(route_length, computed_dist));
}

BOOST_AUTO_TEST_CASE(GetRouteLength_RouteWithRepeatedStops)
{
    TransportCatalogue_Testing tc;

    tc.AddStop("Stop1", {1.0, 1.0});
    tc.AddStop("Stop2", {1.0, 2.0});
    vector<string> str({"Stop1", "Stop2", "Stop1", "Stop2", "Stop1"});
    tc.AddRoute("Bus1", vector<string_view>(str.begin(), str.end()));

    double route_length = tc.GetRouteLength("Bus1"s);

    double computed_dist = ComputeDistance({1.0, 1.0}, {1.0, 2.0});
    computed_dist += ComputeDistance({1.0, 2.0}, {1.0, 1.0});
    computed_dist += ComputeDistance({1.0, 1.0}, {1.0, 2.0});
    computed_dist += ComputeDistance({1.0, 2.0}, {1.0, 1.0});

    BOOST_CHECK(IsEqualDouble(route_length, computed_dist));
}

BOOST_AUTO_TEST_CASE(GetRouteLength_ComplexRoute)
{
    TransportCatalogue_Testing tc;

    tc.AddStop("Stop1", {1.0, 1.0});
    tc.AddStop("Stop2", {1.0, 1.5});
    tc.AddStop("Stop3", {1.5, 1.5});
    tc.AddStop("Stop4", {1.5, 2.0});
    vector<string> str({"Stop1", "Stop2", "Stop3", "Stop4",
                        "Stop3", "Stop2", "Stop1"});
    tc.AddRoute("Bus1", vector<string_view>(str.begin(), str.end()));

    double route_length = tc.GetRouteLength("Bus1"s);

    double computed_dist = ComputeDistance({1.0, 1.0}, {1.0, 1.5});
    computed_dist += ComputeDistance({1.0, 1.5}, {1.5, 1.5});
    computed_dist += ComputeDistance({1.5, 1.5}, {1.5, 2.0});
    computed_dist += ComputeDistance({1.5, 2.0}, {1.5, 1.5});
    computed_dist += ComputeDistance({1.5, 1.5}, {1.0, 1.5});
    computed_dist += ComputeDistance({1.0, 1.5}, {1.0, 1.0});

    BOOST_CHECK(IsEqualDouble(route_length, computed_dist));
}

BOOST_AUTO_TEST_CASE(GetRouteLength_SingleStopRoute)
{
    TransportCatalogue_Testing tc;

    tc.AddStop("Stop1", {1.0, 1.0});
    vector<string> str({"Stop1", "Stop1"});
    tc.AddRoute("Bus1", vector<string_view>(str.begin(), str.end()));

    double route_length = tc.GetRouteLength("Bus1"s);

    BOOST_CHECK(IsEqualDouble(route_length, 0.0));
}

BOOST_AUTO_TEST_CASE(GetRouteLength_EmptyRoute)
{
    TransportCatalogue_Testing tc;

    tc.AddRoute("Bus1", vector<string_view>());

    double route_length = tc.GetRouteLength("Bus1"s);

    BOOST_CHECK(IsEqualDouble(route_length, 0.0));
}

BOOST_AUTO_TEST_SUITE_END();

#endif