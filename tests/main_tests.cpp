#include "main_tests.h"
#ifdef DEBUG

#include "simpletest.h"

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

int main() {
    TestFixture::ExecuteAllTests();
    return 0;
}

#endif