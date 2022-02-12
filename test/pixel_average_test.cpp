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
    static std::vector<unsigned char> m_image;
    std::unique_ptr<PixelSum> m_pixelSum;

    static void SetUpTestCase()
    {
        m_image.resize(m_imageWidth * m_imageHeight);
        for (auto& pixel : m_image)
        {
            pixel = (uint8_t)(rand() % 255);
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

std::vector<unsigned char> PS_AverageTests::m_image = {};

TEST_F(PS_AverageTests, single_item_equals_to_input_value)
{
    for (int x = 0; x < m_imageWidth; ++x)
    {
        for (int y = 0; y < m_imageHeight; ++y)
        {
            ASSERT_DOUBLE_EQ(m_image[x + y * m_imageWidth], m_pixelSum->getPixelAverage(x, y, x, y));
        }
    }
}

TEST_F(PS_AverageTests, full_image_average)
{
    ASSERT_DOUBLE_EQ(naiveCalculations::average(m_image, m_imageWidth, 0, 0, m_imageWidth-1, m_imageHeight-1),
        m_pixelSum->getPixelAverage(0, 0, m_imageWidth-1, m_imageHeight-1));
}

TEST_F(PS_AverageTests, partially_out_of_image_average_negative)
{
    double average = (double)naiveCalculations::sum(m_image, m_imageWidth, 0, 0, m_imageWidth - 1, m_imageHeight - 1) / 
        ((m_imageWidth + 100) * (m_imageHeight + 100));
    ASSERT_DOUBLE_EQ(average, m_pixelSum->getPixelAverage(-100, -100, m_imageWidth - 1, m_imageHeight - 1));
}

TEST_F(PS_AverageTests, partially_out_of_image_average_positive)
{
    double average = (double)naiveCalculations::sum(m_image, m_imageWidth, 100, 100, m_imageWidth - 1, m_imageHeight - 1) /
        (m_imageWidth * m_imageHeight);
    // m_imageWidth + 99 = (m_imageWidth - 1) + 100;
    ASSERT_DOUBLE_EQ(average, m_pixelSum->getPixelAverage(100, 100, m_imageWidth + 99, m_imageHeight + 99));
}

TEST_F(PS_AverageTests, image_average_x_bounds)
{
    uint32_t inside_line = 5;
    uint32_t outside_line = 10;
    double average = (double)naiveCalculations::sum(m_image, m_imageWidth, m_imageWidth - inside_line, 0, m_imageWidth - 1, m_imageHeight - 1) /
        (m_imageHeight * (outside_line + inside_line + 1));
    ASSERT_DOUBLE_EQ(average, m_pixelSum->getPixelAverage(m_imageWidth - inside_line, 0, m_imageWidth + outside_line, m_imageHeight-1));
}

TEST_F(PS_AverageTests, image_average_y_bounds)
{
    uint32_t inside_line = 5;
    uint32_t outside_line = 10;
    double average = (double)naiveCalculations::sum(m_image, m_imageWidth, 0, m_imageHeight - inside_line, m_imageWidth - 1, m_imageHeight - 1) /
        (m_imageWidth * (outside_line + inside_line + 1));
    ASSERT_DOUBLE_EQ(average, m_pixelSum->getPixelAverage(0, m_imageHeight - inside_line, m_imageWidth-1, m_imageHeight + outside_line));
}

