#include "main_tests.h"
#ifdef DEBUG

#include <boost\test\unit_test.hpp>
#include "../stat_reader.h"
#include "main_tests.h"

#include <fstream>

using namespace std;

BOOST_AUTO_TEST_SUITE(StatReared_Testing);

BOOST_AUTO_TEST_CASE(PrintingParsedData_Standart) {
    ifstream stream("E:/Programming/Training_projects/"
    "Transport_Catalogue_tests_data/tsA_case0_input.txt");
    vector<string> lines_input = ReadCurrentLayerStream(stream);
    vector<string> lines_request = ReadCurrentLayerStream(stream);

    InputReader reader;
    for (auto& line : lines_input) {reader.ParseLine(line); }
    
    TransportCatalogue catalogue;
    reader.ApplyCommands(catalogue);

    {
        ostringstream os;
        BOOST_CHECK_NO_THROW(ParseAndPrintStat(catalogue, lines_request[0], os));
        cout << os.str();
        BOOST_CHECK_EQUAL(os.str(), "Bus 256: 6 stops on route, "s
        "5 unique stops, 4371.02 route length\n"s);
    }
    
    {
        ostringstream os;
        BOOST_CHECK_NO_THROW(ParseAndPrintStat(catalogue, lines_request[1], os));
        BOOST_CHECK_EQUAL(os.str(), "Bus 750: 5 stops on route, "s
        "3 unique stops, 20939.5 route length\n"s);}
    {
        ostringstream os;
        BOOST_CHECK_NO_THROW(ParseAndPrintStat(catalogue, lines_request[2], os));
        BOOST_CHECK_EQUAL(os.str(), "Bus 751: not found\n"s);
    }
}

BOOST_AUTO_TEST_SUITE_END();

#endif