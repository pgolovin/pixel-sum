#include "include/pixel_sum.h"
#include <assert.h>

int PixelSum::bufferDimensionLimit = 4096;

PixelSum::PixelSum(const unsigned char* buffer, int xWidth, int yHeight)
{
    // Choose exception here
    // Reason: if data is invalid then object should not be constructed
    //         according to the standard this can be acheived by throwing exception.
    if (!buffer)
    {
        throw NullImageProvided();
    }
    // well since no clear specification about sizes restrictions were provided
    // i assume 0 can be treated as an invalid size
    // Comment: to reduce amount of checks I'd propose to use unsiged type
    if (xWidth <= 0 || yHeight <= 0)
    {
        throw NegativeOrZeroSizeProvided();
    }
    if (xWidth > bufferDimensionLimit || yHeight > bufferDimensionLimit)
    {
        throw MaximumSizeExceeded();
    }

    // looks like i didn't miss any obvious input issue. 
    // The only remaining is discrepancy between buffer real size and provided sizes

    // Calculate integral image
    // since calculations require to check previous x or y value we should secure situation 
    // also keep in mind that retrieving functions should return value as fast as possible
    // so we have to avoid unnecessary checks at all costs
    // when we are asking for the region with x0 = 0 or y0 = 0. because x0 - 1 or y0 - 1
    // in this case will lead to negative item access and crash
    // since we have no memory restrictions lets add safe zones here filled with zeroes:
    // 0 0 0 0
    // 0 X X X
    // 0 X X X
    // 0 X X X
    m_integralImageWidth = xWidth + m_shift;
    m_integralImageHeight = yHeight + m_shift;
    m_integralImage.resize(m_integralImageWidth * m_integralImageHeight);
    m_nonZeroIntegralImage.resize(m_integralImageWidth * m_integralImageHeight);
    
    // all stored parameters are related to the original image.
    m_imageWidth = xWidth;
    m_imageHeight = yHeight;
    // create x = 0 safe zone
    for (uint32_t i = 0; i < m_integralImageWidth; ++i)
    {
        m_integralImage[i] = 0;
        m_nonZeroIntegralImage[i] = 0;
    }
    // calculate the integral image using 2x2 window, kkeping in mind safe zones
    // integralValue[x,y] = buffer[x,y] + integralValue[x-1,y] + integralValue[x, y-1] - integralValue[x-1,y-1] 
    for (uint32_t line = 0; line < m_imageHeight; ++line)
    {
        const uint32_t y = line * m_imageWidth;
        uint32_t iiY = (line + m_shift) * m_integralImageWidth;
        // the first item in every line will be 0. y safe zone
        m_integralImage[iiY] = 0;
        m_nonZeroIntegralImage[iiY] = 0;

        // calculate the integral image. since safe zone values a 0, 
        // we don't need to add special case for border coordinates.

        for (uint32_t x = 0; x < m_imageWidth; ++x)
        {
            const uint32_t iiX = x + m_shift;
            m_integralImage[iiX + iiY] = buffer[x + y]
                + m_integralImage[iiX + (iiY - m_integralImageWidth)]
                + m_integralImage[iiX - 1 + iiY]
                - m_integralImage[iiX - 1 + (iiY - m_integralImageWidth)];

            // create non-zero image mask
            const uint32_t nonZero = buffer[x + y] ? 1 : 0;
            m_nonZeroIntegralImage[iiX + iiY] = nonZero
                + m_nonZeroIntegralImage[iiX + (iiY - m_integralImageWidth)]
                + m_nonZeroIntegralImage[iiX - 1 + iiY]
                - m_nonZeroIntegralImage[iiX - 1 + (iiY - m_integralImageWidth)];
        }
    }
}

PixelSum::PixelSum(const PixelSum& original)
{
    //assuming that original is always valid, since you cannot construct invalid object
    //reinterpret_cast case it is the only possible solution to break the algorythm
    //but in the case of reinterpret cast hack, it is hard to detect violation. 
    //need to create some keys/checksums to validate input data. assume this is out of the task scope

    copyContent(original);
}

PixelSum& PixelSum::operator= (const PixelSum& original)
{
    //the same note as for Copy Constructor
    copyContent(original);
    return *this;
}

void PixelSum::copyContent(const PixelSum& original)
{
    // just copy content of the original object
    m_integralImage = original.m_integralImage;
    m_imageHeight = original.m_imageHeight;
    m_imageWidth = original.m_imageWidth;
    m_integralImageHeight = original.m_integralImageHeight;
    m_integralImageWidth = original.m_integralImageWidth;
}

unsigned int PixelSum::getPixelSum(int x0, int y0, int x1, int y1) const
{
    // if no elements are requested, return 0 as their sum;
    if (!validateAndFixInput(x0, y0, x1, y1))
    {
        return 0;
    }

    return calculateFrankCrowValue(m_integralImage, x0, y0, x1, y1);
}

double PixelSum::getPixelAverage(int x0, int y0, int x1, int y1) const
{
    uint32_t num_elements = validateAndFixInput(x0, y0, x1, y1);
    if (0 == num_elements)
    {
        return 0;
    }

    return (double)calculateFrankCrowValue(m_integralImage, x0, y0, x1, y1) / num_elements;
}

int PixelSum::getNonZeroCount(int x0, int y0, int x1, int y1) const
{
    if (!validateAndFixInput(x0, y0, x1, y1))
    {
        return 0;
    }
    return calculateFrankCrowValue(m_nonZeroIntegralImage, x0, y0, x1, y1);
}

uint32_t PixelSum::calculateFrankCrowValue(const std::vector<uint32_t>& table, int x0, int y0, int x1, int y1) const
{
    // use algorythm proposed by Frank Crow to get integral image values O(1)
    return table.at(x0 + y0) + table.at(x1 + y1) - table.at(x0 + y1) - table.at(x1 + y0);
}


uint32_t PixelSum::validateAndFixInput(int& x0, int& y0, int& x1, int& y1) const
{
    // calculate number of elements that we should use in our calculations
    uint32_t num_elements = 0;
    // input correction. swap coordinates if order is incorrect
    if (x0 > x1)
    {
        std::swap(x0, x1);
    }
    if (y0 > y1)
    {
        std::swap(y0, y1);
    }
    //return out of bound if the requested area is fully out of image.
    if (x1 < 0 || y1 < 0 ||
        x0 > static_cast<int>(m_imageWidth) ||
        y0 > static_cast<int>(m_imageHeight))
    {
        return num_elements; // in this case it is 0;
    }
    // save number of elements here.
    // add 1 for both coordinates since boundaries are inclusive
    num_elements = (x1 - x0 + 1) * (y1 - y0 + 1);

    // it is impossible to have 0 here, if it's happened, then all checks above has a misstake
    assert(num_elements > 0);

    // move bounds inside the image, and linearize them.
    // from this moment coordinates are no longer represent real number of elements
    // that we should use in our calculations, they are truncated by the image
    y0 = std::max(0, y0) * m_integralImageWidth;
    y1 = std::min(y1 + m_shift, m_integralImageHeight - m_shift) * m_integralImageWidth;
    x0 = std::max(0, x0);
    x1 = std::min(x1 + m_shift, m_integralImageWidth - m_shift);

    return num_elements;
}

// EOF