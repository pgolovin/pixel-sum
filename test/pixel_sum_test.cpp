
#include "include/pixel_sum.h"
#include <gtest/gtest.h>
#include <memory>
#include <array>
#include <exception>

class PS_BasicTest : public ::testing::Test
{
protected:
    std::vector<unsigned char> m_image = { 0, 0, 0, 0 };
    std::unique_ptr<PixelSum> m_pixelSum;

    virtual void SetUp()
    {
    }

    virtual void TearDown()
    {
    }
};

TEST_F(PS_BasicTest, can_create_class_with_valid_parameters)
{

    ASSERT_NO_THROW(m_pixelSum = std::make_unique<PixelSum>(m_image.data(), 2, 2));
}

//checking unobvious errors that can be generated by the interface construction
// 1. object cannot be created if buffer is not provided
TEST_F(PS_BasicTest, failed_create_class_nullptr)
{
    ASSERT_THROW(m_pixelSum = std::make_unique<PixelSum>(nullptr, 2, 2), NullImageProvided);
    ASSERT_TRUE(m_pixelSum == nullptr);
}

// 2. user provided incorrect sizes for the m_image, negative values
// 2.1 x - negative
TEST_F(PS_BasicTest, failed_create_class_negative_x_size)
{
    ASSERT_THROW(m_pixelSum = std::make_unique<PixelSum>(m_image.data(), -1, 2), NegativeOrZeroSizeProvided);
    ASSERT_TRUE(m_pixelSum == nullptr);
}

// 2.2 y - negative
TEST_F(PS_BasicTest, failed_create_class_negative_y_size)
{
    ASSERT_THROW(m_pixelSum = std::make_unique<PixelSum>(m_image.data(), 2, -1), NegativeOrZeroSizeProvided);
    ASSERT_TRUE(m_pixelSum == nullptr);
}

// 3. user provided incorrect sizes for the m_image, zero values
// 3.1 x - 0
TEST_F(PS_BasicTest, failed_create_class_zero_x_size)
{
    ASSERT_THROW(m_pixelSum = std::make_unique<PixelSum>(m_image.data(), 0, 2), NegativeOrZeroSizeProvided);
    ASSERT_TRUE(m_pixelSum == nullptr);
}

// 3.2 y - 0
TEST_F(PS_BasicTest, failed_create_class_zero_y_size)
{
    ASSERT_THROW(m_pixelSum = std::make_unique<PixelSum>(m_image.data(), 2, 0), NegativeOrZeroSizeProvided);
    ASSERT_TRUE(m_pixelSum == nullptr);
}

// 4. user provided incorrect sizes for the m_image, exceeding algorithm limits
// 4.1 width restriction
TEST_F(PS_BasicTest, failed_create_class_exceed_x_size)
{
    ASSERT_THROW(m_pixelSum = std::make_unique<PixelSum>(m_image.data(), PixelSum::bufferDimensionLimit + 1, 2), MaximumSizeExceeded);
    ASSERT_TRUE(m_pixelSum == nullptr);
}

// 4.1 height restriction
TEST_F(PS_BasicTest, failed_create_class_exceed_y_size)
{
    ASSERT_THROW(m_pixelSum = std::make_unique<PixelSum>(m_image.data(), 2, PixelSum::bufferDimensionLimit + 1), MaximumSizeExceeded);
    ASSERT_TRUE(m_pixelSum == nullptr);
}

// algorythmic tests
class PS_IntegralImageTest : public ::testing::Test
{
protected:
    const static int m_imageWidth = 3;
    const static int m_imageHeight = 3;
    const std::array<unsigned char, m_imageWidth * m_imageHeight> m_image = { 1, 0, 1, 0, 1, 0, 1, 0, 1 };
    std::unique_ptr<PixelSum> m_pixelSum;

    uint32_t naiveCalculation(uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1)
    {
        uint32_t result = 0;
        for (uint32_t x = x0; x <= x1; ++x)
        {
            for (uint32_t y = y0; y <= y1; ++y)
            {
                result += m_image[x + y * m_imageWidth];
            }
        }
        return result;
    }

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
    uint32_t calcValue = naiveCalculation(0, 0, 2, 2);
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
    uint32_t calcValue = naiveCalculation(0, 0, 1, 1);
    ASSERT_EQ(calcValue, iiValue);
}

TEST_F(PS_IntegralImageTest, negative_y_rectangle_bounds)
{
    uint32_t iiValue = m_pixelSum->getPixelSum(0, -2, 1, 1);
    uint32_t calcValue = naiveCalculation(0, 0, 1, 1);
    ASSERT_EQ(calcValue, iiValue);
}

TEST_F(PS_IntegralImageTest, inverted_x_rectangle_bounds)
{
    uint32_t iiValue = m_pixelSum->getPixelSum(2, 0, 0, 2);
    uint32_t calcValue = naiveCalculation(0, 0, 2, 2);
    ASSERT_EQ(calcValue, iiValue);
}

TEST_F(PS_IntegralImageTest, inverted_y_rectangle_bounds)
{
    uint32_t iiValue = m_pixelSum->getPixelSum(0, 2, 2, 0);
    uint32_t calcValue = naiveCalculation(0, 0, 2, 2);
    ASSERT_EQ(calcValue, iiValue);
}

TEST_F(PS_IntegralImageTest, out_of_image_x_rectangle_bounds)
{
    uint32_t iiValue = m_pixelSum->getPixelSum(0, 0, 5, 2);
    uint32_t calcValue = naiveCalculation(0, 0, 2, 2);
    ASSERT_EQ(calcValue, iiValue);
}

TEST_F(PS_IntegralImageTest, out_of_image_y_rectangle_bounds)
{
    uint32_t iiValue = m_pixelSum->getPixelSum(0, 0, 2, 5);
    uint32_t calcValue = naiveCalculation(0, 0, 2, 2);
    ASSERT_EQ(calcValue, iiValue);
}
