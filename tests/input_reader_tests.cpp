#include "main_tests.h"
#ifdef DEBUG

#include "../input_reader.h"
#include "../transport_catalogue.h"
#include "main_tests.h"

#include "simpletest.h"

#include <fstream>
#include <iostream>
#include <algorithm>

using namespace std;
using namespace Geo;

DEFINE_TEST_G(Basic, SplitToPair)
{
    std::string input = "hello::world";
    auto result = Parser::SplitToPair(input, "::"sv);
    TEST_EQ(result.first, "hello"sv);
    TEST_EQ(result.second, "world"sv);
}

DEFINE_TEST_G(MultipleDelimiters, SplitToPair)
{
    std::string input = "a::b::c::d";
    auto result = Parser::SplitToPair(input, "::"sv, 2);
    TEST_EQ(result.first, "a::b"sv);
    TEST_EQ(result.second, "c::d"sv);
}

DEFINE_TEST_G(NoDelimiter, SplitToPair)
{
    std::string input = "singleword";
    auto result = Parser::SplitToPair(input, "::"sv);
    TEST_EQ(result.first, "singleword"sv);
    TEST_EQ(result.second, ""sv);
}

DEFINE_TEST_G(DelimiterAtEnd, SplitToPair)
{
    std::string input = "trailing::";
    auto result = Parser::SplitToPair(input, "::"sv);
    TEST_EQ(result.first, "trailing"sv);
    TEST_EQ(result.second, ""sv);
}

DEFINE_TEST_G(DelimiterAtStart, SplitToPair)
{
    std::string input = "::leading";
    auto result = Parser::SplitToPair(input, "::"sv);
    TEST_EQ(result.first, ""sv);
    TEST_EQ(result.second, "leading"sv);
}

DEFINE_TEST_G(EmptyString, SplitToPair)
{
    std::string input = "";
    auto result = Parser::SplitToPair(input, "::"sv);
    TEST_EQ(result.first, ""sv);
    TEST_EQ(result.second, ""sv);
}

DEFINE_TEST_G(MultipleSameDelimiters, SplitToPair)
{
    std::string input = "x::y::z";
    auto result = Parser::SplitToPair(input, ":"sv, 2);
    TEST_EQ(result.first, "x:"sv);
    TEST_EQ(result.second, "y::z"sv);
}

DEFINE_TEST_G(ExceedingDelimiterIndex, SplitToPair)
{
    std::string input = "x::y::z";
    auto result = Parser::SplitToPair(input, "::"sv, 5);
    TEST_EQ(result.first, "x::y::z"sv);
    TEST_EQ(result.second, ""sv);
}

DEFINE_TEST_G(ConsecutiveDelimiters, SplitToPair)
{
    std::string input = "a::b::c";
    auto result = Parser::SplitToPair(input, ":"sv, 2);
    TEST_EQ(result.first, "a:"sv);
    TEST_EQ(result.second, "b::c"sv);
}


DEFINE_TEST_G(Standart_Test, InputRead_Testing) {
    ifstream stream(TESTS_PATH / IN_FILE_1);
    vector<string> lines = ReadCurrentLayerStream(stream);

    InputReader reader;
    for (auto& line : lines) {reader.ParseLine(line); }
    
    TransportCatalogue catalogue;
    reader.ApplyCommands(catalogue);

    TEST(*catalogue.FindStop("Tolstopaltsevo"s) == 
    Stop("Tolstopaltsevo"s, Coordinates{55.611087, 37.208290}));
    TEST(*catalogue.FindStop("Rasskazovka"s) == 
    Stop("Rasskazovka"s, Coordinates{55.632761, 37.333324}));
    TEST(*catalogue.FindStop("Biryulyovo Passazhirskaya"s) == 
    Stop("Biryulyovo Passazhirskaya"s, Coordinates{55.580999, 37.659164}));

    RouteStatistics stat = catalogue.GetRouteStatistics("750"s);

    TEST_EQ(stat.stops_count_, size_t(7));
    TEST_EQ(stat.unique_stops_count_, size_t(3));
    
    TEST((VecPtrToVecNames((catalogue.FindBus("750"s)->route_)) == 
    vector<string>{"Tolstopaltsevo"s, "Marushkino"s, "Marushkino"s, "Rasskazovka"s,
    "Marushkino"s, "Marushkino"s, "Tolstopaltsevo"s}));

    stat = catalogue.GetRouteStatistics("256"s);

    TEST_EQ(stat.stops_count_, size_t(6));
    TEST_EQ(stat.unique_stops_count_, size_t(5));
    TEST((VecPtrToVecNames(catalogue.FindBus("256"s)->route_) == 
    vector<string>{"Biryulyovo Zapadnoye"s, "Biryusinka"s, "Universam"s,
    "Biryulyovo Tovarnaya"s, "Biryulyovo Passazhirskaya"s, "Biryulyovo Zapadnoye"s}));
}

#endif