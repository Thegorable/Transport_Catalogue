#include "main_tests.h"
#ifdef DEBUG

#include <chrono>
#include <sstream>
#include <string_view>

#include "../json.h"

using namespace json;
using namespace std;

namespace {

json::Document LoadJSON(const std::string& s) {
    std::istringstream strm(s);
    return json::Load(strm);
}

std::string PrintNodeToString(const Node& node) {
    std::ostringstream out;
    PrintNode(Document{node}, out);
    return out.str();
}

void MustFailToLoad(const std::string& s) {
    try {
        Node node = LoadJSON(s).GetRoot();
        TEST_ERROR_("ParsingError exception is expected on '", s, "'"s);
    } catch (const json::ParsingError&) {
        TEST_MESSAGE(true, "ParsingError is executed succsessfuly");
    } catch (const std::exception& e) {
        TEST_ERROR_("exception thrown: '", e.what(), "'"s);
    } catch (...) {
        TEST_ERROR_("Unexpected error");
    }
}

template <typename Fn>
void MustThrowLogicError(Fn fn) {
    try {
        fn();
        TEST_ERROR_("logic_error is expected");
    } catch (const std::logic_error&) {
        // ok
    } catch (const std::exception& e) {
        TEST_ERROR_("exception thrown: ");
    } catch (...) {
        TEST_ERROR_("Unexpected error");
    }
}

// void Benchmark() {
//     const auto start = std::chrono::steady_clock::now();
//     Array arr;
//     arr.reserve(1000);
//     for (int i = 0; i < 1000; ++i) {
//         arr.emplace_back(Dict{
//             {"int"s, 42},
//             {"double"s, 42.1},
//             {"null"s, nullptr},
//             {"string"s, "hello"s},
//             {"array"s, Array{1, 2, 3}},
//             {"bool"s, true},
//             {"map"s, Dict{{"key"s, "value"s}}},
//         });
//     }
//     std::stringstream strm;
//     json::PrintNode(Document{arr}, strm);
//     const auto doc = json::Load(strm);
//     TEST(doc.GetRoot() == arr);
//     const auto duration = std::chrono::steady_clock::now() - start;
//     std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms"sv
//               << std::endl;
// }

DEFINE_TEST_G(NullJson, Json_Testing) {
    Node null_node;
    TEST(null_node.IsNull());
    TEST(!null_node.IsInt());
    TEST(!null_node.IsDouble());
    TEST(!null_node.IsPureDouble());
    TEST(!null_node.IsString());
    TEST(!null_node.IsArray());
    TEST(!null_node.IsMap());

    Node null_node1{nullptr};
    TEST(null_node1.IsNull());

    TEST(PrintNodeToString(null_node) == "null"s);
    TEST(null_node == null_node1);

    const Node node = LoadJSON("null"s).GetRoot();
    TEST(node.IsNull());
    TEST(node == null_node);

    Node null_node_2 = LoadJSON(" \t\r\n\n\r null \t\r\n\n\r "s).GetRoot();
    TEST(null_node_2 == null_node);
}

DEFINE_TEST_G(NumbersJson, Json_Testing) {
    const Node int_node{42};
    TEST(int_node.IsInt());
    TEST(int_node.AsInt() == 42);

    TEST(int_node.IsDouble());

    TEST(int_node.AsDouble() == 42.0);
    TEST(!int_node.IsPureDouble());
    TEST(int_node == Node{42});

    TEST_NEQ(int_node, Node{42.0});

    const Node dbl_node{123.45};
    TEST(dbl_node.IsDouble());
    TEST(dbl_node.AsDouble() == 123.45);
    TEST(dbl_node.IsPureDouble());
    TEST(!dbl_node.IsInt());

    TEST(PrintNodeToString(int_node) == "42"s);
    TEST(PrintNodeToString(dbl_node) == "123.45"s);
    TEST(PrintNodeToString(Node{-42}) == "-42"s);
    TEST(PrintNodeToString(Node{-3.5}) == "-3.5"s);
    
    // Node node_zero = LoadJSON("0"s).GetRoot();
    // Node node_42 = LoadJSON("42.baba"s).GetRoot();
    // Node node_1_2 = LoadJSON("1.2elka"s).GetRoot();
    // Node node_1_2_plus = LoadJSON("1.2e+lka"s).GetRoot();
    // Node node_1_2_minus = LoadJSON("1.2e-lka"s).GetRoot();
    // Node node_int_overflow = LoadJSON("2147483648"s).GetRoot();

    // TEST(node_zero.IsInt());
    // TEST(node_zero.AsInt() == 0);
    // TEST(node_42.IsInt());
    // TEST(node_42.AsInt() == 42);
    // TEST(node_1_2.IsDouble());
    // TEST(node_1_2.AsDouble() == 1.2);
    // TEST(node_1_2_plus.IsDouble());
    // TEST(node_1_2_plus.AsDouble() == 1.2);
    // TEST(node_1_2_minus.IsDouble());
    // TEST(node_1_2_minus.AsDouble() == 1.2);
    // TEST(node_int_overflow.IsDouble());
    // TEST(node_int_overflow.AsDouble() == 2147483648.0);

    TEST(LoadJSON("42"s).GetRoot() == int_node);
    TEST(LoadJSON("123.45"s).GetRoot() == dbl_node);
    TEST(LoadJSON("0.25"s).GetRoot().AsDouble() == 0.25);
    TEST(LoadJSON("3e5"s).GetRoot().AsDouble() == 3e5);
    TEST(LoadJSON("-3e5"s).GetRoot().AsDouble() == -3e5);
    TEST(LoadJSON("1.2e-5"s).GetRoot().AsDouble() == 1.2e-5);
    TEST(LoadJSON("1563.26e-11"s).GetRoot().AsDouble() == 1563.26e-11);
    TEST(LoadJSON("-0.0053"s).GetRoot().AsDouble() == -0.0053);
    TEST(LoadJSON("1.2E-5"s).GetRoot().AsDouble() == 1.2e-5);
    TEST(LoadJSON("1.2e+5"s).GetRoot().AsDouble() == 1.2e5);
    TEST(LoadJSON("1.2E+5"s).GetRoot().AsDouble() == 1.2e5);
    TEST(LoadJSON("-123456"s).GetRoot().AsInt() == -123456);
    TEST(LoadJSON("0").GetRoot() == Node{0});
    TEST(LoadJSON("0.0").GetRoot() == Node{0.0});

    TEST(LoadJSON(" \t\r\n\n\r 0.0 \t\r\n\n\r ").GetRoot() == Node{0.0});
}

DEFINE_TEST_G(StringJson, Json_Testing) {
    {
        Node str_node{"Hello, \"everybody\""s};
        TEST(str_node.IsString());
        TEST(str_node.AsString() == "Hello, \"everybody\""s);

        TEST(!str_node.IsInt());
        TEST(!str_node.IsDouble());

        TEST(PrintNodeToString(str_node) == "\"Hello, \\\"everybody\\\"\""s);
        TEST(LoadJSON(PrintNodeToString(str_node)).GetRoot() == str_node);
        const std::string escape_chars = R"("\r\n\t\"\\")"s;
        TEST(escape_chars == escape_chars);
        TEST(PrintNodeToString(LoadJSON(escape_chars).GetRoot()) == "\"\\r\\n\\t\\\"\\\\\""s);
        TEST(LoadJSON("\t\r\n\n\r \"Hello\" \t\r\n\n\r ").GetRoot() == Node{"Hello"s});
    }

