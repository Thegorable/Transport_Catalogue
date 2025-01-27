#include "main_tests.h"
#ifdef DEBUG

#define BOOST_TEST_MODULE Transport_Catalogue_Testing
#include <boost\test\unit_test.hpp>

#include <fstream>
#include <iostream>

using namespace std;

vector<string> ReadCurrentLayerStream(ifstream& stream) {
    int base_request_count;
    stream >> base_request_count >> ws;
    vector<string> lines;

        for (int i = 0; i < base_request_count; ++i) {
            string line;
            getline(stream, line);
            lines.push_back(line);
        }

    return lines;
}

#endif