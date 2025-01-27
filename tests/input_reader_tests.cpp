#include "main_tests.h"
#ifdef DEBUG

#include <boost\test\unit_test.hpp>
#include "../input_reader.h"
#include "../transport_catalogue.h"
#include "main_tests.h"

#include <fstream>
#include <iostream>
#include <algorithm>

using namespace std;

BOOST_AUTO_TEST_SUITE(InputRead_Testing);

BOOST_AUTO_TEST_CASE(Standart_Test) {
    ifstream stream("E:/Programming/Training_projects/"
    "Transport_Catalogue_tests_data/tsA_case0_input.txt");
    vector<string> lines = ReadCurrentLayerStream(stream);

    InputReader reader;
    for (auto& line : lines) {reader.ParseLine(line); }
    
    TransportCatalogue catalogue;
    BOOST_CHECK_NO_THROW(reader.ApplyCommands(catalogue));

    BOOST_CHECK_EQUAL(catalogue.GetCountAllbuses(), (size_t)2);
    BOOST_CHECK_EQUAL(catalogue.GetCountAllStops(), (size_t)8);

    BOOST_CHECK_EQUAL(catalogue.FindStop("Tolstopaltsevo"s), (Coordinates{55.611087, 37.208290}));
    BOOST_CHECK_EQUAL(catalogue.FindStop("Rasskazovka"s), (Coordinates{55.632761, 37.333324}));
    BOOST_CHECK_EQUAL(catalogue.FindStop("Biryulyovo Passazhirskaya"s), (Coordinates{55.580999, 37.659164}));

    BOOST_CHECK_EQUAL(catalogue.GetCountStops("750"s), size_t(5));
    BOOST_CHECK_EQUAL(catalogue.GetCountUniqueStops("750"s), size_t(3));
    BOOST_CHECK((catalogue.FindRoute("750"s) == 
    vector<string_view>{"Tolstopaltsevo"sv, "Marushkino"sv, "Rasskazovka"sv,
    "Marushkino"sv, "Tolstopaltsevo"sv}));

    BOOST_CHECK_EQUAL(catalogue.GetCountStops("256"s), size_t(6));
    BOOST_CHECK_EQUAL(catalogue.GetCountUniqueStops("256"s), size_t(5));
    BOOST_CHECK((catalogue.FindRoute("256"s) == 
    vector<string_view>{"Biryulyovo Zapadnoye"sv, "Biryusinka"sv, "Universam"sv,
    "Biryulyovo Tovarnaya"sv, "Biryulyovo Passazhirskaya"sv, "Biryulyovo Zapadnoye"sv}));
}

BOOST_AUTO_TEST_SUITE_END();

#endif