    {
        TEST(LoadJSON("\"\\\"nested\\\" string\"").GetRoot().AsString() == "\"nested\" string");
        std::string raw = R"("\\\"double nested\\\"")";
        std::string raw_no_escaped = "\\\"double nested\\\"";
        std::string escaped = "\"\\\\\\\"double nested\\\\\\\"\""s;
        TEST(raw == escaped);
        TEST(LoadJSON(raw).GetRoot().AsString() == raw_no_escaped);
        std::string test = PrintNodeToString(LoadJSON(raw).GetRoot());
        TEST(PrintNodeToString(LoadJSON(raw).GetRoot()) == escaped);
        TEST(LoadJSON("\"\\\"\\t tricky \\\"\"").GetRoot().AsString() == "\"\t tricky \"");
    }

    {
        Node node = LoadJSON("\"\\t\\r\\n\\\"Hello\\\"\\t\\r\\n\"").GetRoot();
        TEST(node.AsString() == "\t\r\n\"Hello\"\t\r\n");
    }

    {
        Node node("\t\r\n\"\\\\");
        TEST(PrintNodeToString(node) == "\"\\t\\r\\n\\\"\\\\\\\\\"");
    }
}

DEFINE_TEST_G(BoolJson, Json_Testing) {
    Node true_node{true};
    TEST(true_node.IsBool());
    TEST(true_node.AsBool());

    Node false_node{false};
    TEST(false_node.IsBool());
    TEST(!false_node.AsBool());

    TEST(PrintNodeToString(true_node) == "true"s);
    TEST(PrintNodeToString(false_node) == "false"s);

    TEST(LoadJSON("true"s).GetRoot() == true_node);
    TEST(LoadJSON("false"s).GetRoot() == false_node);
    TEST(LoadJSON(" \t\r\n\n\r true \r\n"s).GetRoot() == true_node);
    TEST(LoadJSON(" \t\r\n\n\r false \t\r\n\n\r "s).GetRoot() == false_node);
}

