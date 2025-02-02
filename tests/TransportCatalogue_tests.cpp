#include "main_tests.h"
#ifdef DEBUG

#include <boost\test\unit_test.hpp>
#include "../transport_catalogue.h"

using namespace std;
using namespace Geo;

class TestableTransportCatalogue : public TransportCatalogue {
public:
    using TransportCatalogue::IsContainsFullRoute;
};

BOOST_AUTO_TEST_CASE(AddStop_Testing) {

    TransportCatalogue catalogue;
    Coordinates coord_1(0.1, 0.1);
    Coordinates coord_2(0.2, 0.2);
    Coordinates coord_3(0.3, 0.3);

    string name_1("Stop_1");
    string name_2("Stop_2");
    string name_3("Stop_3");

    BOOST_CHECK_EQUAL(catalogue.GetCountAllStops(), (size_t)0);
    BOOST_CHECK_NO_THROW(catalogue.AddStop(name_1, coord_1));

    BOOST_CHECK_EQUAL(catalogue.GetCountAllStops(), (size_t)1);
    BOOST_TEST((catalogue.FindStop(name_1) == Stop(name_1, Coordinates(0.1, 0.1)) ));

    catalogue.AddStop(name_2, coord_2);
    catalogue.AddStop(name_3, coord_3);

    BOOST_CHECK_EQUAL(catalogue.GetCountAllStops(), (size_t)3);
    BOOST_TEST((catalogue.FindStop(name_2) == Stop(name_2, Coordinates(0.2, 0.2)) ));
    BOOST_TEST((catalogue.FindStop(name_3) == Stop(name_3, Coordinates(0.3, 0.3)) ));
};

BOOST_AUTO_TEST_CASE(AddStop_Rvalue_Testing) {
    TransportCatalogue catalogue;

    Coordinates coord_1(0.1, 0.1);

    string name_1("Stop_1");

    BOOST_CHECK_NO_THROW(catalogue.AddStop(move(name_1), coord_1));
    BOOST_TEST(name_1.empty());

    BOOST_TEST(catalogue.GetCountAllStops() == (size_t)1);
    BOOST_TEST((catalogue.FindStop("Stop_1"s) == 
    Stop("Stop_1"s, Coordinates(0.1, 0.1))));

    catalogue.AddStop("Stop_2"s, {0.2, 0.2});
    catalogue.AddStop("Stop_3"s, {0.3, 0.3});

    BOOST_CHECK_EQUAL(catalogue.GetCountAllStops(), (size_t)3);
    BOOST_TEST((catalogue.FindStop("Stop_2"s) == 
    Stop("Stop_2"s, Coordinates(0.2, 0.2) )));
    BOOST_TEST((catalogue.FindStop("Stop_3"s) == 
    Stop("Stop_3"s, Coordinates(0.3, 0.3) )));

    BOOST_CHECK_THROW(catalogue.AddStop("Stop_3"s, {0.3, 0.3}), invalid_argument);
}

BOOST_AUTO_TEST_SUITE(IsContainsFullRoute_Testing);

BOOST_AUTO_TEST_CASE(IsContainsFullRoute_AllStopsPresent)
{
    TestableTransportCatalogue tc;

    tc.AddStop("Stop1", {55.7522, 37.6156});
    tc.AddStop("Stop2", {59.9343, 30.3351});
    std::vector<std::string> route = {"Stop1", "Stop2"};

    auto result = tc.IsContainsFullRoute(route);

    BOOST_CHECK(result.first);
    BOOST_CHECK(result.second.empty());
}

BOOST_AUTO_TEST_CASE(IsContainsFullRoute_MissingStops)
{
    TestableTransportCatalogue tc;

    tc.AddStop("Stop1", {55.7522, 37.6156});
    std::vector<std::string> route = {"Stop1", "Stop3"};

    auto result = tc.IsContainsFullRoute(route);

    BOOST_CHECK(!result.first);
    BOOST_REQUIRE_EQUAL(result.second.size(), (size_t)1);
    BOOST_CHECK(result.second[0] == "Stop3"s);
}

BOOST_AUTO_TEST_CASE(IsContainsFullRoute_EmptyRoute)
{
    TestableTransportCatalogue tc;

    std::vector<std::string> route;

    auto result = tc.IsContainsFullRoute(route);

    BOOST_CHECK(result.first);
    BOOST_CHECK(result.second.empty());
}

BOOST_AUTO_TEST_SUITE_END();

BOOST_AUTO_TEST_CASE(AddBus_Testing) {
    TransportCatalogue catalogue;

    catalogue.AddStop("Stop_1"s, {0.1, 0.1});
    catalogue.AddStop("Stop_2"s, {0.2, 0.2});
    catalogue.AddStop("Stop_3"s, {0.3, 0.3});

    BOOST_CHECK_NO_THROW(catalogue.AddRoute("Bus_1"s, 
    vector<string>({"Stop_1"s, "Stop_2"s, "Stop_1"s})));
    BOOST_CHECK_NO_THROW(catalogue.AddRoute("Bus_2"s, 
    vector<string>({"Stop_1"s, "Stop_2"s, "Stop_3"s})));

    BOOST_CHECK_EQUAL(catalogue.GetCountAllbuses(), (size_t)2);

    Bus bus_1 = catalogue.FindBus("Bus_1"s);
    Bus bus_2 = catalogue.FindBus("Bus_2"s);
    
    BOOST_CHECK(bus_1.name_ == "Bus_1");
    BOOST_CHECK(bus_2.name_ == "Bus_2");

    BOOST_CHECK(( VecPtrToVecNames(bus_1.route_) == 
    vector<string>({"Stop_1"s, "Stop_2"s, "Stop_1"s})));
    BOOST_CHECK(( VecPtrToVecNames(bus_2.route_) == 
    vector<string>({"Stop_1"s, "Stop_2"s, "Stop_3"s})));

    BOOST_CHECK_THROW(catalogue.AddRoute("Bus_3"s, 
    vector<string>({"Stop_1"s, "Stop_2"s, "Stop_5"s})), invalid_argument);
    BOOST_CHECK_THROW(catalogue.AddRoute("Bus_1"s, 
    vector<string>({"Stop_1"s, "Stop_2"s, "Stop_1"s})), invalid_argument);
}

