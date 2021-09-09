/** encoding: UTF-8
 *
 * © Copyright 2021 Rafał M. Siejakowski <rs@rs-math.net>
 *
 * This software is licensed under the terms of the 3-Clause BSD License.
 * Please refer to the accompanying LICENSE file for the license terms.
 *
==========================================================

    Tests for knlite

*/

#include <complex>
#include "gtest/gtest.h"
#include "../knlite.hpp"

#include "test_classes.h"

/**
 * @class The knlite_test class inherits from Google testing
 * ::testing::Test class
 */
class knlite_test : public ::testing::Test
{
public:
	double huge_dbl, tiny_dbl; // Store test doubles
	float  huge_fl, tiny_fl;   // Store test floats

	/**
	 * @brief knlite_test class constructor
	 */
	knlite_test()
	{
		// Construct the huge and tiny double:
		size_t half_bitsize_of_double = 4 * sizeof(double);
		huge_dbl = tiny_dbl = 1.0;
		for (unsigned p=0; p < half_bitsize_of_double; p++)
		{
			huge_dbl *= 2.0; // make huge_dbl at least 2^(mantissa)
			tiny_dbl *= 0.5; // mage tiny_dbl at most 2^(-mantissa)
		}

		// Construct the huge and tiny float
		size_t half_bitsize_of_float = 4 * sizeof(float);
		huge_fl = tiny_fl = 1.0;
		for (unsigned p=0; p < half_bitsize_of_float; p++)
		{
			huge_fl *= 2.0; // make huge_fl at least 2^(mantissa)
			tiny_fl *= 0.5; // mage tiny_fl at most 2^(-mantissa)
		}
		::testing::Test::SetUpTestCase();
	}

	/**
	 * @brief Destructor of class knlite_test
	 */
	~knlite_test() override
	{
		::testing::Test::TearDownTestCase();
	}
};

//============================================================================================

/**
 * @test Check if the precision is lost with ordinary operations
 */
TEST_F(knlite_test, testcase_sanity)
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
TEST_F(knlite_test, object_size)
{
	kn::value<double> kd;
	kn::value< std::complex<double> > kc;
	EXPECT_EQ(sizeof(kd), 2*sizeof(double));
	EXPECT_EQ(sizeof(kc), 2*sizeof(std::complex<double>));
}

/**
 * @test Check if the conversions to and from raw values work as expected
 */
TEST_F(knlite_test, conversions)
{
	// Test conversion from double to kn::value and back
	double x = 1.0;
	kn::value<double> kx{x};
	double dkx = (double) kx;
	EXPECT_DOUBLE_EQ(x, dkx);

	// Test conversion from kn::value to double and back,
	// using operator==
	kn::value<double> k{huge_dbl};
	double dk = (double) k;
	kn::value<double> kdk{dk};
	EXPECT_EQ(k, kdk);

	// Test assignment and conversion operators
	k = 1.0;
	dk = k;
	EXPECT_DOUBLE_EQ(dk, 1.0);

	// Test conversions by extracting real and imaginary parts
	// of a complex number
	std::complex<double> z{1.0, 2.0};
	kn::value< std::complex<double> > kz{z};
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
TEST_F(knlite_test, equality_comparison)
{
	double x = 42.0;
	auto kx1 = kn::value{x}; // template parameter inference
	auto kx2 = kn::value<double>{x}; // no inference
	auto other = kn::value<double>{huge_dbl};
	EXPECT_TRUE(kx1 == kx2);
	EXPECT_FALSE(kx1 != kx2);
	EXPECT_FALSE(kx1 == other);
	EXPECT_FALSE(kx2 == other);
	EXPECT_TRUE(kx1 != other);
	EXPECT_TRUE(kx2 != other);
	EXPECT_TRUE(kx1 == x);
	EXPECT_FALSE(kx1 != x);
	EXPECT_TRUE(kx2 == x);
	EXPECT_FALSE(kx2 != x);
	EXPECT_TRUE(x == kx1);
	EXPECT_TRUE(x == kx2);
	EXPECT_FALSE(huge_dbl == kx1);
}

/**
 * @test Check if the unary minus works
 */
TEST_F(knlite_test, unary_minus)
{
	// Test unary minus with complex raw vale
	std::complex<double> z{1.0, 2.0};
	auto k = kn::value{z};
	auto neg = -k;
	auto sum = k + neg;
	EXPECT_DOUBLE_EQ(sum.real(), 0.0);
	EXPECT_DOUBLE_EQ(sum.imag(), 0.0);

	// Test unary minus with floats
	kn::value<float> t{huge_fl};
	auto minus_t = -t;
	auto fsum = minus_t + t;
	EXPECT_EQ(fsum, 0.0);
}

/**
 * @test Test arithmetic operations on floats
 */
TEST_F(knlite_test, kahan_neumaier_float)
{
	// Test adding floats
	kn::value<float> v{huge_fl};
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
TEST_F(knlite_test, kahan_neumaier_double)
{
	// Test adding doubles
	kn::value<double> v{huge_dbl};
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
TEST_F(knlite_test, kahan_neumaier_complex)
{
	// Test Kahan-Neumaier algorithm for complex numbers
	std::complex<double> z {huge_dbl, tiny_dbl};
	std::complex<double> w {tiny_dbl, huge_dbl};
	kn::value kz {z};
	kn::value kw {w};
	auto result = kz + kw - kz - kw;
	EXPECT_DOUBLE_EQ(result.real(), 0.0);
	EXPECT_DOUBLE_EQ(result.imag(), 0.0);

	// Test operators on the right
	kn::value b {z};
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

/**
 * @test Test the Kahan-Neumaier summation with a custom value
 * type representing a real number, with user-supplied abs() member
 */
TEST_F(knlite_test, custom_real_type)
{
	real_with_custom_abs x = 0;
	real_with_custom_abs h = huge_dbl;
	real_with_custom_abs t = tiny_dbl;
	kn::value<real_with_custom_abs> kx {x};

	// Test right operations
	kx = kx + h;
	kx = kx + t;
	kx = kx - h;
	kx = kx - t;
	real_with_custom_abs result = kx;
	EXPECT_TRUE(result.is_zero());

	// Test mixed operations
	kx = x;
	kx = h + kx;
	kx = t + kx;
	kx = kx - h;
	kx = kx - t;
	result = kx;
	EXPECT_TRUE(result.is_zero());
}

/**
 * @test Test the Kahan-Neumaier summation with a custom complex-like type
 */
TEST_F(knlite_test, custom_complex_type)
{
	custom_complex z(huge_fl, tiny_fl);
	custom_complex w(tiny_fl, huge_fl);
	kn::value test{z};
	test = w + test;
	test = test - z;
	test -= w;
	EXPECT_FLOAT_EQ(test.real(), 0.0);
	EXPECT_FLOAT_EQ(test.imag(), 0.0);
}

/**
 * @test Test the pure Kahan summation with a custom general type
 */
TEST_F(knlite_test, custom_general_type)
{
	// Make huge that we lose precision adding tiny_fl to huge_fl
	EXPECT_FLOAT_EQ(huge_fl + tiny_fl, huge_fl);

	// Check that we do NOT lose precision with kn::value
	custom_gadget all_huge(huge_fl, huge_fl, huge_fl);
	custom_gadget all_tiny(tiny_fl, tiny_fl, tiny_fl);

	kn::value test{all_huge};
	kn::value<custom_gadget> larger = test + all_tiny;
	EXPECT_FALSE(test == larger);
}

/**
 * @brief main function of the test suite
 */
int main(int argc, char** argv)
{
	::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=8:softtabstop=4:fenc=utf-8 :
