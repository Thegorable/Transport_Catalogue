#define DEBUG

#ifdef DEBUG

#pragma once
#include "../input_reader.h"
#include <string>
#include <filesystem>

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

#endif