#include "main_tests.h"
#ifdef DEBUG

#include "../transport_catalogue.h"

using namespace std;
using namespace Geo;

class TransportCatalogue_Testing : public TransportCatalogue {
    public:
    using TransportCatalogue::GetRouteLength;
    using TransportCatalogue::GetRealRouteLength;
};

DEFINE_TEST_GF(AddStop_Testing, TransportCatalogue_Tests, ExceptionFixture) {

    TransportCatalogue_Testing catalogue;
    Coordinates coord_1(0.1, 0.1);
    Coordinates coord_2(0.2, 0.2);
    Coordinates coord_3(0.3, 0.3);

    string name_1("Stop_1");
    string name_2("Stop_2");
    string name_3("Stop_3");

    (catalogue.AddStop(name_1, coord_1));
    TEST((*catalogue.FindStop(name_1) == Stop(name_1, Coordinates(0.1, 0.1)) ));

    catalogue.AddStop(name_2, coord_2);
    catalogue.AddStop(name_3, coord_3);

    TEST((*catalogue.FindStop(name_2) == Stop(name_2, Coordinates(0.2, 0.2)) ));
    TEST((*catalogue.FindStop(name_3) == Stop(name_3, Coordinates(0.3, 0.3)) ));
};

DEFINE_TEST_GF(AddBus_Testing, TransportCatalogue_Tests, ExceptionFixture) {
    TransportCatalogue_Testing catalogue;

    catalogue.AddStop("Stop_1"s, {0.1, 0.1});
    catalogue.AddStop("Stop_2"s, {0.2, 0.2});
    catalogue.AddStop("Stop_3"s, {0.3, 0.3});

    vector<string> route_1({"Stop_1"s, "Stop_2"s, "Stop_1"s});
    vector<string> route_2({"Stop_1"s, "Stop_2"s, "Stop_3"s});

    catalogue.AddRoute("Bus_1"s, 
    vector<string_view>(route_1.begin(), route_1.end()));

    catalogue.AddRoute("Bus_2"s, 
    vector<string_view>(route_2.begin(), route_2.end()));

    const Bus* bus_1 = catalogue.FindBus("Bus_1"s);
    const Bus* bus_2 = catalogue.FindBus("Bus_2"s);
    
    TEST(bus_1->name_ == "Bus_1");
    TEST(bus_2->name_ == "Bus_2");

    TEST(( VecPtrToVecNames(bus_1->route_) == 
    vector<string>({"Stop_1"s, "Stop_2"s, "Stop_1"s})));
    TEST(( VecPtrToVecNames(bus_2->route_) == 
    vector<string>({"Stop_1"s, "Stop_2"s, "Stop_3"s})));
}

DEFINE_TEST_G(GetCountUniqueStops_UniqueStopsOnly, TransportCatalogue_Tests)
{
    TransportCatalogue_Testing tc;

    tc.AddStop("Stop1", {55.7522, 37.6156});
    tc.AddStop("Stop2", {59.9343, 30.3351});
    tc.AddStop("Stop3", {40.7128, -74.0060});
    vector<string> str({"Stop1", "Stop2", "Stop3"});

    tc.AddNeighborStopDistance("Stop1"sv, "Stop2"sv, 100);
    tc.AddNeighborStopDistance("Stop2"sv, "Stop3"sv, 100);
    tc.AddNeighborStopDistance("Stop3"sv, "Stop1"sv, 100);
    
    tc.AddRoute("Bus1", 
    vector<string_view>(str.begin(), str.end()));

    RouteStatistics stat = tc.GetRouteStatistics("Bus1"s);

    TEST_EQ(stat.unique_stops_count_, (size_t)3);
}

DEFINE_TEST_G(GetCountUniqueStops_RepeatedStops, TransportCatalogue_Tests)
{
    TransportCatalogue_Testing tc;

    tc.AddStop("Stop1", {55.7522, 37.6156});
    tc.AddStop("Stop2", {59.9343, 30.3351});
    vector<string> str({"Stop1", "Stop2", "Stop1", "Stop2"});

    tc.AddRoute("Bus1", 
    vector<string_view>(str.begin(), str.end()));

    RouteStatistics stat = tc.GetRouteStatistics("Bus1"s);

    TEST_EQ(stat.unique_stops_count_, (size_t)2);
}

