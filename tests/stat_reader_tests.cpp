#include "main_tests.h"
#ifdef DEBUG

#include "../stat_reader.h"
#include "main_tests.h"

#include <fstream>
#include <cassert>

using namespace std;

DEFINE_TEST_G(Standart_1, StatRead_Testing) {

    TransportCatalogue catalogue;

    ifstream in(TESTS_PATH / IN_FILE_1);
    ifstream out(TESTS_PATH / OUT_FILE_1);
    stringstream write;

    ReadInputAndApply(in, catalogue);
    ParseFullRequest(catalogue, in, write);

    std::ostringstream result;
    result << out.rdbuf();
    TEST_EQ(result.str(), write.str());
}

DEFINE_TEST_G(Standart_2, StatRead_Testing) {

    TransportCatalogue catalogue;

    ifstream in(TESTS_PATH / IN_FILE_2);
    ifstream out(TESTS_PATH / OUT_FILE_2);
    stringstream write;

    ReadInputAndApply(in, catalogue);
    ParseFullRequest(catalogue, in, write);

    std::ostringstream result;
    result << out.rdbuf();
    TEST_EQ(result.str(), write.str());
}

DEFINE_TEST_G(Standart_3, StatRead_Testing) {

    TransportCatalogue catalogue;

    ifstream in(TESTS_PATH / IN_FILE_3);
    ifstream out(TESTS_PATH / OUT_FILE_3);
    stringstream write;

    ReadInputAndApply(in, catalogue);
    ParseFullRequest(catalogue, in, write);

    std::ostringstream result;
    result << out.rdbuf();
    TEST_EQ(result.str(), write.str());
}

// DEFINE_TEST_G(Standart_4, StatRead_Testing) {

//     ifstream stream(TESTS_PATH / IN_FILE_4);
//     vector<string> lines_input = ReadCurrentLayerStream(stream);
//     vector<string> lines_request = ReadCurrentLayerStream(stream);

//     InputReader reader;
//     for (auto& line : lines_input) {reader.ParseLine(line); }
    
//     TransportCatalogue catalogue;
//     reader.ApplyCommands(catalogue);

//     ifstream stream_out(TESTS_PATH / OUT_FILE_4);
    
//     for (size_t i = 0; i < lines_request.size() && i < 10; i++) {
//         ostringstream os;
//         ParseAndPrintStat(catalogue, lines_request[i], os);
        
//         string line_in = os.str();
//         string line_out;
//         getline(stream_out, line_out);
//         line_out += '\n';
//         cout << line_in;
//         cout << line_out << "\n\n";
//         TEST_EQ(line_in, line_out);
//     }
// }

#endif