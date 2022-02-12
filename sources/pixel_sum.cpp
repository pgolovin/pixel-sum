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

    // calculate the integral image using 2x2 window
    // integralValue[x,y] = buffer[x,y] + integralValue[x-1,y] + integralValue[x, y-1] - integralValue[x-1,y-1] 
    for (uint32_t line = 0; line < m_imageHeight; ++line)
    {
        const uint32_t y = line * m_imageWidth;
        iiY = (line + m_shift) * m_integralImageWidth;
        // the first item in every line will be 0. y safe zone
        m_integralImage[iiY] = 0;

        // calculate the integral image. since safe zone values a 0, 
        // we don't need to add special case for border coordinates.

        for (uint32_t x = 0; x < m_imageWidth; ++x)
        {
            const uint32_t iiX = x + m_shift;
            m_integralImage[iiX + iiY] = buffer[x + y]
                + m_integralImage[iiX + (iiY - m_integralImageWidth)]
                + m_integralImage[iiX - 1 + iiY]
                - m_integralImage[iiX - 1 + (iiY - m_integralImageWidth)];
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
    // TODO: whole input correction block loosk ugle. refactoring is required. but tomorrow.

    // input correction. swap coordinates if order is incorrect
    if (x0 > x1)
    {
        std::swap(x0, x1);
    }
    if (y0 > y1)
    {
        std::swap(y0, y1);
    }
    if (x1 < 0 || 
        y1 < 0 || 
        x0 > static_cast<int>(m_imageWidth) || 
        y0 > static_cast<int>(m_imageHeight))
    {
        return 0;
    }
    // use algorythm proposed by Frank Crow to get integral image values
    // integral_image_value = integralValue[x0 - 1, y0 - 1] + integralValue[x1,y1] - integralValue[x0 - 1, y1] - integralValue[x1, y0 - 1]
    const uint32_t linearizedY0 = std::max(0, y0) * m_integralImageWidth;
    const uint32_t linearizedY1 = std::min(y1 + m_shift, m_integralImageHeight - m_shift) * m_integralImageWidth;
    const uint32_t correctedX0 = std::max(0, x0);
    const uint32_t correctedX1 = std::min(x1 + m_shift, m_integralImageWidth - m_shift);

    return m_integralImage.at(correctedX0 + linearizedY0)
        + m_integralImage.at(correctedX1 + linearizedY1)
        - m_integralImage.at(correctedX0 + linearizedY1)
        - m_integralImage.at(correctedX1 + linearizedY0);
}

// EOF