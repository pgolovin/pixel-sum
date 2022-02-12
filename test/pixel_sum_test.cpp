
#include "include/pixel_sum.h"
#include "test/support_functions.h"

#include <gtest/gtest.h>
#include <memory>
#include <vector>

// algorythmic tests
class PS_IntegralImageTest : public ::testing::Test
{
protected:
    const static int m_imageWidth = 3;
    const static int m_imageHeight = 3;
    const std::vector<unsigned char> m_image = { 1, 0, 1, 0, 1, 0, 1, 0, 1 };
    std::unique_ptr<PixelSum> m_pixelSum;

    virtual void SetUp()
    {
        m_pixelSum = std::make_unique<PixelSum>(m_image.data(), m_imageWidth, m_imageHeight);
    }

    virtual void TearDown()
    {
    }
};

TEST_F(PS_IntegralImageTest, single_item_equals_to_input_value)
{
    uint32_t iiValue = m_pixelSum->getPixelSum(1, 1, 1, 1);
    ASSERT_EQ(1U, iiValue);
}

TEST_F(PS_IntegralImageTest, block_of_items_image)
{
    uint32_t iiValue = m_pixelSum->getPixelSum(1, 1, 2, 2);
    ASSERT_EQ(2U, iiValue);
}

TEST_F(PS_IntegralImageTest, full_image)
{
    uint32_t iiValue = m_pixelSum->getPixelSum(0, 0, 2, 2);
    uint32_t calcValue = naiveCalculations::sum(m_image, m_imageWidth, 0, 0, 2, 2);
    ASSERT_EQ(calcValue, iiValue);
}

TEST_F(PS_IntegralImageTest, check_0_0_item)
{
    uint32_t iiValue = m_pixelSum->getPixelSum(0, 0, 0, 0);
    ASSERT_EQ(m_image[0], iiValue);
}

TEST_F(PS_IntegralImageTest, negative_x_rectangle_bounds)
{
    uint32_t iiValue = m_pixelSum->getPixelSum(-2, 0, 1, 1);
    uint32_t calcValue = naiveCalculations::sum(m_image, m_imageWidth, 0, 0, 1, 1);
    ASSERT_EQ(calcValue, iiValue);
}

TEST_F(PS_IntegralImageTest, negative_y_rectangle_bounds)
{
    uint32_t iiValue = m_pixelSum->getPixelSum(0, -2, 1, 1);
    uint32_t calcValue = naiveCalculations::sum(m_image, m_imageWidth, 0, 0, 1, 1);
    ASSERT_EQ(calcValue, iiValue);
}

TEST_F(PS_IntegralImageTest, inverted_x_rectangle_bounds)
{
    uint32_t iiValue = m_pixelSum->getPixelSum(2, 0, 0, 2);
    uint32_t calcValue = naiveCalculations::sum(m_image, m_imageWidth, 0, 0, 2, 2);
    ASSERT_EQ(calcValue, iiValue);
}

TEST_F(PS_IntegralImageTest, inverted_y_rectangle_bounds)
{
    uint32_t iiValue = m_pixelSum->getPixelSum(0, 2, 2, 0);
    uint32_t calcValue = naiveCalculations::sum(m_image, m_imageWidth, 0, 0, 2, 2);
    ASSERT_EQ(calcValue, iiValue);
}

TEST_F(PS_IntegralImageTest, out_of_image_x_rectangle_bounds)
{
    uint32_t iiValue = m_pixelSum->getPixelSum(0, 0, 5, 2);
    uint32_t calcValue = naiveCalculations::sum(m_image, m_imageWidth, 0, 0, 2, 2);
    ASSERT_EQ(calcValue, iiValue);
}

TEST_F(PS_IntegralImageTest, out_of_image_y_rectangle_bounds)
{
    uint32_t iiValue = m_pixelSum->getPixelSum(0, 0, 2, 5);
    uint32_t calcValue = naiveCalculations::sum(m_image, m_imageWidth, 0, 0, 2, 2);
    ASSERT_EQ(calcValue, iiValue);
}

TEST_F(PS_IntegralImageTest, full_out_of_image_negative_x_rectangle_bounds)
{
    uint32_t iiValue = m_pixelSum->getPixelSum(-3, 0, -5, 5);
    ASSERT_EQ(0U, iiValue);
}

TEST_F(PS_IntegralImageTest, full_out_of_image_negative_y_rectangle_bounds)
{
    uint32_t iiValue = m_pixelSum->getPixelSum(0, -5, 2, -3);
    ASSERT_EQ(0U, iiValue);
}

TEST_F(PS_IntegralImageTest, full_out_of_image_x_rectangle_bounds)
{
    uint32_t iiValue = m_pixelSum->getPixelSum(3, 0, 5, 5);
    ASSERT_EQ(0U, iiValue);
}

TEST_F(PS_IntegralImageTest, full_out_of_image_y_rectangle_bounds)
{
    uint32_t iiValue = m_pixelSum->getPixelSum(0, 3, 2, 5);
    ASSERT_EQ(0U, iiValue);
}

TEST_F(PS_IntegralImageTest, partially_away_of_image_rectangle_bounds)
{
    uint32_t iiValue = m_pixelSum->getPixelSum(2, 15, 15, 2);
    ASSERT_EQ(1U, iiValue);
}

TEST_F(PS_IntegralImageTest, away_of_image_rectangle_bounds)
{
    uint32_t iiValue = m_pixelSum->getPixelSum(10, 15, 15, 10);
    ASSERT_EQ(0U, iiValue);
}

// advanced parametrized test, to check functionality in more complicated and unpredictable case
// thanks to rand()
struct RectangularBounds
{
    // input coordinates
    int32_t x0;
    int32_t y0;

    int32_t x1;
    int32_t y1;

    //validation coordinates
    int32_t vX0;
    int32_t vY0;

    int32_t vX1;
    int32_t vY1;
};

class PS_getPixelSumAdvancedTest : public ::testing::TestWithParam<RectangularBounds>
{
protected:
    
    const static int m_imageWidth = 500;
    const static int m_imageHeight = 500;
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
std::vector<unsigned char> PS_getPixelSumAdvancedTest::m_image = {};


TEST_P(PS_getPixelSumAdvancedTest, get_pixel_sum)
{
    const auto& coordinates = GetParam();
    uint32_t iiValue = m_pixelSum->getPixelSum(coordinates.x0, coordinates.y0, coordinates.x1, coordinates.y1);
    uint32_t calcValue = naiveCalculations::sum(m_image, m_imageWidth, coordinates.vX0, coordinates.vY0, coordinates.vX1, coordinates.vY1);
    ASSERT_EQ(calcValue, iiValue);
}

INSTANTIATE_TEST_SUITE_P(
    PS_GetPixelSumTest, PS_getPixelSumAdvancedTest,
    ::testing::Values(
        RectangularBounds{ 0, 0, 0, 0, 0, 0, 0, 0 },
        RectangularBounds{ 0, 0, 499, 499, 0, 0, 499, 499 },
        RectangularBounds{ 1, 1, 499, 499, 1, 1, 499, 499 },
        RectangularBounds{ -5, 1, 600, 499, 0, 1, 499, 499 },
        RectangularBounds{ 300, 100, 600, 499, 300, 100, 499, 499 },
        RectangularBounds{ 200, 400, 400, -5, 200, 0, 400, 400 },
        RectangularBounds{ 400, 400, 200, -5, 200, 0, 400, 400 }
        )
);
