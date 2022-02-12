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

    PixelSum(const unsigned char* buffer, int xWidth, int yHeight);
    ~PixelSum(void) {};

    PixelSum(const PixelSum&);
    PixelSum& operator= (const PixelSum&);

    unsigned int getPixelSum(int x0, int y0, int x1, int y1) const;
    double getPixelAverage(int x0, int y0, int x1, int y1) const;
    int getNonZeroCount(int x0, int y0, int x1, int y1) const;
    double getNonZeroAverage(int x0, int y0, int x1, int y1) const;

    static int bufferDimensionLimit;
private:

    void copyContent(const PixelSum& original);
    uint32_t validateAndFixInput(int& x0, int& y0, int& x1, int& y1) const;
    // calculate value using proposed algorythms for Frank Crow for MipMaps
    // it should be inlined here, i suppose compiler will do this for me;
    uint32_t calculateFrankCrowValue(const std::vector<uint32_t>& table, int x0, int y0, int x1, int y1) const;

    uint32_t m_imageWidth = 0;
    uint32_t m_imageHeight = 0;

    // Since our field is 4096sq max and each cell is 1Byte long
    // the max number that can be stored in integral image if all cells contains max available value
    // is 2^12 (4096) *2^12 * 2^8 = 2^(12+12+8) = 2^32 so uint32_t should sutisfy our purpose.
    // 
    // Q: what if size restrictions are not 4096 per edge but bigger,
    // A: uint32_t will not work here we should use wider types like uint64_t
    //    the same situation if cell size of initial image is not a Byte but Word
    std::vector<uint32_t> m_integralImage;
    uint32_t m_integralImageWidth = 1;
    uint32_t m_integralImageHeight = 1;

    // algorithm uses paradigm to store summed area table with shift according to original image
    // see details in implementation
    const uint32_t m_shift = 1;
};
//EOF