#pragma once
#include <vector>

namespace naiveCalculations
{
    uint32_t sum(const std::vector<unsigned char>& image, uint32_t width, uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1);
    double average(const std::vector<unsigned char>& image, uint32_t width, uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1);
};

//EOF