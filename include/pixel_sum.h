#pragma once
#include <vector>
#include <stdexcept>
//----------------------------------------------------------------------------
// Class for providing fast region queries from an 8-bit pixel buffer.
// Note: all coordinates are *inclusive* and clamped internally to the borders
// of the buffer by the implementation.
//
// For example: getPixelSum(4,8,7,10) gets the sum of a 4x3 region where top left
// corner is located at (4,8) and bottom right at (7,10). In other words
// all coordinates are _inclusive_.
//
// If the resulting region after clamping is empty, the return value for all
// functions should be 0.
//
// The width and height of the buffer dimensions < 4096 x 4096.
//----------------------------------------------------------------------------
class NullImageProvided : public std::exception
{
    // nothing to do here
};

class NegativeOrZeroSizeProvided : public std::exception
{
    // nothing to do here
};

class MaximumSizeExceeded : public std::exception
{
    // nothing to do here
};

class PixelSum
{
public:

    // original interface
    PixelSum(const unsigned char* buffer, int xWidth, int yHeight);
    ~PixelSum(void) {};

    PixelSum(const PixelSum&);
    PixelSum& operator= (const PixelSum&);

    unsigned int getPixelSum(int x0, int y0, int x1, int y1) const;
    double getPixelAverage(int x0, int y0, int x1, int y1) const;
    int getNonZeroCount(int x0, int y0, int x1, int y1) const;
    double getNonZeroAverage(int x0, int y0, int x1, int y1) const;

    static const int bufferDimensionLimit = 4096;
private:
    void copyContent(const PixelSum& original);
    // validate input parameters and convert them to linearized indexes in a 1D vector
    // with respect to negative values and bounds restricitions
    uint32_t validateAndFixInput(int& x0, int& y0, int& x1, int& y1) const;
    // Calculate value using proposed algorythms for Frank Crow for MipMaps
    // it should be inlined here, I suppose compiler will do this for me;
    uint32_t calculateFrankCrowValue(const std::vector<uint32_t>& table, int x0, int y0, int x1, int y1) const;

    uint32_t m_imageWidth = 0;
    uint32_t m_imageHeight = 0;

    // Store integral image (summed table) instead of original image
    std::vector<uint32_t> m_integralImage;
    // For non-zeroes use the same approach, create summed table where 
    //  - 0 if pixel is zero
    //  - 1 if pixel has value
    // will be summed instead of value
    std::vector<uint32_t> m_nonZeroIntegralImage;

    uint32_t m_integralImageWidth = 1;
    uint32_t m_integralImageHeight = 1;

    // Algorithm uses paradigm to store summed area table with shift according to original image
    // see details in implementation
    const uint32_t m_shift = 1;
};
//EOF