#include "tests/main_tests.h"

#ifndef DEBUG
#include <fstream>
#include <iostream>

#include "request_handler.h"
#include "json.h"
#include "json_reader.h"

using namespace std;

void ReadAndWriteRequest(istream& input, ostream& out) {
    json::Document parsed_doc = json::Load(input);

    RequestHander handler;
    JsonReader reader;
    reader.ReadBaseJsonRequests(parsed_doc, handler);
    reader.ReadStatJsonRequests(parsed_doc, handler);

    TransportCatalogue transfport_catalogue;
    handler.ProvideInputRequests(transfport_catalogue);
    
    MapRenderer::RouteMap route_map;
    reader.ReadRenderSettingsJson(parsed_doc, route_map);
    for (auto& bus_ptr : transfport_catalogue.GetAllBuses()) {
        route_map.AddRoute(bus_ptr);
    }
    route_map.ReorderRouteColors();
    
    auto stats = handler.GetStats(transfport_catalogue, route_map);
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
    route_map.ReorderRouteColors();

    svg::Document doc_draw;
    route_map.Draw(doc_draw);
    doc_draw.Render(out);
}

int main() {
    // ifstream in("H:\\Programming\\Training_projects\\Transport_Catalogue_tests_data\\render_testCase_1_input.json");
    // ofstream file("H:\\Programming\\Training_projects\\Transport_Catalogue\\render_testCase_1_output_user.xml");
    // ReadAndRenderMap(in, file);
    // file.close();
    ReadAndWriteRequest(cin, cout);
    return 0;
}

#endif