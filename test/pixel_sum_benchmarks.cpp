#include "include/pixel_sum.h"
#include "test/support_functions.h"

#include <gtest/gtest.h>

#include <memory>
#include <vector>
#include <time.h>

#ifdef NDEBUG
#define TIMELIMIT 50
#else
#define TIMELIMIT 500
#endif

// algorythmic tests
class PS_Benchmarks : public ::testing::Test
{
protected:
    const static int m_imageWidth = PixelSum::bufferDimensionLimit;
    const static int m_imageHeight = PixelSum::bufferDimensionLimit;
    const static uint32_t m_iterrations = 1000000;

    static std::vector<unsigned char> m_image;
    static std::unique_ptr<PixelSum> m_pixelSum;

    clock_t m_testDuration = 0;
    // to spare time use the only one big image for full benchmarking time
    static void SetUpTestSuite()
    {
        m_image.resize(m_imageWidth * m_imageHeight);
        for (auto& pixel : m_image)
        {
            pixel = (rand() % 3 == 0) ? (uint8_t)(rand() % 255) : 0;
        }
        m_pixelSum = std::make_unique<PixelSum>(m_image.data(), m_imageWidth, m_imageHeight);

        std::cout << "Warn: benchmarks can be highly dependent on the machine you are executing them" << std::endl;
        std::cout << "      current benchmark executes all functions with timelimit: " << TIMELIMIT << "ms" << std::endl;
        std::cout << "      number of iterrations per function is: " << m_iterrations << std::endl;
    }

    virtual void SetUp()
    {
        m_testDuration = clock();
    }

    virtual void TearDown()
    {
        // get time to perform million of operations limited to 0.1 second for release
        m_testDuration = clock() - m_testDuration;
        ASSERT_LE(m_testDuration, TIMELIMIT);
    }
};

std::vector<unsigned char> PS_Benchmarks::m_image = {};
std::unique_ptr<PixelSum> PS_Benchmarks::m_pixelSum = nullptr;

TEST_F(PS_Benchmarks, pixel_sum_valid_input)
{
    for (uint32_t i = 0; i < m_iterrations; ++i)
    {
        m_pixelSum->getPixelSum(100, 100, m_imageWidth - 100, m_imageHeight - 100);
    }
}

TEST_F(PS_Benchmarks, pixel_sum_invalid_input)
{
    for (uint32_t i = 0; i < m_iterrations; ++i)
    {
        m_pixelSum->getPixelSum(m_imageWidth - 100, 100, - 100, m_imageHeight + 100);
    }
}

TEST_F(PS_Benchmarks, pixel_avg_valid_input)
{
    for (uint32_t i = 0; i < m_iterrations; ++i)
    {
        m_pixelSum->getPixelAverage(100, 100, m_imageWidth - 100, m_imageHeight - 100);
    }
}

TEST_F(PS_Benchmarks, pixel_avg_invalid_input)
{
    for (uint32_t i = 0; i < m_iterrations; ++i)
    {
        m_pixelSum->getPixelAverage(m_imageWidth - 100, 100, -100, m_imageHeight + 100);
    }
}

TEST_F(PS_Benchmarks, pixel_non_zeroes_valid_input)
{
    for (uint32_t i = 0; i < m_iterrations; ++i)
    {
        m_pixelSum->getNonZeroCount(100, 100, m_imageWidth - 100, m_imageHeight - 100);
    }
}

TEST_F(PS_Benchmarks, pixel_non_zeroes_invalid_input)
{
    for (uint32_t i = 0; i < m_iterrations; ++i)
    {
        m_pixelSum->getNonZeroCount(m_imageWidth - 100, 100, -100, m_imageHeight + 100);
    }
}

TEST_F(PS_Benchmarks, pixel_non_zeroes_avg_valid_input)
{
    for (uint32_t i = 0; i < m_iterrations; ++i)
    {
        m_pixelSum->getNonZeroAverage(100, 100, m_imageWidth - 100, m_imageHeight - 100);
    }
}

TEST_F(PS_Benchmarks, pixel_non_zeroes_avg_invalid_input)
{
    for (uint32_t i = 0; i < m_iterrations; ++i)
    {
        m_pixelSum->getNonZeroAverage(m_imageWidth - 100, 100, -100, m_imageHeight + 100);
    }
}

