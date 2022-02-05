#include "include/pixel_sum.h"

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
    
    // all stored parameters are related to the original image.
    m_imageWidth = xWidth;
    m_imageHeight = yHeight;
    // create x = 0 safe zone
    for (uint32_t i = 0; i < m_integralImageWidth; ++i)
    {
        m_integralImage[i] = 0;
    }
    // the first item in every line will be 0. here we create y = 0 safe zone
    // ii here abbreviation for integral image
    uint32_t iiY = m_shift * m_integralImageWidth;
    m_integralImage[iiY] = 0;
    // keep in mind that image in m_integralImage is shifted by 1 on X and Y
    // prepare integral image preprocessed table also known as summed-area table
    // step one: plant the initial seed: [0,0] (here [1,1]) item equals to the value on the provided image
    m_integralImage[m_shift + iiY] = buffer[0];

    // calculate the 1st line of the integral image
    // make it as a separate step to reduce amount of if's in the algorithm
    // well it is not an effecient optimization, but allow to read code better
    for (uint32_t x = 1; x < m_imageWidth; ++x)
    {
        const uint32_t iiX = x + m_shift;
        m_integralImage[iiX + iiY] = m_integralImage[iiX - 1 + iiY] + buffer[x];
    }
    // calculate the rest of the integral image using 2x2 window
    // integralValue[x,y] = buffer[x,y] + integralValue[x-1,y] + integralValue[x, y-1] - integralValue[x-1,y-1] 
    for (uint32_t line = 1; line < m_imageHeight; ++line)
    {
        const uint32_t y = line * m_imageWidth;
        iiY = (line + m_shift) * m_integralImageWidth;
        // the first item in every line will be 0. y safe zone
        m_integralImage[iiY] = 0;
        // 1st valid item is equal to sum of original image value and callculated integral value on previous line
        m_integralImage[m_shift + iiY] = m_integralImage[m_shift + (iiY - m_integralImageWidth)] + buffer[y];

        for (uint32_t x = 1; x < m_imageWidth; ++x)
        {
            const uint32_t iiX = x + m_shift;
            m_integralImage[iiX + iiY] = buffer[x + y]
                + m_integralImage[iiX + (iiY - m_integralImageWidth)]
                + m_integralImage[iiX - 1 + iiY]
                - m_integralImage[iiX - 1 + (iiY - m_integralImageWidth)];
        }
    }
}

unsigned int PixelSum::getPixelSum(int x0, int y0, int x1, int y1) const
{
    // TODO: whole input correction block loosk ugle. refactoring is required. but tomorrow.

    // input correction. swap coordinates if order is incorrect
    if ((x0 < 0 && x1 < 0) || (y0 < 0 && y1 < 0))
    {
        return 0;
    }
    if (x0 > x1)
    {
        std::swap(x0, x1);
    }
    if (y0 > y1)
    {
        std::swap(y0, y1);
    }
    // use algorythm proposed by Frank Crow to get integral image values
    // integral_image_value = integralValue[x0 - 1, y0 - 1] + integralValue[x1,y1] - integralValue[x0 - 1, y1] - integralValue[x1, y0 - 1]
    const uint32_t linearizedY0 = std::max(0, y0) * m_integralImageWidth;
    const uint32_t linearizedY1 = std::min(y1 + m_shift, m_integralImageHeight - 1) * m_integralImageWidth;
    const uint32_t correctedX0 = std::max(0, x0);
    const uint32_t correctedX1 = std::min(x1 + m_shift, m_integralImageWidth - 1);

    return m_integralImage.at(correctedX0 + linearizedY0)
        + m_integralImage.at(correctedX1 + linearizedY1)
        - m_integralImage.at(correctedX0 + linearizedY1)
        - m_integralImage.at(correctedX1 + linearizedY0);
}

// EOF