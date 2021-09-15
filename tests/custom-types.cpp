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
#include "../knlite.h"
#include "custom-types.h"

/**
 * @file Tests of knlite with user-defined raw value types
 */
//============================================================================================

/**
 * @test Test the Kahan-Neumaier summation with a custom value
 * type representing a real number, with a user-supplied abs() member
 */
TEST(knlite_test, custom_real_type)
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
TEST(knlite_test, custom_complex_type)
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
TEST(knlite_test, custom_general_type)
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

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4:fenc=utf-8 :