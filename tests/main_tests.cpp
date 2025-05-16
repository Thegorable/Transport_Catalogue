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

    size_t max_lines = max(input_lines.size(), comparator_lines.size());

    for (size_t i = 0; i < input_lines.size(); ++i) {
        your_out << input_lines[i];
        if (i < comparator_lines.size()) {
            if (input_lines[i] == comparator_lines[i]) {
                your_out << " OK!\n";
            } else {
                your_out << " Wrong!\n";
            }
        } else {
            your_out << " Wrong!\n";
        }
    }

    compare_out << "\n\nCompare string:\n\n";

    for (size_t i = 0; i < comparator_lines.size(); ++i) {
        compare_out << comparator_lines[i];
        if (i >= input_lines.size()) {
            compare_out << " Not exists!\n";
        } else if (comparator_lines[i] != input_lines[i]) {
            compare_out << " Wrong!\n";
        } else {
            compare_out << '\n';
        }
    }

    compare_out << "\n\n";
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

string ExecuteRequestStat(fs::path in, bool render = true, ostream& out_source = cout) {
    ifstream input(in);
    json::Document parsed_doc = json::Load(input);

    RequestHander handler;
    JsonReader reader;
    reader.ReadBaseJsonRequests(parsed_doc, handler);
    reader.ReadStatJsonRequests(parsed_doc, handler);

    TransportCatalogue transfport_catalogue;
    handler.ProvideInputRequests(transfport_catalogue);

    vector<shared_ptr<Stat>> stats;
    if (render) {
        MapRenderer::RouteMap route_map;
        reader.ReadRenderSettingsJson(parsed_doc, route_map);
        for (auto& bus_ptr : transfport_catalogue.GetAllBuses()) {
            route_map.AddRoute(bus_ptr);
        }
        route_map.ReorderRouteColors();
        
        stats = handler.GetStats(transfport_catalogue, route_map);
    }
    else {
        stats = handler.GetStats(transfport_catalogue);
    }

    json::Document out_doc = reader.BuildStatJsonOutput(stats);

    stringstream output;
    json::PrintNode(out_doc, output);

    return output.str();
}

json::Document BuildDocRequestStat(fs::path in, bool render = true) {
    ifstream input(in);
    json::Document parsed_doc = json::Load(input);

    RequestHander handler;
    JsonReader reader;
    reader.ReadBaseJsonRequests(parsed_doc, handler);
    reader.ReadStatJsonRequests(parsed_doc, handler);

    TransportCatalogue transfport_catalogue;
    handler.ProvideInputRequests(transfport_catalogue);
    
    vector<shared_ptr<Stat>> stats;
    if (render) {
        MapRenderer::RouteMap route_map;
        reader.ReadRenderSettingsJson(parsed_doc, route_map);
        for (auto& bus_ptr : transfport_catalogue.GetAllBuses()) {
            route_map.AddRoute(bus_ptr);
        }
        route_map.ReorderRouteColors();
        
        stats = handler.GetStats(transfport_catalogue, route_map);
    }
    else {
        stats = handler.GetStats(transfport_catalogue);
    }

    return reader.BuildStatJsonOutput(stats);
}

string BuildMapSvg(fs::path in, bool print_result = false, ostream& out_source = cout) {
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
    route_map.ReorderRouteColors();

    svg::Document doc_draw;
    route_map.Draw(doc_draw);

    stringstream output;
    doc_draw.Render(output);

    if (print_result) {
        out_source << "my svg:\n\n" << output.str() << "\n\n";
    }

    return output.str();
};

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
        json::Document doc_result = BuildDocRequestStat(TESTS_PATH / IN_FILE_JSON_1, false);
        ifstream input(TESTS_PATH / OUT_FILE_JSON_1);
        json::Document doc_test = json::Load(input);
        TEST(doc_result == doc_test);
    }

    {
        json::Document doc_result = BuildDocRequestStat(TESTS_PATH / IN_FILE_JSON_2, false);
        ifstream input(TESTS_PATH / OUT_FILE_JSON_2);
        json::Document doc_test = json::Load(input);
        TEST(doc_result == doc_test);
    }

    {
        json::Document doc_result = BuildDocRequestStat(TESTS_PATH / IN_FILE_JSON_3, false);
        ifstream input(TESTS_PATH / OUT_FILE_JSON_3);
        json::Document doc_test = json::Load(input);
        TEST(doc_result == doc_test);
    }

    {
        json::Document doc_result = BuildDocRequestStat(TESTS_PATH / IN_FILE_JSON_4, false);
        ifstream input(TESTS_PATH / OUT_FILE_JSON_4);
        json::Document doc_test = json::Load(input);
        TEST(doc_result == doc_test);
    }
    
    {
        json::Document doc_result = BuildDocRequestStat(TESTS_PATH / IN_FILE_JSON_5, false);
        ifstream input(TESTS_PATH / OUT_FILE_JSON_5);
        json::Document doc_test = json::Load(input);
        TEST(doc_result == doc_test);
    }
    {
        json::Document doc_result = BuildDocRequestStat(TESTS_PATH / IN_FILE_JSON_6, false);
        ifstream input(TESTS_PATH / OUT_FILE_JSON_6);
        json::Document doc_test = json::Load(input);
        TEST(doc_result == doc_test);
    }
}

