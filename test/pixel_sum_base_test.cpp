
#include "include/pixel_sum.h"
#include <gtest/gtest.h>
#include <memory>
#include <array>
#include <exception>

class PS_BasicTest : public ::testing::Test
{
protected:
    std::vector<unsigned char> m_image = { 0, 1, 2, 3 };
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

// Content is copied
TEST_F(PS_BasicTest, copy_content)
{
    m_pixelSum = std::make_unique<PixelSum>(m_image.data(), 2, 2);
    unsigned int iiValue = m_pixelSum->getPixelSum(0, 0, 1, 1);
    for (auto& pixel : m_image)
    {
        pixel = 0;
    }
    ASSERT_GE(iiValue, m_pixelSum->getPixelSum(0, 0, 1, 1));
}

TEST_F(PS_BasicTest, copy_constructor)
{
    m_pixelSum = std::make_unique<PixelSum>(m_image.data(), 2, 2);
    std::unique_ptr<PixelSum> pixelSumCopy = std::make_unique<PixelSum>(*m_pixelSum);
    ASSERT_GE(pixelSumCopy->getPixelSum(0, 0, 1, 1), m_pixelSum->getPixelSum(0, 0, 1, 1));
}


TEST_F(PS_BasicTest, assignment_operator)
{
    m_pixelSum = std::make_unique<PixelSum>(m_image.data(), 2, 2);
    std::array<unsigned char, 20 * 10> local_image = { 100, 100, 100, 100 };
    std::unique_ptr<PixelSum> pixelSumCopy = std::make_unique<PixelSum>(local_image.data(), 20, 10);
    ASSERT_NE(pixelSumCopy->getPixelSum(0, 0, 1, 1), m_pixelSum->getPixelSum(0, 0, 1, 1));
    *pixelSumCopy = *m_pixelSum;
    ASSERT_EQ(pixelSumCopy->getPixelSum(0, 0, 1, 1), m_pixelSum->getPixelSum(0, 0, 1, 1));
}
