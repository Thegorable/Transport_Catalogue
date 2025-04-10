#include "tests/main_tests.h"

#ifndef DEBUG
#include <fstream>
#include <iostream>

#include "request_handler.h"
#include "json.h"
#include "json_reader.h"

using namespace std;

void ReadAndWriteRequest(istream& in, ostream& out) {
    TransportCatalogue catalogue;
    json::Document parsed_doc = json::Load(in);

    vector<shared_ptr<Request>> requests_base = ReadBaseJsonRequests(parsed_doc);
    vector<shared_ptr<Stat>> requests_stat = ReadStatJsonRequests(parsed_doc);
    
    ProvideInputRequests(requests_base, catalogue);
    auto stats = GetStats(requests_stat, catalogue);
    json::Document out_doc = BuildStatJsonOutput(stats);

    json::PrintNode(out_doc, out);
}

int main() {
    // ifstream in("H:\\Programming\\Training_projects\\Transport_Catalogue_tests_data\\json_testCase_2_input.json");
    // ofstream file("H:\\Programming\\Training_projects\\Transport_Catalogue\\json_testCase_2_OutResult.json");
    // ReadAndWriteRequest(in, file);
    // file.close();
    ReadAndWriteRequest(cin, cout);
    return 0;
}

#endif