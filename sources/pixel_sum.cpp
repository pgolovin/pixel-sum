#include "include/pixel_sum.h"

PixelSum::PixelSum(const unsigned char* buffer, int xWidth, int yHeight)
{
    // Choose exception here
    // Reason: if data is invalid then object should not be constructed
    //         according to the standard this can be acheived by throwing exception.
    if (!buffer)
    {
        throw NullImageProvided();
    }
    // well since no clear specification about sizes restrictions was provided
    // i assume i can treat 0 as an invalid size
    // comment: to reduce amount of checks I'd propose to use unsiged type
    if (xWidth <= 0 || yHeight <= 0)
    {
        throw NegativeOrZeroSizeProvided();
    }
}

// EOF