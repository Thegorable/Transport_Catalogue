// #define DEBUG

#ifdef DEBUG

#pragma once
#include "../input_reader.h"
#include <string>

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