DEFINE_TEST_G(GetCountUniqueStops_SingleStop, TransportCatalogue_Tests)
{
    TransportCatalogue_Testing tc;

    tc.AddStop("Stop1", {55.7522, 37.6156});

    tc.AddRoute("Bus1", 
        vector<string_view>({"Stop1"sv}));

    RouteStatistics stat = tc.GetRouteStatistics("Bus1"s);

    TEST_EQ(stat.unique_stops_count_, (size_t)1);
}

DEFINE_TEST_G(GetCountUniqueStops_EmptyRoute, TransportCatalogue_Tests)
{
    TransportCatalogue_Testing tc;

    tc.AddRoute("Bus1", vector<string_view>());

    RouteStatistics stat = tc.GetRouteStatistics("Bus1"s);

    TEST_EQ(stat.unique_stops_count_, (size_t)0);
}

DEFINE_TEST_G(GetRouteLength_SimpleRoute, TransportCatalogue_Tests)
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

    TEST(IsEqualDouble(route_length, computed_dist));
}

DEFINE_TEST_G(GetRouteLength_RouteWithRepeatedStops, TransportCatalogue_Tests)
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

    TEST(IsEqualDouble(route_length, computed_dist));
}

DEFINE_TEST_G(GetRouteLength_ComplexRoute, TransportCatalogue_Tests)
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

    TEST(IsEqualDouble(route_length, computed_dist));
}

DEFINE_TEST_G(GetRouteLength_SingleStopRoute, TransportCatalogue_Tests)
{
    TransportCatalogue_Testing tc;

    tc.AddStop("Stop1", {1.0, 1.0});
    vector<string> str({"Stop1", "Stop1"});
    tc.AddRoute("Bus1", vector<string_view>(str.begin(), str.end()));

    double route_length = tc.GetRouteLength("Bus1"s);

    TEST(IsEqualDouble(route_length, 0.0));
}

DEFINE_TEST_G(GetRouteLength_EmptyRoute, TransportCatalogue_Tests)
{
    TransportCatalogue_Testing tc;

    tc.AddRoute("Bus1", vector<string_view>());

    double route_length = tc.GetRouteLength("Bus1"s);

    TEST(IsEqualDouble(route_length, 0.0));
}

DEFINE_TEST_G(AddNeighborStopDistance_SymmetricAddition, TransportCatalogue_Tests) {
    TransportCatalogue_Testing catalogue;

    const Stop& stop1 = catalogue.AddStop("StopA", {55.0, 37.0});
    const Stop& stop2 = catalogue.AddStop("StopB", {55.1, 37.1});

    catalogue.AddNeighborStopDistance("StopA", "StopB", 500);

    auto itA = stop1.neighbor_stops_dist_.find({const_cast<Stop*>(&stop2)});
    TEST_NEQ(itA, stop1.neighbor_stops_dist_.end());
    TEST_EQ(itA->second, 500);

    auto itB = stop2.neighbor_stops_dist_.find({const_cast<Stop*>(&stop1)});
    TEST_NEQ(itB, stop2.neighbor_stops_dist_.end());
    TEST_EQ(itB->second, 500);
}

DEFINE_TEST_G(AddNeighborStopDistance_NoOverwriteExisting, TransportCatalogue_Tests) {
    TransportCatalogue_Testing catalogue;

    const Stop& stop1 = catalogue.AddStop("StopA", {55.0, 37.0});
    const Stop& stop2 = catalogue.AddStop("StopB", {55.1, 37.1});

    catalogue.AddNeighborStopDistance("StopA", "StopB", 500);
    catalogue.AddNeighborStopDistance("StopB", "StopA", 700);

    auto itA = stop1.neighbor_stops_dist_.find({const_cast<Stop*>(&stop2)});
    TEST_NEQ(itA, stop1.neighbor_stops_dist_.end());
    TEST_EQ(itA->second, 500);

    auto itB = stop2.neighbor_stops_dist_.find({const_cast<Stop*>(&stop1)});
    TEST_NEQ(itB, stop2.neighbor_stops_dist_.end());
    TEST_EQ(itB->second, 700);
}