BOOST_AUTO_TEST_SUITE(GetCountUniqueStops_Testing);

BOOST_AUTO_TEST_CASE(GetCountUniqueStops_UniqueStopsOnly)
{
    TransportCatalogue tc;

    tc.AddStop("Stop1", {55.7522, 37.6156});
    tc.AddStop("Stop2", {59.9343, 30.3351});
    tc.AddStop("Stop3", {40.7128, -74.0060});
    tc.AddRoute("Bus1", 
    vector<string>({"Stop1", "Stop2", "Stop3"}));

    size_t unique_stops = tc.GetCountUniqueStops("Bus1"s);

    BOOST_CHECK_EQUAL(unique_stops, (size_t)3);
}

BOOST_AUTO_TEST_CASE(GetCountUniqueStops_RepeatedStops)
{
    TransportCatalogue tc;

    tc.AddStop("Stop1", {55.7522, 37.6156});
    tc.AddStop("Stop2", {59.9343, 30.3351});
    tc.AddRoute("Bus1", 
    vector<string>({"Stop1", "Stop2", "Stop1", "Stop2"}));

    size_t unique_stops = tc.GetCountUniqueStops("Bus1"s);

    BOOST_CHECK_EQUAL(unique_stops, (size_t)2);
}

BOOST_AUTO_TEST_CASE(GetCountUniqueStops_SingleStop)
{
    TransportCatalogue tc;

    tc.AddStop("Stop1", {55.7522, 37.6156});
    tc.AddRoute("Bus1", vector<string>({"Stop1", "Stop1", "Stop1"}));

    size_t unique_stops = tc.GetCountUniqueStops("Bus1"s);

    BOOST_CHECK_EQUAL(unique_stops, (size_t)1);
}

BOOST_AUTO_TEST_CASE(GetCountUniqueStops_EmptyRoute)
{
    TransportCatalogue tc;

    tc.AddRoute("Bus1", vector<string>());

    size_t unique_stops = tc.GetCountUniqueStops("Bus1"s);

    BOOST_CHECK_EQUAL(unique_stops, (size_t)0);
}

BOOST_AUTO_TEST_SUITE_END();

BOOST_AUTO_TEST_SUITE(GetRouteLength_Testing);

BOOST_AUTO_TEST_CASE(GetRouteLength_SimpleRoute)
{
    TransportCatalogue tc;

    tc.AddStop("Stop1", {1.0, 1.0});
    tc.AddStop("Stop2", {1.0, 2.0});
    tc.AddStop("Stop3", {2.0, 2.0});
    tc.AddRoute("Bus1", vector<string>({"Stop1", "Stop2", "Stop3"}));

    double route_length = tc.GetRouteLength("Bus1"s);

    double computed_dist = ComputeDistance({1.0, 1.0}, {1.0, 2.0});
    computed_dist += ComputeDistance({1.0, 2.0}, {2.0, 2.0});

    BOOST_CHECK(IsEqualDouble(route_length, computed_dist));
}

BOOST_AUTO_TEST_CASE(GetRouteLength_RouteWithRepeatedStops)
{
    TransportCatalogue tc;

    tc.AddStop("Stop1", {1.0, 1.0});
    tc.AddStop("Stop2", {1.0, 2.0});
    tc.AddRoute("Bus1", vector<string>({"Stop1", "Stop2", "Stop1", "Stop2", "Stop1"}));

    double route_length = tc.GetRouteLength("Bus1"s);

    double computed_dist = ComputeDistance({1.0, 1.0}, {1.0, 2.0});
    computed_dist += ComputeDistance({1.0, 2.0}, {1.0, 1.0});
    computed_dist += ComputeDistance({1.0, 1.0}, {1.0, 2.0});
    computed_dist += ComputeDistance({1.0, 2.0}, {1.0, 1.0});

    BOOST_CHECK(IsEqualDouble(route_length, computed_dist));
}

BOOST_AUTO_TEST_CASE(GetRouteLength_ComplexRoute)
{
    TransportCatalogue tc;

    tc.AddStop("Stop1", {1.0, 1.0});
    tc.AddStop("Stop2", {1.0, 1.5});
    tc.AddStop("Stop3", {1.5, 1.5});
    tc.AddStop("Stop4", {1.5, 2.0});
    tc.AddRoute("Bus1", vector<string>({"Stop1", "Stop2", "Stop3", "Stop4",
                        "Stop3", "Stop2", "Stop1"}));

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
    TransportCatalogue tc;

    tc.AddStop("Stop1", {1.0, 1.0});
    tc.AddRoute("Bus1", vector<string>({"Stop1", "Stop1"}));

    double route_length = tc.GetRouteLength("Bus1"s);

    BOOST_CHECK(IsEqualDouble(route_length, 0.0));
}

BOOST_AUTO_TEST_CASE(GetRouteLength_EmptyRoute)
{
    TransportCatalogue tc;

    tc.AddRoute("Bus1", vector<string>());

    double route_length = tc.GetRouteLength("Bus1"s);

    BOOST_CHECK(IsEqualDouble(route_length, 0.0));
}

BOOST_AUTO_TEST_SUITE_END();

#endif