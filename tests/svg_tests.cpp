#include "main_tests.h"
#ifdef DEBUG

#include "../svg.h"
#include "../json.h"

#include <sstream>
#include <fstream>

using namespace std;

namespace svg {

DEFINE_TEST_G( WriteToOstream, SVG_Document_Testring) {
    Circle circle_1;
    Circle circle_2;
    Circle circle_3;
    circle_1.SetRadius(50).SetCenter({100.54, 100.5}).SetStrokeWidth(10).SetFillColor("yellow");
    circle_2.SetRadius(75).SetCenter({100.52, 150.52}).SetStrokeWidth(15).SetFillColor("black");
    circle_3.SetRadius(100).SetCenter({100.554, 250.5}).SetStrokeWidth(20).SetFillColor("red");
    Text text;
    text.SetData("Testing"s).SetFillColor(Rgb{0, 0, 255}).SetPosition({30, 20}).SetStrokeWidth(5).
    SetFontSize(20).SetFontWeight("bold"s).SetFontFamily("Verdana"s);

    Document doc;
    doc.AddObject(circle_1);
    doc.AddObject(circle_2);
    doc.AddObject(circle_3);
    doc.AddObject(text);

    {
        ifstream stream(TESTS_PATH / SVG_FILE_RESULT_SVG_1);
        string result_test = ReadFStream(stream);
        ostringstream streem_result;
        doc.Render(streem_result);
        string text_result = streem_result.str();

        TEST(result_test == text_result);
    }
}

}

#endif