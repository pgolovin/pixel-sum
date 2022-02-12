#include "include/pixel_sum.h"
#include "test/support_functions.h"

#include <gtest/gtest.h>

#include <memory>
#include <vector>

// algorythmic tests
class PS_NonZeroesAverageTests : public ::testing::Test
{
protected:
    const static int m_imageWidth = 500;
    const static int m_imageHeight = 400;
    static uint32_t m_nonZeroes;
    static uint32_t m_pixelSumValue;
    static std::vector<unsigned char> m_image;
    std::unique_ptr<PixelSum> m_pixelSum;

    static void SetUpTestCase()
    {
        m_image.resize(m_imageWidth * m_imageHeight);
        for (auto& pixel : m_image)
        {
            // make only 1/3rd part of elements non-zeroes
            pixel = (rand() % 3 == 0) ? (uint8_t)(rand() % 255) : 0;

            m_pixelSumValue += pixel;
            if (pixel > 0)
            {
                ++m_nonZeroes;
            }
        }
    }

    virtual void SetUp()
    {
        m_pixelSum = std::make_unique<PixelSum>(m_image.data(), m_imageWidth, m_imageHeight);
    }

    virtual void TearDown()
    {
    }
};

uint32_t PS_NonZeroesAverageTests::m_nonZeroes = 0;
uint32_t PS_NonZeroesAverageTests::m_pixelSumValue = 0;
std::vector<unsigned char> PS_NonZeroesAverageTests::m_image = {};

TEST_F(PS_NonZeroesAverageTests, single_pixel_equals_to_input_value)
{
    for (int x = 0; x < m_imageWidth; ++x)
    {
        for (int y = 0; y < m_imageHeight; ++y)
        {
            ASSERT_DOUBLE_EQ(m_image[x + y * m_imageWidth], m_pixelSum->getNonZeroAverage(x, y, x, y));
        }
    }
}

TEST_F(PS_NonZeroesAverageTests, zero_items_avg_is_zero)
{
    std::vector<unsigned char> local_image = { 0,0,0,0 };
    m_pixelSum = std::make_unique<PixelSum>(local_image.data(), 2, 2);
    ASSERT_DOUBLE_EQ(0, m_pixelSum->getNonZeroAverage(0, 0, 1, 1));
}

TEST_F(PS_NonZeroesAverageTests, full_image_counted_precalculated)
{
    ASSERT_EQ((double)m_pixelSumValue/m_nonZeroes, m_pixelSum->getNonZeroAverage(0, 0, m_imageWidth - 1, m_imageHeight - 1));
}

TEST_F(PS_NonZeroesAverageTests, full_image_counted)
{
    double nonZeroAverageValue = naiveCalculations::nonZeroAvg(m_image, m_imageWidth, 0, 0, m_imageWidth - 1, m_imageHeight - 1);
    double getNonZeroAverageValue = m_pixelSum->getNonZeroAverage(0, 0, m_imageWidth - 1, m_imageHeight - 1);
    ASSERT_DOUBLE_EQ(nonZeroAverageValue, getNonZeroAverageValue);

    // ASSERT_EQ(naiveCalculations::nonZeroAvg(m_image, m_imageWidth, 0, 0, m_imageWidth - 1, m_imageHeight - 1),
    //     m_pixelSum->getNonZeroAverage(0, 0, m_imageWidth - 1, m_imageHeight - 1));
}

TEST_F(PS_NonZeroesAverageTests, sub_image_counted)
{
    int x0 = 100;
    int y0 = 75;
    int x1 = m_imageWidth - 100;
    int y1 = m_imageHeight - 150;
    ASSERT_DOUBLE_EQ(naiveCalculations::nonZeroAvg(m_image, m_imageWidth, x0, y0, x1, y1), m_pixelSum->getNonZeroAverage(x0, y0, x1, y1));
}

TEST_F(PS_NonZeroesAverageTests, out_of_x_image_counted)
{
    ASSERT_DOUBLE_EQ(naiveCalculations::nonZeroAvg(m_image, m_imageWidth, 100, 0, m_imageWidth - 1, m_imageHeight - 1),
        m_pixelSum->getNonZeroAverage(100, 0, m_imageWidth + 1000, m_imageHeight - 1));
}

TEST_F(PS_NonZeroesAverageTests, out_of_y_image_counted)
{
    ASSERT_DOUBLE_EQ(naiveCalculations::nonZeroAvg(m_image, m_imageWidth, 0, 100, m_imageWidth - 1, m_imageHeight - 1),
        m_pixelSum->getNonZeroAverage(0, 100, m_imageWidth - 1, m_imageHeight + 1000));
}

TEST_F(PS_NonZeroesAverageTests, negative_out_of_x_image_counted)
{
    ASSERT_DOUBLE_EQ(naiveCalculations::nonZeroAvg(m_image, m_imageWidth, 0, 0, m_imageWidth - 1, m_imageHeight - 1),
        m_pixelSum->getNonZeroAverage(-100, 0, m_imageWidth + 1000, m_imageHeight - 1));
}

TEST_F(PS_NonZeroesAverageTests, negative_out_of_y_image_counted)
{
    ASSERT_DOUBLE_EQ(naiveCalculations::nonZeroAvg(m_image, m_imageWidth, 0, 0, m_imageWidth - 1, m_imageHeight - 50),
        m_pixelSum->getNonZeroAverage(0, -100, m_imageWidth - 1, m_imageHeight - 50));
}

TEST_F(PS_NonZeroesAverageTests, negative_out_of_image)
{
    ASSERT_DOUBLE_EQ(0, m_pixelSum->getNonZeroAverage(100, -1000, m_imageWidth - 25, -50));
}

TEST_F(PS_NonZeroesAverageTests, positive_out_of_image)
{
    ASSERT_DOUBLE_EQ(0, m_pixelSum->getNonZeroAverage(m_imageWidth + 100, 10, m_imageWidth + 25, 50));
}

