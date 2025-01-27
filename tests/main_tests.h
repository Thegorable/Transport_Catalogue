#define DEBUG

#ifdef DEBUG

#pragma once
#include "../input_reader.h"
#include <string>

std::vector<std::string> ReadCurrentLayerStream(std::ifstream& stream);

#endif