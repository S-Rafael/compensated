/** encoding: UTF-8
 *
 * © Copyright 2021 Rafał M. Siejakowski <rs@rs-math.net>
 *
 * This software is licensed under the terms of the 3-Clause BSD License.
 * Please refer to the accompanying LICENSE file for the license terms.
 *
 */

#include "tests.h"
#include "lossy_values.h"
#include "../compensated.h"

/**
 * @file Basic functionality tests
 */
//============================================================================================

/**
 * @test Check if the precision is lost with ordinary operations;
 * this ensures that other tests check if Compensated actually makes a
 * difference.
 */
TEST(compensated_test, lossy_values)
{
    // Try losing precision with doubles...
    double test1 = huge_dbl + tiny_dbl - huge_dbl - tiny_dbl;
    ASSERT_FALSE(test1 == 0.0);

    // ... and with floats
    float test2 = huge_fl + tiny_fl - huge_fl - tiny_fl;
    ASSERT_FALSE(test2 == 0.0);
}

/**
 * @test Check if the size of object is twice the size of raw value
 */
TEST(compensated_test, object_size)
{
    compensated::value<double> kd;
    compensated::value< std::complex<double> > kc;
    EXPECT_EQ(sizeof(kd), 2*sizeof(double));
    EXPECT_EQ(sizeof(kc), 2*sizeof(std::complex<double>));
}

/**
 * @test Check if the conversions to and from raw values work as expected
 */
TEST(compensated_test, conversions)
{
    // Test conversion from double to compensated::value and back
    double x = 1.0;
    compensated::value<double> kx{x};
    double dkx = (double) kx;
    EXPECT_DOUBLE_EQ(x, dkx);

    // Test conversion from compensated::value to double and back,
    // using operator==
    compensated::value<double> k{huge_dbl};
    double dk = (double) k;
    compensated::value<double> kdk{dk};
    EXPECT_EQ(k, kdk);

    // Test assignment and conversion operators
    k = 1.0;
    dk = k;
    EXPECT_DOUBLE_EQ(dk, 1.0);

    // Test conversions by extracting real and imaginary parts
    // of a complex number
    std::complex<double> z{1.0, 2.0};
    compensated::value< std::complex<double> > kz{z};
    double rekz = kz.real();
    double imkz = kz.imag();
    std::complex<double> reconstructed{rekz, imkz};
    EXPECT_EQ(reconstructed, z);
    EXPECT_DOUBLE_EQ(rekz, z.real());
    EXPECT_DOUBLE_EQ(imkz, z.imag());
}

/**
 * @test Check if the equality comparison operators work as expected
 */
TEST(compensated_test, equality_comparison)
{
    double x = 42.0;
    auto kx1 = compensated::value{x}; // template parameter inference
    auto kx2 = compensated::value<double>{x}; // no inference
    auto other = compensated::value<double>{huge_dbl};
    // Comparisons between two object of compensated::value class
    EXPECT_TRUE(kx1 == kx2);
    EXPECT_FALSE(kx1 == other);
    EXPECT_FALSE(kx2 == other);
    EXPECT_FALSE(huge_dbl == kx1);
    // Comparisons with raw value on the right
    EXPECT_TRUE(kx1 == x);
    EXPECT_TRUE(kx2 == x);
    // Comparisons with raw value on the left
    EXPECT_TRUE(x == kx1);
    EXPECT_TRUE(x == kx2);
    EXPECT_FALSE(x == other);
}

/**
 * @test Rewritten operator !=
 */
TEST(compensated_test, inequality_comparison)
{
    double x = 42.0;
    auto kx1 = compensated::value{x}; // template parameter inference
    auto kx2 = compensated::value<double>{x}; // no inference
    auto other = compensated::value<double>{huge_dbl};
    // Comparisons between two object of compensated::value class
    EXPECT_FALSE(kx1 != kx2);
    EXPECT_TRUE(kx1 != other);
    EXPECT_TRUE(kx2 != other);
    EXPECT_FALSE(huge_dbl == kx1);
    // Comparisons with raw value on the right
    EXPECT_FALSE(kx1 != x);
    EXPECT_FALSE(kx2 != x);
    // Comparisons with raw value on the left
    EXPECT_FALSE(x != kx1);
    EXPECT_TRUE(x != other);
}

/**
 * @test Check if the unary minus works
 */
TEST(compensated_test, unary_minus)
{
    // Test unary minus with complex raw vale
    std::complex<double> z{1.0, 2.0};
    auto k = compensated::value{z};
    auto neg = -k;
    auto sum = k + neg;
    EXPECT_DOUBLE_EQ(sum.real(), 0.0);
    EXPECT_DOUBLE_EQ(sum.imag(), 0.0);

    // Test unary minus with floats
    compensated::value<float> t{huge_fl};
    auto minus_t = -t;
    auto fsum = minus_t + t;
    EXPECT_EQ(fsum, 0.0);
}

/**
 * @test Test arithmetic operations on floats
 */
TEST(compensated_test, kahan_neumaier_float)
{
    // Test adding floats
    compensated::value<float> v{huge_fl};
    v += tiny_fl;
    v -= huge_fl;
    v -= tiny_fl;
    float result{v};
    EXPECT_FLOAT_EQ(result, 0.0f);

    // Test adding floats on the left
    v = huge_fl;
    v = tiny_fl + v;
    v = (-huge_fl) + v;
    v = (-tiny_fl) +v;
    result = v;
    EXPECT_FLOAT_EQ(result, 0.0f);
}

/**
 * @test Test arithmetic operations on doubles
 */
TEST(compensated_test, kahan_neumaier_double)
{
    // Test adding doubles
    compensated::value<double> v{huge_dbl};
    v += tiny_dbl;
    v = v - huge_dbl;
    v = v - tiny_dbl;
    double result{v};
    EXPECT_DOUBLE_EQ(result, 0.0f);

    // Test adding doubles on the left
    v = huge_dbl;
    v = tiny_dbl + v;
    v = (-huge_dbl) + v;
    v = (-tiny_dbl) + v;
    result = v;
    EXPECT_DOUBLE_EQ(result, 0.0f);
}

/**
 * @test Test arithmetic operations on complex numbers
 */
TEST(compensated_test, kahan_neumaier_complex)
{
    // Test Kahan-Neumaier algorithm for complex numbers
    std::complex<double> z {huge_dbl, tiny_dbl};
    std::complex<double> w {tiny_dbl, huge_dbl};
    compensated::value kz {z};
    compensated::value kw {w};
    auto result = kz + kw - kz - kw;
    EXPECT_DOUBLE_EQ(result.real(), 0.0);
    EXPECT_DOUBLE_EQ(result.imag(), 0.0);

    // Test operators on the right
    compensated::value b {z};
    std::complex<double> sum = ((b + w) - z) - w;
    EXPECT_DOUBLE_EQ(sum.real(), 0.0);
    EXPECT_DOUBLE_EQ(sum.imag(), 0.0);

    // Test operators on the right
    b = z;
    sum = w + (-z + (-w + b));
    EXPECT_DOUBLE_EQ(sum.real(), 0.0);
    EXPECT_DOUBLE_EQ(sum.imag(), 0.0);

    // Test in-place addition
    b = z;
    b += w;
    b -= z;
    b -= w;
    sum = b;
    EXPECT_DOUBLE_EQ(sum.real(), 0.0);
    EXPECT_DOUBLE_EQ(sum.imag(), 0.0);
}

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4:fenc=utf-8 :