DEFINE_TEST_G(ArrayJson, Json_Testing) {
    {
        Node arr_node{Array{1, 1.23, "Hello"s}};
        TEST(arr_node.IsArray());
        const Array& arr = arr_node.AsArray();
        TEST(arr.size() == 3);
        TEST(arr.at(0).AsInt() == 1);

        Node test = LoadJSON("[1,1.23,\"Hello\"]"s).GetRoot();
        TEST(LoadJSON("[1,1.23,\"Hello\"]"s).GetRoot() == arr_node);
        TEST(LoadJSON(PrintNodeToString(arr_node)).GetRoot() == arr_node);
        TEST(LoadJSON(R"(  [ 1  ,  1.23,  "Hello"   ]   )"s).GetRoot() == arr_node);
        TEST(LoadJSON("[ 1 \r \n ,  \r\n\t 1.23, \n \n  \t\t  \"Hello\" \t \n  ] \n  "s).GetRoot() == arr_node);
    }

    {
        Node array = LoadJSON("[1, 2, 3]").GetRoot();
        TEST(array.IsArray());
        TEST(array.AsArray().size() == 3);
        TEST(array.AsArray()[0].AsInt() == 1);
        TEST(array.AsArray()[1].AsInt() == 2);
        TEST(array.AsArray()[2].AsInt() == 3);
    }

    {
        Node array = Node(Array{1, 2, 3});
        TEST(PrintNodeToString(array) == "[\n    1,\n    2,\n    3\n]");
    }
}

DEFINE_TEST_G(MapJson, Json_Testing) {
    Node dict_node{Dict{{"key1"s, "value1"s}, {"key2"s, 42}}};
    TEST(dict_node.IsMap());
    const Dict& dict = dict_node.AsMap();
    TEST(dict.size() == 2);
    TEST(dict.at("key1"s).AsString() == "value1"s);
    TEST(dict.at("key2"s).AsInt() == 42);

    TEST(LoadJSON("{ \"key1\": \"value1\", \"key2\": 42 }"s).GetRoot() == dict_node);
    std::string str = PrintNodeToString(dict_node);
    Node test = LoadJSON(str).GetRoot();
    TEST(LoadJSON(PrintNodeToString(dict_node)).GetRoot() == dict_node);
    TEST(
        LoadJSON(
            "\t\r\n\n\r { \t\r\n\n\r \"key1\" \t\r\n\n\r: \t\r\n\n\r \"value1\" \t\r\n\n\r , \t\r\n\n\r \"key2\" \t\r\n\n\r : \t\r\n\n\r 42 \t\r\n\n\r } \t\r\n\n\r"s)
            .GetRoot()
        == dict_node);
}

DEFINE_TEST_G(ParsingComplexNested, Json_Testing) {
    Node complex = LoadJSON("{\"data\": [42, {\"key\": [null, true, 3.14]}]}").GetRoot();
    TEST(complex.IsMap());
    TEST(complex.AsMap().at("data").IsArray());
    TEST(complex.AsMap().at("data").AsArray()[0].AsInt() == 42);
    TEST(complex.AsMap().at("data").AsArray()[1].AsMap().at("key").AsArray()[1].AsBool() == true);
}

