#include "tests/main_tests.h"

#ifndef DEBUG
#include <iostream>
#include <string>

#include "input_reader.h"
#include "stat_reader.h"

using namespace std;

int main() {
    TransportCatalogue catalogue;

    ReadInputAndApply(cin, catalogue);
    ParseFullRequest(catalogue, cin, cout);

    return 0;
}

#endif