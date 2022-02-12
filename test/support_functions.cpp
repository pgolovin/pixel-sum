
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

    int nonZeroes(const std::vector<unsigned char>& image, uint32_t width, uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1)
    {
        int result = 0;
        for (uint32_t x = x0; x <= x1; ++x)
        {
            for (uint32_t y = y0; y <= y1; ++y)
            {
                if (image[x + y * width] != 0)
                {
                    ++result;
                }
            }
        }
        return result;
    }

    double nonZeroAvg(const std::vector<unsigned char>& image, uint32_t width, uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1)
    {
        int count = nonZeroes(image, width, x0, y0, x1, y1);
        if (!count)
        {
            return 0;
        }
        return (double)sum(image, width, x0, y0, x1, y1) / count;
    }
}
