#include "include/pixel_sum.h"
#include "test/support_functions.h"

#include <gtest/gtest.h>

#include <memory>
#include <vector>

// algorythmic tests
class PS_AverageTests : public ::testing::Test
{
protected:

    const static int m_imageWidth = 500;
    const static int m_imageHeight = 400;
    static uint32_t m_pixelSumValue;
    static std::vector<unsigned char> m_image;
    std::unique_ptr<PixelSum> m_pixelSum;

    static void SetUpTestCase()
    {
        m_image.resize(m_imageWidth * m_imageHeight);
        for (auto& pixel : m_image)
        {
            pixel = (uint8_t)(rand() % 255);
            m_pixelSumValue += pixel;
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
uint32_t PS_AverageTests::m_pixelSumValue = 0;
std::vector<unsigned char> PS_AverageTests::m_image = {};

TEST_F(PS_AverageTests, test_from_task)
{
    std::vector<unsigned char> local_image = { 0, 4, 0, 2, 1, 0 };
    m_pixelSum = std::make_unique<PixelSum>(local_image.data(), 3, 2);
    ASSERT_EQ(7/6., m_pixelSum->getPixelAverage(0, 0, 2, 1));
}

TEST_F(PS_AverageTests, single_pixel_equals_to_input_value)
{
    for (int x = 0; x < m_imageWidth; ++x)
    {
        for (int y = 0; y < m_imageHeight; ++y)
        {
            ASSERT_DOUBLE_EQ(m_image[x + y * m_imageWidth], m_pixelSum->getPixelAverage(x, y, x, y));
        }
    }
}

TEST_F(PS_AverageTests, zero_items_avg_is_zero)
{
    std::vector<unsigned char> local_image = { 0,0,0,0 };
    m_pixelSum = std::make_unique<PixelSum>(local_image.data(), 2, 2);
    ASSERT_DOUBLE_EQ(0, m_pixelSum->getPixelAverage(0, 0, 1, 1));
}

TEST_F(PS_AverageTests, full_image_average_precalculated)
{
    ASSERT_DOUBLE_EQ((double)m_pixelSumValue / (m_imageWidth* m_imageHeight), m_pixelSum->getPixelAverage(0, 0, m_imageWidth - 1, m_imageHeight - 1));
}

TEST_F(PS_AverageTests, full_image_average)
{
    double average = (double)naiveCalculations::sum(m_image, m_imageWidth, 0, 0, m_imageWidth - 1, m_imageHeight - 1) /
        (m_imageWidth * m_imageHeight);
    ASSERT_DOUBLE_EQ(average, m_pixelSum->getPixelAverage(0, 0, m_imageWidth - 1, m_imageHeight - 1));
}

TEST_F(PS_AverageTests, sub_image_average)
{
    int x0 = 100;
    int y0 = 75;
    int x1 = m_imageWidth - 100;
    int y1 = m_imageHeight - 150;
    double average = (double)naiveCalculations::sum(m_image, m_imageWidth, x0, y0, x1, y1) /
        ((x1 - x0 + 1) * (y1 - y0 + 1));
    ASSERT_DOUBLE_EQ(average, m_pixelSum->getPixelAverage(x0, y0, x1, y1));
}

TEST_F(PS_AverageTests, out_of_x_image_average)
{
    int x0 = 100;
    int y0 = 75;
    int x1 = m_imageWidth + 100;
    int y1 = m_imageHeight - 1;

    double average = (double)naiveCalculations::sum(m_image, m_imageWidth, x0, y0, m_imageWidth-1, y1) /
        ((x1 - x0 + 1) * (y1 - y0 + 1));

    ASSERT_DOUBLE_EQ(average, m_pixelSum->getPixelAverage(x0, y0, x1, y1));
}

TEST_F(PS_AverageTests, out_of_y_image_average)
{
    int x0 = 100;
    int y0 = 75;
    int x1 = m_imageWidth -1;
    int y1 = m_imageHeight + 100;

    double average = (double)naiveCalculations::sum(m_image, m_imageWidth, x0, y0, x1, m_imageHeight-1) /
        ((x1 - x0 + 1) * (y1 - y0 + 1));

    ASSERT_DOUBLE_EQ(average, m_pixelSum->getPixelAverage(x0, y0, x1, y1));
}

TEST_F(PS_AverageTests, negative_out_of_x_image_average)
{
    int x0 = -100;
    int y0 = 75;
    int x1 = m_imageWidth - 1;
    int y1 = 200;

    double average = (double)naiveCalculations::sum(m_image, m_imageWidth, 0, y0, x1, y1) /
        ((x1 - x0 + 1) * (y1 - y0 + 1));

    ASSERT_DOUBLE_EQ(average, m_pixelSum->getPixelAverage(x0, y0, x1, y1));
}

TEST_F(PS_AverageTests, negative_out_of_y_image_average)
{
    int x0 = 100;
    int y0 = -75;
    int x1 = m_imageWidth - 1;
    int y1 = 200;

    double average = (double)naiveCalculations::sum(m_image, m_imageWidth, x0, 0, x1, y1) /
        ((x1 - x0 + 1) * (y1 - y0 + 1));

    ASSERT_DOUBLE_EQ(average, m_pixelSum->getPixelAverage(x0, y0, x1, y1));
}

TEST_F(PS_AverageTests, negative_out_of_image)
{
    ASSERT_DOUBLE_EQ(0., m_pixelSum->getPixelAverage(100, -1000, m_imageWidth - 25, -50));
}

TEST_F(PS_AverageTests, positive_out_of_image)
{
    ASSERT_DOUBLE_EQ(0., m_pixelSum->getPixelAverage(m_imageWidth + 100, 10, m_imageWidth + 25, 50));
}

TEST_F(PS_AverageTests, positive_away_of_image)
{
    ASSERT_DOUBLE_EQ(0., m_pixelSum->getPixelAverage(m_imageWidth + 100, m_imageHeight + 10, m_imageWidth + 150, m_imageHeight + 50));
}

TEST_F(PS_AverageTests, negative_away_of_image)
{
    ASSERT_DOUBLE_EQ(0., m_pixelSum->getPixelAverage(-100, -10, -100, -50));
}

TEST_F(PS_AverageTests, full_image_counted_for_copied_object)
{
    std::unique_ptr<PixelSum> pixelSum = std::make_unique<PixelSum>(*m_pixelSum);
    ASSERT_DOUBLE_EQ(pixelSum->getPixelAverage(0, 0, m_imageWidth - 1, m_imageHeight - 1), m_pixelSum->getPixelAverage(0, 0, m_imageWidth - 1, m_imageHeight - 1));
}

TEST_F(PS_AverageTests, full_image_counted_for_assigned_object)
{
    std::vector<unsigned char> local_image = { 0,0,0,0 };
    std::unique_ptr<PixelSum> pixelSum = std::make_unique<PixelSum>(local_image.data(), 2, 2);
    ASSERT_NE(pixelSum->getPixelAverage(0, 0, m_imageWidth - 1, m_imageHeight - 1), m_pixelSum->getPixelAverage(0, 0, m_imageWidth - 1, m_imageHeight - 1));
    *pixelSum = *m_pixelSum;
    ASSERT_DOUBLE_EQ(pixelSum->getPixelAverage(0, 0, m_imageWidth - 1, m_imageHeight - 1), m_pixelSum->getPixelAverage(0, 0, m_imageWidth - 1, m_imageHeight - 1));
}

