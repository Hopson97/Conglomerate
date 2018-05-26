#pragma once

#include <chrono>
#include <optional>
#include "Filesystem.h"

template<typename F>
auto timeFunction(F f)
{
    auto begin = std::chrono::high_resolution_clock::now();
    f();
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = end - begin;
    return std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
}

bool lineContainsInclude    (const std::string& line);
bool lineContainsPragmaOnce (const std::string& line);
std::optional<std::string> tryExtractHeaderNameFromInclude(const std::string& line);