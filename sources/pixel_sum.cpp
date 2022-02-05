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

    // looks like i didnt miss any obvious input issue. 
    // The only remaining is discrepancy between buffer real size and provided sizes
    // assume that it wont happen

    // calculate integral image
    const int imageSize = xWidth * yHeight;
    m_integralImage.resize(imageSize);
    m_imageWidth = xWidth;
    m_imageHeight = yHeight;
    // prepare integral image preprocessed table also known as summed-area table
    // step one: plant the initial seed: [0,0] item equals to the value on the provided image
    m_integralImage[0] = buffer[0];

    // calculate the 1st line of the integral image
    // make it as a separate step to reduce amount of if's in the algorithm
    // well it is not an effecient optimization, but allow to read code better
    for (uint32_t i = 1; i < m_imageWidth; ++i)
    {
        m_integralImage[i] = m_integralImage[i - 1] + buffer[i];
    }
    // calculate the rest of the integral image using 2x2 window
    // integralValue[x,y] = buffer[x,y] + integralValue[x-1,y] + integralValue[x, y-1] - integralValue[x-1,y-1] 
    for (uint32_t line = 1; line < m_imageHeight; ++line)
    {
        const uint32_t y = line * m_imageWidth;
        // plant the seed of the new line
        // value of the 0th item on each line is the sum of current image value and calculated sum of all
        // zero items on the previous lines
        m_integralImage[y] = m_integralImage[0 + (y - m_imageWidth)] + buffer[y];
        for (uint32_t x = 1; x < m_imageWidth; ++x)
        {
            m_integralImage[x + y] = buffer[x + y] 
                + m_integralImage[x + (y - m_imageWidth)] 
                + m_integralImage[x - 1 + y] 
                - m_integralImage[x - 1 + (y - m_imageWidth)];
        }
    }
}

unsigned int PixelSum::getPixelSum(int x0, int y0, int x1, int y1) const
{
    // use algorythm proposed by Frank Crow to get integral image values
    // integral_image_value = integralValue[x0 - 1, y0 - 1] + integralValue[x1,y1] - integralValue[x0 - 1, y1] - integralValue[x1, y0 - 1]
    const uint32_t linearizedY0 = (y0 - 1) * m_imageWidth;
    const uint32_t linearizedY1 = y1 * m_imageWidth;
    const uint32_t correctedX0 = x0 - 1;

    return (m_integralImage.at(correctedX0 + linearizedY0) + m_integralImage.at(x1 + linearizedY1) - m_integralImage.at(correctedX0 + linearizedY1) - m_integralImage.at(x1 + linearizedY0));
}

// EOF