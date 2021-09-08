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
#include <iostream>

#include "gtest/gtest.h"
#include "../knlite.hpp"

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
		size_t half_bitsize_of_float = sizeof(float);
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

int main(int argc, char** argv)
{
	::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=8:softtabstop=4:enc=utf-8 :