DEFINE_TEST_G(TransportCatalogue_Render_Main, MainRenderTests) {    
    {
        string str_rec = BuildMapSvg(TESTS_PATH / IN_FILE_RENDER_2);
        ifstream stream_out(TESTS_PATH / OUT_FILE_RENDER_2);
        string test_file = ReadFStream(stream_out);
        TEST(str_rec == test_file);
    }

    {
        string str_rec = BuildMapSvg(TESTS_PATH / IN_FILE_RENDER_7);
        ifstream stream_out(TESTS_PATH / OUT_FILE_RENDER_7);
        string test_file = ReadFStream(stream_out);
        TEST(str_rec == test_file);
    }

    {
        string str_rec = BuildMapSvg(TESTS_PATH / IN_FILE_RENDER_12);
        ifstream stream_out(TESTS_PATH / OUT_FILE_RENDER_12);
        string test_file = ReadFStream(stream_out);
        TEST(str_rec == test_file);
    }

    {
        string str_rec = BuildMapSvg(TESTS_PATH / IN_FILE_RENDER_23);
        ifstream stream_out(TESTS_PATH / OUT_FILE_RENDER_23);
        string test_file = ReadFStream(stream_out);
        TEST(str_rec == test_file);
    }
}

DEFINE_TEST_G(TransportCatalogue_Map_Main, Json_Map_Tests) {    
    // {
    //     json::Document result_doc = BuildDocRequestStat(TESTS_PATH / IN_FILE_MAP_DEF);
    //     ifstream test_stream_in(TESTS_PATH / OUT_FILE_MAP_DEF);
    //     json::Document test_doc = json::Load(test_stream_in);

    //     TEST(result_doc == test_doc);
    // }

    // {
    //     json::Document result_doc = BuildDocRequestStat(TESTS_PATH / IN_FILE_MAP_1);
    //     ifstream test_stream_in(TESTS_PATH / OUT_FILE_MAP_1);
    //     json::Document test_doc = json::Load(test_stream_in);

    //     TEST(result_doc == test_doc);
    // }

    // {
    //     json::Document result_doc = BuildDocRequestStat(TESTS_PATH / IN_FILE_MAP_2);
    //     ifstream test_stream_in(TESTS_PATH / OUT_FILE_MAP_2);
    //     json::Document test_doc = json::Load(test_stream_in);

    //     TEST(result_doc == test_doc);
    // }

    // {
    //     json::Document result_doc = BuildDocRequestStat(TESTS_PATH / IN_FILE_MAP_3);
    //     ifstream test_stream_in(TESTS_PATH / OUT_FILE_MAP_3);
    //     json::Document test_doc = json::Load(test_stream_in);

    //     TEST(result_doc == test_doc);
    // }

    {
        string result_doc = ExecuteRequestStat(TESTS_PATH / "DoubleRGBA_test.txt", true);
        ofstream file_result(TESTS_PATH / "result.txt");
        file_result << result_doc;
        file_result.close();

        // ifstream input(TESTS_PATH / "DoubleRGBA_test.txt");
        // ofstream file_result(TESTS_PATH / "result.txt");
        // RePrintJson(TESTS_PATH / "DoubleRGBA_test.txt", true, file_result);
    }

}

int main() {
    // TestFixture::ExecuteTestGroup("MainTests");
    // TestFixture::ExecuteTestGroup("MainRenderTests");
    TestFixture::ExecuteTestGroup("Json_Map_Tests");
    // TestFixture::ExecuteAllTests();
    return 0;
}

#endif