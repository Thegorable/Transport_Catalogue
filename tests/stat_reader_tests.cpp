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

    ifstream stream_out("E:/Programming/Training_projects/"
    "Transport_Catalogue_tests_data/tsA_case0_output.txt");
    
    for (size_t i = 0; i < lines_request.size(); i++) {
        ostringstream os;
        BOOST_CHECK_NO_THROW(ParseAndPrintStat(catalogue, lines_request[i], os));
        
        string line_in = os.str();
        string line_out;
        getline(stream_out, line_out);
        line_out += '\n';
        BOOST_CHECK_EQUAL(line_in, line_out);
    }
}

BOOST_AUTO_TEST_SUITE_END();

#endif