#include "main_tests.h"

#ifdef BENCHMARK

#include "../svg.h"
#include "../json.h"
#include "../request_handler.h"
#include "../json_reader.h"
#include "../map_renderer.h"
#include "C:/dev/libs/time/time.h"

#include <fstream>

using namespace std;

void BenchmarkStringAndOstreamRender() {
    svg::Document doc;
    vector<svg::Color> colors {svg::NoneColor, "red", svg::Rgb{0, 255, 0} , svg::Rgba{0, 0, 255, 0.5735678}};

    for (int i = 0; i < 1000; i++){
        svg::Polyline line;
        line.AddPoint({(double)i, (double)i*2}).SetStrokeLineCap(svg::StrokeLineCap::ROUND).SetStrokeLineJoin(svg::StrokeLineJoin::ROUND).
        SetFillColor(colors[i % 4]).SetStrokeColor(colors[i % 4]).SetStrokeWidth(5.5252);
        doc.AddObject(line);
    }

    stringstream stream;
    string str;
    Profiler profiler;
    
    {
        doc.Render(stream);
        profiler.PrintResults<chrono::microseconds>("The stream svg redner duration: ");
    }
    profiler.Restart();
    {
        doc.Render(str);
        profiler.PrintResults<chrono::microseconds>("The string svg redner duration: ");
    }
    
    stream.clear();
    str.clear();
    profiler.Restart();
    {
        doc.Render(stream);
        profiler.PrintResults<chrono::microseconds>("The string svg redner via ostream duration: ");
        stream >> str;
    }
    
};

void BenchmarkBuildingSvgJson() {
    ifstream input(TESTS_PATH / IN_FILE_MAP_3);
    Profiler profiler;

    json::Document parsed_doc = json::Load(input);
    profiler.PrintResults<chrono::microseconds>("Parsing and serialization doc to json: ");

    RequestHander handler;
    JsonReader reader;
    profiler.Restart();
    reader.ReadBaseJsonRequests(parsed_doc, handler);
    reader.ReadStatJsonRequests(parsed_doc, handler);
    profiler.PrintResults<chrono::microseconds>("ReadJsonRequests: ");

    TransportCatalogue transfport_catalogue;
    profiler.Restart();
    handler.ProvideInputRequests(transfport_catalogue);
    profiler.PrintResults<chrono::microseconds>("ProvideInputRequests: ");

    vector<shared_ptr<Stat>> stats;
    MapRenderer::RouteMap route_map;
    profiler.Restart();
    reader.ReadRenderSettingsJson(parsed_doc, route_map);
    profiler.PrintResults<chrono::microseconds>("ReadRenderSettingsJson: ");
    for (auto& bus_ptr : transfport_catalogue.GetAllBuses()) {
        route_map.AddRoute(bus_ptr);
    }
    route_map.ReorderRouteColors();
    
    stats = handler.GetStats(transfport_catalogue, route_map);

    profiler.Restart();
    json::Document out_doc = reader.BuildStatJsonOutput(stats);
    profiler.PrintResults<chrono::microseconds>("Build full json: ");
}

int main() {
    BenchmarkStringAndOstreamRender();
    BenchmarkBuildingSvgJson();
}

#endif