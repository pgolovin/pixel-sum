
#include "test/support_functions.h"
namespace naiveCalculations
{
    uint32_t sum(const std::vector<unsigned char>& image, uint32_t width, uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1)
    {
        uint32_t result = 0;
        for (uint32_t x = x0; x <= x1; ++x)
        {
            for (uint32_t y = y0; y <= y1; ++y)
            {
                result += image[x + y * width];
            }
        }
        return result;
    }

    double average(const std::vector<unsigned char>& image, uint32_t width, uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1)
    {
        uint32_t result = sum(image, width, x0, y0, x1, y1);
        // since all coordinates are inclusive, then when all coordinates are equal we should have 1 element;
        uint32_t length = x1 - x0 + 1;
        uint32_t height = y1 - y0 + 1;
        return (double)result / (length * height);
    }
}