DEFINE_TEST_G(ParsingDict, Json_Testing) {
    Node dict = LoadJSON("{\"key1\": 1, \"key2\": \"value\"}").GetRoot();
    TEST(dict.IsMap());
    TEST(dict.AsMap().at("key1").AsInt() == 1);
    TEST(dict.AsMap().at("key2").AsString() =="value");
}

DEFINE_TEST_G(ParsingPrimitives, Json_Testing) {
    TEST(LoadJSON("null").GetRoot().IsNull());
    TEST(LoadJSON("42").GetRoot().AsInt() == 42);
    TEST(LoadJSON("3.14").GetRoot().AsDouble() == 3.14);
    TEST(LoadJSON("true").GetRoot().AsBool() == true);
    TEST(LoadJSON("false").GetRoot().AsBool() == false);
    TEST(LoadJSON("\"Hello\"").GetRoot().AsString() == "Hello");
}

DEFINE_TEST_G(PrintingPrimitives, Json_Testing) {
    TEST(PrintNodeToString(Node(nullptr)) == "null");
    TEST(PrintNodeToString(Node(42)) == "42");
    TEST(PrintNodeToString(Node(3.14)) == "3.14");
    TEST(PrintNodeToString(Node(true)) == "true");
    TEST(PrintNodeToString(Node(false)) == "false");
    TEST(PrintNodeToString(Node("Hello"s)) == "\"Hello\"");
}


DEFINE_TEST_G(MinifiedLoad, Json_Testing) {
    std::string s = "[{\"array\":[1,2,3],\"bool\":true,\"double\":42.1,\"int\":42,\"map\":{\"key\":\"value\",\"bool_key\":false},\"null\":null,\"string\":\"hello\"}]";
    Array arr;
    arr.emplace_back(Dict{
        {"int"s, 42},
        {"double"s, 42.1},
        {"null"s, nullptr},
        {"string"s, "hello"s},
        {"array"s, Array{1, 2, 3}},
        {"bool"s, true},
        {"map"s, Dict{{"key"s, "value"s}, {"bool_key"s, false}}},
        });
    std::stringstream strm;
    strm << s;
    const auto doc = Load(strm);
    TEST(doc.GetRoot() == arr);
}

DEFINE_TEST_G(ErrorHandling, Json_Testing) {
    MustFailToLoad("["s);
    MustFailToLoad("]"s);

    MustFailToLoad("{"s);
    MustFailToLoad("}"s);

    MustFailToLoad("\"hello"s);

    MustFailToLoad("tru"s);
    MustFailToLoad("fals"s);
    MustFailToLoad("nul"s);

    MustFailToLoad("truestory"s);

    MustFailToLoad("0123"s);

    Node dbl_node{3.5};
    MustThrowLogicError([&dbl_node] { 
        dbl_node.AsInt(); 
    });
    MustThrowLogicError([&dbl_node] {
        dbl_node.AsString();
    });
    MustThrowLogicError([&dbl_node] {
        dbl_node.AsArray();
    });

    Node array_node{Array{}};
    MustThrowLogicError([&array_node] {
        array_node.AsMap();
    });
    MustThrowLogicError([&array_node] {
        array_node.AsDouble();
    });
    MustThrowLogicError([&array_node] {
        array_node.AsBool();
    });
}

DEFINE_TEST_G(ParsingErrors, Json_Testing) {
    try {
        LoadJSON("[1, 2, 3");
        TEST_ERROR_("Expected ParsingError for missing closing bracket");
    } catch (const ParsingError&) {}

    try {
        LoadJSON("{\"key\": 1");
        TEST_ERROR_("Expected ParsingError for missing closing brace");
    } catch (const ParsingError&) {}

    try {
        LoadJSON("[1, 2, null, true, 3.14,,]");
        TEST_ERROR_("Expected ParsingError for malformed array");
    } catch (const ParsingError&) {}

    try {
        LoadJSON("{\"key\": 1, \"another_key\":}");
        TEST_ERROR_("Expected ParsingError for malformed object");
    } catch (const ParsingError&) {}

    try {
        LoadJSON("unquoted_string");
        TEST_ERROR_("Expected ParsingError for invalid literal");
    } catch (const ParsingError&) {}
}

}  // namespace

#endif