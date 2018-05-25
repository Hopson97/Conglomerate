#pragma once

#include <chrono>
#include "Filesystem.h"

template<typename F>
auto timeFunction(F f)
{
    auto begin = std::chrono::high_resolution_clock::now();
    f();
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = end - begin;
    return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
}

std::string tryExtractHeaderNameFromInclude(const std::string& line);