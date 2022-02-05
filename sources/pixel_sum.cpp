#include "include/pixel_sum.h"
#include <stdexcept>

PixelSum::PixelSum(const unsigned char* buffer, int xWidth, int yHeight)
{
    // Choose exception here
    // Reason: if data is invalid then object should not be constructed
    //         according to the standard this can be acheived by throwing exception.
    if (!buffer)
    {
        throw std::invalid_argument("Null image provided");
    }
    // well since no clear specification about sizes restrictions was provided
    // i assume i can treat 0 as an invalid size
    if (xWidth <= 0 || yHeight <= 0)
    {
        throw std::invalid_argument("Negative or Zero image sizes provided");
    }
}

// EOF