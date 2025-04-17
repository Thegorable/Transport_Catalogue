#include "tests/main_tests.h"

#ifndef DEBUG
#include <fstream>
#include <iostream>

#include "request_handler.h"
#include "json.h"
#include "json_reader.h"

using namespace std;

void ReadAndWriteRequest(istream& in, ostream& out) {
    json::Document parsed_doc = json::Load(in);

    RequestHander handler;
    JsonReader reader;
    reader.ReadBaseJsonRequests(parsed_doc, handler);
    reader.ReadStatJsonRequests(parsed_doc, handler);

    TransportCatalogue transfport_catalogue;
    handler.ProvideInputRequests(transfport_catalogue);
    auto stats = handler.GetStats(transfport_catalogue);
    json::Document out_doc = reader.BuildStatJsonOutput(stats);

    json::PrintNode(out_doc, out);
}

void ReadAndRenderMap(istream& in, ostream& out) {
    json::Document parsed_doc = json::Load(in);

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
    route_map.Draw(doc_draw);
    doc_draw.Render(out);
}

int main() {
    // ifstream in("H:\\Programming\\Training_projects\\Transport_Catalogue_tests_data\\render_testCase_1_input.json");
    // ofstream file("H:\\Programming\\Training_projects\\Transport_Catalogue\\render_testCase_1_output_user.xml");
    // ReadAndRenderMap(in, file);
    // file.close();
    ReadAndRenderMap(cin, cout);
    return 0;
}

#endif