#include "include/pixel_sum.h"
#include "test/support_functions.h"

#include <gtest/gtest.h>

#include <memory>
#include <vector>

// algorythmic tests
class PS_NonZeroesCountTests : public ::testing::Test
{
protected:

    const static int m_imageWidth = 500;
    const static int m_imageHeight = 900;
    static int m_nonZeroes;
    static std::vector<unsigned char> m_image;
    std::unique_ptr<PixelSum> m_pixelSum;

    static void SetUpTestCase()
    {
        m_image.resize(m_imageWidth * m_imageHeight);
        for (auto& pixel : m_image)
        {
            // make only 1/3rd part of elements non-zeroes
            pixel = (rand() % 3 == 0) ? (uint8_t)(rand() % 255) : 0;

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

int PS_NonZeroesCountTests::m_nonZeroes = 0;
std::vector<unsigned char> PS_NonZeroesCountTests::m_image = {};

TEST_F(PS_NonZeroesCountTests, single_item_zeroed_not_counted)
{
    for (int x = 0; x < m_imageWidth; ++x)
    {
        for (int y = 0; y < m_imageHeight; ++y)
        {
            if (m_image[x + y * m_imageWidth])
            {
                ASSERT_EQ(1, m_pixelSum->getNonZeroCount(x, y, x, y));
            }
            else
            {
                ASSERT_EQ(0, m_pixelSum->getNonZeroCount(x, y, x, y));
            }
        }
    }
}

TEST_F(PS_NonZeroesCountTests, full_image_counted_precalculated)
{
    ASSERT_EQ(m_nonZeroes, m_pixelSum->getNonZeroCount(0, 0, m_imageWidth - 1, m_imageHeight - 1));
}

TEST_F(PS_NonZeroesCountTests, full_image_counted)
{
    ASSERT_EQ(naiveCalculations::nonZeroes(m_image, m_imageWidth, 0, 0, m_imageWidth-1, m_imageHeight-1),
        m_pixelSum->getNonZeroCount(0, 0, m_imageWidth-1, m_imageHeight-1));
}

TEST_F(PS_NonZeroesCountTests, sub_image_counted)
{
    int x0 = 100;
    int y0 = 75;
    int x1 = m_imageWidth - 100;
    int y1 = m_imageHeight - 150;
    ASSERT_EQ(naiveCalculations::nonZeroes(m_image, m_imageWidth, x0, y0, x1, y1), m_pixelSum->getNonZeroCount(x0, y0, x1, y1));
}

TEST_F(PS_NonZeroesCountTests, out_of_x_image_counted)
{
    ASSERT_EQ(naiveCalculations::nonZeroes(m_image, m_imageWidth, 100, 0, m_imageWidth - 1, m_imageHeight - 1),
        m_pixelSum->getNonZeroCount(100, 0, m_imageWidth + 1000, m_imageHeight - 1));
}

TEST_F(PS_NonZeroesCountTests, out_of_y_image_counted)
{
    ASSERT_EQ(naiveCalculations::nonZeroes(m_image, m_imageWidth, 0, 100, m_imageWidth - 1, m_imageHeight - 1),
        m_pixelSum->getNonZeroCount(0, 100, m_imageWidth - 1, m_imageHeight + 1000));
}

TEST_F(PS_NonZeroesCountTests, negative_out_of_x_image_counted)
{
    ASSERT_EQ(naiveCalculations::nonZeroes(m_image, m_imageWidth, 0, 0, m_imageWidth - 1, m_imageHeight - 1),
        m_pixelSum->getNonZeroCount(-100, 0, m_imageWidth + 1000, m_imageHeight - 1));
}

TEST_F(PS_NonZeroesCountTests, negative_out_of_y_image_counted)
{
    ASSERT_EQ(naiveCalculations::nonZeroes(m_image, m_imageWidth, 0, 0, m_imageWidth - 1, m_imageHeight - 50),
        m_pixelSum->getNonZeroCount(0, -100, m_imageWidth - 1, m_imageHeight - 50));
}

TEST_F(PS_NonZeroesCountTests, negative_out_of_image)
{
    ASSERT_EQ(0, m_pixelSum->getNonZeroCount(100, -1000, m_imageWidth - 25, -50));
}

TEST_F(PS_NonZeroesCountTests, positive_out_of_image)
{
    ASSERT_EQ(0, m_pixelSum->getNonZeroCount(m_imageWidth + 100, 10, m_imageWidth + 25, 50));
}


TEST_F(PS_NonZeroesCountTests, positive_away_of_image)
{
    ASSERT_EQ(0, m_pixelSum->getNonZeroCount(m_imageWidth + 100, m_imageHeight + 10, m_imageWidth + 150, m_imageHeight + 50));
}

TEST_F(PS_NonZeroesCountTests, negative_away_of_image)
{
    ASSERT_EQ(0, m_pixelSum->getNonZeroCount(-100, -10, -100, -50));
}

TEST_F(PS_NonZeroesCountTests, full_image_counted_for_copied_object)
{
    std::unique_ptr<PixelSum> pixelSum = std::make_unique<PixelSum>(*m_pixelSum);
    ASSERT_EQ(pixelSum->getNonZeroCount(0, 0, m_imageWidth - 1, m_imageHeight - 1), m_pixelSum->getNonZeroCount(0, 0, m_imageWidth - 1, m_imageHeight - 1));
}

TEST_F(PS_NonZeroesCountTests, full_image_counted_for_assigned_object)
{
    std::vector<unsigned char> local_image = { 0,0,0,0 };
    std::unique_ptr<PixelSum> pixelSum = std::make_unique<PixelSum>(local_image.data(), 2, 2);
    ASSERT_NE(pixelSum->getNonZeroCount(0, 0, m_imageWidth - 1, m_imageHeight - 1), m_pixelSum->getNonZeroCount(0, 0, m_imageWidth - 1, m_imageHeight - 1));
    *pixelSum = *m_pixelSum;
    ASSERT_EQ(pixelSum->getNonZeroCount(0, 0, m_imageWidth - 1, m_imageHeight - 1), m_pixelSum->getNonZeroCount(0, 0, m_imageWidth - 1, m_imageHeight - 1));
}