DEFINE_TEST_G(AddNeighborStopDistance_MultipleNeighbors, TransportCatalogue_Tests) {
    TransportCatalogue_Testing catalogue;

    const Stop& stop1 = catalogue.AddStop("StopA", {55.0, 37.0});
    const Stop& stop2 = catalogue.AddStop("StopB", {55.1, 37.1});
    const Stop& stop3 = catalogue.AddStop("StopC", {55.2, 37.2});

    catalogue.AddNeighborStopDistance("StopA", "StopB", 400);
    catalogue.AddNeighborStopDistance("StopA", "StopC", 600);

    TEST_EQ(stop1.neighbor_stops_dist_.size(), 2);
    TEST(stop1.neighbor_stops_dist_.contains(const_cast<Stop*>(&stop2)));
    TEST(stop1.neighbor_stops_dist_.contains(const_cast<Stop*>(&stop3)));

    auto to_stop_2 = stop1.neighbor_stops_dist_.find(const_cast<Stop*>(&stop2));
    auto to_stop_3 = stop1.neighbor_stops_dist_.find(const_cast<Stop*>(&stop3));
    TEST_EQ(to_stop_2->second, 400);
    TEST_EQ(to_stop_3->second, 600);

    TEST(stop2.neighbor_stops_dist_.contains(const_cast<Stop*>(&stop1)));
    TEST(stop3.neighbor_stops_dist_.contains(const_cast<Stop*>(&stop1)));

    auto stop_2_to_stop_1 = stop2.neighbor_stops_dist_.find(const_cast<Stop*>(&stop1));
    auto stop_3_to_stop_1 = stop3.neighbor_stops_dist_.find(const_cast<Stop*>(&stop1));
    TEST_EQ(stop_2_to_stop_1->second, 400);
    TEST_EQ(stop_3_to_stop_1->second, 600);
}

DEFINE_TEST_G(GetRealRouteLength_SimpleRoute, TransportCatalogue_Tests) {
    TransportCatalogue_Testing catalogue;

    catalogue.AddStop("A", {55.0, 37.0});
    catalogue.AddStop("B", {55.1, 37.1});
    catalogue.AddStop("C", {55.2, 37.2});

    catalogue.AddNeighborStopDistance("A"sv, "B"sv, 100);
    catalogue.AddNeighborStopDistance("B"sv, "C"sv, 200);

    catalogue.AddRoute("Bus1", {"A", "B", "C"});

    TEST_EQ(catalogue.GetRealRouteLength("Bus1"), 300);
}

DEFINE_TEST_G(GetRealRouteLength_CircularRoute, TransportCatalogue_Tests) {
    TransportCatalogue_Testing catalogue;

    catalogue.AddStop("A", {55.0, 37.0});
    catalogue.AddStop("B", {55.1, 37.1});
    catalogue.AddStop("C", {55.2, 37.2});

    catalogue.AddNeighborStopDistance("A"sv, "B"sv, 150);
    catalogue.AddNeighborStopDistance("B"sv, "C"sv, 250);
    catalogue.AddNeighborStopDistance("C"sv, "A"sv, 350);

    catalogue.AddRoute("CircularBus", {"A", "B", "C", "A"});

    TEST_EQ(catalogue.GetRealRouteLength("CircularBus"), 750);
}

DEFINE_TEST_G(GetRealRouteLength_SingleStop, TransportCatalogue_Tests) {
    TransportCatalogue_Testing catalogue;

    catalogue.AddStop("A", {55.0, 37.0});

    catalogue.AddRoute("SingleStopBus", {"A"});

    TEST_EQ(catalogue.GetRealRouteLength("SingleStopBus"), 0);
}

#endif