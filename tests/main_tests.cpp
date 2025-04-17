#include "main_tests.h"
#ifdef DEBUG

#include "simpletest.h"

#include <fstream>
#include <sstream>
#include <string>

#include "../json.h"
#include "../json_reader.h"
#include "../request_handler.h"
#include "../map_renderer.h"

using namespace std;

void CompareStrings(const string& input, const string& comparator, ostream& your_out, ostream& compare_out) {
    // Helper to split string into lines
    auto SplitLines = [](const string& str) {
        vector<string> lines;
        istringstream iss(str);
        string line;
        while (getline(iss, line)) {
            lines.push_back(line);
        }
        return lines;
    };

    vector<string> input_lines = SplitLines(input);
    vector<string> comparator_lines = SplitLines(comparator);

    // your_out << "Your string:\n\n";
    size_t max_lines = max(input_lines.size(), comparator_lines.size());

    for (size_t i = 0; i < input_lines.size(); ++i) {
        your_out << input_lines[i];
        if (i < comparator_lines.size()) {
            if (input_lines[i] == comparator_lines[i]) {
                // your_out << " OK!\n";
                your_out << "\n";
            } else {
                // your_out << " Wrong!\n";
                your_out << "\n";
            }
        } else {
            // your_out << " Wrong!\n";
            your_out << "\n";
        }
    }

    // compare_out << "\n\nCompare string:\n\n";

    for (size_t i = 0; i < comparator_lines.size(); ++i) {
        compare_out << comparator_lines[i];
        if (i >= input_lines.size()) {
            // compare_out << " Not exists!\n";
            compare_out << "\n";
        } else if (comparator_lines[i] != input_lines[i]) {
            // compare_out << " Wrong!\n";
            compare_out << "\n";
        } else {
            // compare_out << '\n';
            compare_out << "\n";
        }
    }

    // compare_out << "\n\n";
}

string ReadFStream(ifstream& in) {
    string str;
    char c = in.get();
    while (in) {
        str += c;
        c = in.get();
    }
    return str;
}

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

string ExecuteRequestStat(fs::path in, bool print_result = false, ostream& out_source = cout) {
    ifstream input(in);
    json::Document parsed_doc = json::Load(input);

    RequestHander handler;
    JsonReader reader;
    reader.ReadBaseJsonRequests(parsed_doc, handler);
    reader.ReadStatJsonRequests(parsed_doc, handler);

    TransportCatalogue transfport_catalogue;
    handler.ProvideInputRequests(transfport_catalogue);
    auto stats = handler.GetStats(transfport_catalogue);
    json::Document out_doc = reader.BuildStatJsonOutput(stats);

    stringstream output;
    json::PrintNode(out_doc, output);

    if (print_result) {
        out_source << output.str() << "\n\n";
    }

    return output.str();
}

string ExecuteMapRender(fs::path in, bool print_result = false, ostream& out_source = cout) {
    ifstream input(in);
    json::Document parsed_doc = json::Load(input);

    RequestHander handler;
    JsonReader reader;
    reader.ReadBaseJsonRequests(parsed_doc, handler);

    MapRenderer::RouteMap route_map;
    reader.ReadRenderSettingsJson(parsed_doc, route_map);

    TransportCatalogue transfport_catalogue;
    handler.ProvideInputRequests(transfport_catalogue);

    for (auto& bus_ptr : transfport_catalogue.GetAllBuses()) {
        route_map.AddRoute(bus_ptr);
    }

    svg::Document doc_draw;
    stringstream output;
    route_map.Draw(doc_draw);
    doc_draw.Render(output);

    if (print_result) {
        out_source << "my svg:\n\n" << output.str() << "\n\n";
    }

    return output.str();
}

string RePrintJson(fs::path in, bool print_result = false, ostream& out_source = cout) {
    ifstream input(in);
    json::Document parsed_doc = json::Load(input);
    stringstream output;
    json::PrintNode(parsed_doc, output);

    if (print_result) {
        out_source << output.str() << "\n\n";
    }

    return output.str();
}

DEFINE_TEST_G(Json_Main, MainTests) {
    {
        string str_rec = ExecuteRequestStat(TESTS_PATH / IN_FILE_JSON_1);
        string test_file = RePrintJson(TESTS_PATH / OUT_FILE_JSON_1);
        TEST(str_rec == test_file);
    }

    {
        string str_rec = ExecuteRequestStat(TESTS_PATH / IN_FILE_JSON_2);
        string test_file = RePrintJson(TESTS_PATH / OUT_FILE_JSON_2);
        TEST(str_rec == test_file);
    }

    {
        string str_rec = ExecuteRequestStat(TESTS_PATH / IN_FILE_JSON_3);
        string test_file = RePrintJson(TESTS_PATH / OUT_FILE_JSON_3);
        TEST(str_rec == test_file);
    }

    {
        string str_rec = ExecuteRequestStat(TESTS_PATH / IN_FILE_JSON_4);
        string test_file = RePrintJson(TESTS_PATH / OUT_FILE_JSON_4);
        TEST(str_rec == test_file);
    }
    
    {
        string str_rec = ExecuteRequestStat(TESTS_PATH / IN_FILE_JSON_5);
        string test_file = RePrintJson(TESTS_PATH / OUT_FILE_JSON_5);
        TEST(str_rec == test_file);
    }
    {
        string str_rec = ExecuteRequestStat(TESTS_PATH / IN_FILE_JSON_6);
        string test_file = RePrintJson(TESTS_PATH / OUT_FILE_JSON_6);
        TEST(str_rec == test_file);
    }
}

DEFINE_TEST_G(TransportCatalogue_Render_Main, MainRenderTests) {
    {
        // string str_rec = ExecuteMapRender(TESTS_PATH / IN_FILE_RENDER_2);
        // ifstream stream_out(TESTS_PATH / OUT_FILE_RENDER_2);
        // string test_file = ReadFStream(stream_out);
        // cout << "Out xml:\n\n" << test_file << "\n\n";
        // CompareStrings(str_rec, test_file);
        // TEST(str_rec == test_file);
    }

    {
        string str_rec = ExecuteMapRender(TESTS_PATH / IN_FILE_RENDER_3);
        ifstream stream_out(TESTS_PATH / OUT_FILE_RENDER_3);
        string test_file = ReadFStream(stream_out);
        // cout << "Out xml:\n\n" << test_file << "\n\n";
        ofstream your(TESTS_PATH / "Your.xml"s);
        ofstream compare(TESTS_PATH / "Compare.xml"s);
        CompareStrings(str_rec, test_file, your, compare);
        your.close();
        compare.close();
        TEST(str_rec == test_file);
    }

    {
        // string str_rec = ExecuteMapRender(TESTS_PATH / IN_FILE_RENDER_4);
        // ifstream stream_out(TESTS_PATH / OUT_FILE_RENDER_4);
        // string test_file = ReadFStream(stream_out);
        // cout << "Out xml:\n\n" << test_file << "\n\n";
        // CompareStrings(str_rec, test_file);
        // TEST(str_rec == test_file);
    }
}

int main() {
    TestFixture::ExecuteTestGroup("MainRenderTests");
    // TestFixture::ExecuteAllTests();
    return 0;
}

#endif