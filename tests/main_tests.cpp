#include "main_tests.h"
#ifdef DEBUG

#include "simpletest.h"

#include <fstream>
#include <sstream>
#include <string>

#include "../json.h"
#include "../json_reader.h"
#include "../request_handler.h"

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

string ExecuteRequest(fs::path in, bool print_result = false) {
    ifstream input(in);
    json::Document parsed_doc = json::Load(input);

    vector<shared_ptr<Request>> requests_base = ReadBaseJsonRequests(parsed_doc);
    vector<shared_ptr<Stat>> requests_stat = ReadStatJsonRequests(parsed_doc);

    TransportCatalogue transfport_catalogue;
    ProvideInputRequests(requests_base, transfport_catalogue);
    auto stats = GetStats(requests_stat, transfport_catalogue);
    json::Document out_doc = BuildStatJsonOutput(stats);

    stringstream output;
    json::PrintNode(out_doc, output);

    if (print_result) {
        cout << output.str() << "\n\n";
    }

    return output.str();
}

string RePrintJson(fs::path in, bool print_result = false) {
    ifstream input(in);
    json::Document parsed_doc = json::Load(input);
    stringstream output;
    json::PrintNode(parsed_doc, output);

    if (print_result) {
        cout << output.str() << "\n\n";
    }

    return output.str();
}

DEFINE_TEST_G(Json_Main, MainTests) {
    {
        string str_rec = ExecuteRequest(TESTS_PATH / IN_FILE_JSON_1);
        string test_file = RePrintJson(TESTS_PATH / OUT_FILE_JSON_1);
        TEST(str_rec == test_file);
    }

    {
        string str_rec = ExecuteRequest(TESTS_PATH / IN_FILE_JSON_2);
        string test_file = RePrintJson(TESTS_PATH / OUT_FILE_JSON_2);
        TEST(str_rec == test_file);
    }
}

int main() {
    TestFixture::ExecuteAllTests();
    return 0;
}

#endif