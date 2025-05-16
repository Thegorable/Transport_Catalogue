#define DEBUG

#ifdef DEBUG

#pragma once
#include "../input_reader.h"
#include <string>
#include <filesystem>
#include <iostream>

#include "simpletest.h"

namespace fs = std::filesystem;

const fs::path TESTS_PATH = "H:/Programming/Training_projects/Transport_Catalogue_tests_data";
const fs::path IN_FILE_1 = "tsC_case0_input.txt";
const fs::path IN_FILE_2 = "tsC_case1_input.txt";
const fs::path IN_FILE_3 = "tsC_case2_input.txt";
const fs::path IN_FILE_4 = "tsC_case3_input.txt";
const fs::path OUT_FILE_1 = "tsC_case0_output.txt";
const fs::path OUT_FILE_2 = "tsC_case1_output.txt";
const fs::path OUT_FILE_3 = "tsC_case2_output.txt";
const fs::path OUT_FILE_4 = "tsC_case3_output1.txt";
const fs::path IN_FILE_JSON_1 = "json_testCase_1_input.json";
const fs::path OUT_FILE_JSON_1 = "json_testCase_1_output.json";
const fs::path IN_FILE_JSON_2 = "json_testCase_2_input.json";
const fs::path OUT_FILE_JSON_2 = "json_testCase_2_output.json";
const fs::path IN_FILE_JSON_3 = "json_testCase_3_input.json";
const fs::path OUT_FILE_JSON_3 = "json_testCase_3_output.json";
const fs::path IN_FILE_JSON_4 = "json_testCase_4_input.json";
const fs::path OUT_FILE_JSON_4 = "json_testCase_4_output.json";
const fs::path IN_FILE_JSON_5 = "json_testCase_5_input.json";
const fs::path OUT_FILE_JSON_5 = "json_testCase_5_output.json";
const fs::path IN_FILE_JSON_6 = "json_testCase_6_input.json";
const fs::path OUT_FILE_JSON_6 = "json_testCase_6_output.json";

const fs::path SVG_FILE_RESULT_JSON_1 = "svg_testCase_01.json";
const fs::path SVG_FILE_RESULT_SVG_1 = "svg_testCase_01.svg";

const fs::path IN_FILE_RENDER_2 = "render_testCase_2_input.json";
const fs::path OUT_FILE_RENDER_2 = "render_testCase_2_output.xml";
const fs::path IN_FILE_RENDER_3 = "render_testCase_3_input.json";
const fs::path OUT_FILE_RENDER_3 = "render_testCase_3_output.xml";
const fs::path IN_FILE_RENDER_4 = "render_testCase_4_input.json";
const fs::path OUT_FILE_RENDER_4 = "render_testCase_4_output.xml";
const fs::path IN_FILE_RENDER_5 = "render_testCase_5_input.json";
const fs::path OUT_FILE_RENDER_5 = "render_testCase_5_output.xml";
const fs::path IN_FILE_RENDER_7 = "render_testCase_7_input.json";
const fs::path OUT_FILE_RENDER_7 = "render_testCase_7_output.xml";
const fs::path IN_FILE_RENDER_12 = "render_testCase_12_input.json";
const fs::path OUT_FILE_RENDER_12 = "render_testCase_12_output.xml";
const fs::path IN_FILE_RENDER_23 = "render_testCase_23_input.json";
const fs::path OUT_FILE_RENDER_23 = "render_testCase_23_output.xml";
const fs::path IN_FILE_RENDER_9 = "render_testCase_9_input.json";
const fs::path OUT_FILE_RENDER_9 = "render_testCase_9_output.xml";

const fs::path IN_FILE_MAP_DEF = "map_testCase_default_input.json";
const fs::path OUT_FILE_MAP_DEF = "map_testCase_default_output.json";
const fs::path IN_FILE_MAP_1 = "map_testCase_1_input.json";
const fs::path OUT_FILE_MAP_1 = "map_testCase_1_output.json";
const fs::path IN_FILE_MAP_2 = "map_testCase_2_input.json";
const fs::path OUT_FILE_MAP_2 = "map_testCase_2_output.json";
const fs::path IN_FILE_MAP_3 = "map_testCase_3_input.json";
const fs::path OUT_FILE_MAP_3 = "map_testCase_3_output.json";

void CompareStrings(const std::string& input, const std::string& comparator, 
	std::ostream& your_out = std::cout, std::ostream& compare_out = std::cout);

class ExceptionFixture : public TestFixture
{
public:
	bool ExecuteTest() override
	{
		try
		{
			return TestFixture::ExecuteTest();
		}
		catch (const char* message)
		{
			TEST_ERROR_("Exception caught", "%s", message);
		}
		catch (...)
		{
			TEST_ERROR_("Exception caught", "Unknown exception");
		}
		return false;
	}
};

std::vector<std::string> ReadCurrentLayerStream(std::ifstream& stream);

template <typename container>
std::vector<std::string> VecPtrToVecNames(container stops_ptr) {
    std::vector<std::string> vec;
    for (auto stop_ptr : stops_ptr) {
        vec.push_back(stop_ptr->name_);
    }
    return vec;
};

std::string ReadFStream(std::ifstream& in);

#endif