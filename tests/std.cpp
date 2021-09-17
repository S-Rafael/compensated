/** encoding: UTF-8
 *
 * © Copyright 2021 Rafał M. Siejakowski <rs@rs-math.net>
 *
 * This software is licensed under the terms of the 3-Clause BSD License.
 * Please refer to the accompanying LICENSE file for the license terms.
 *
 */

#include <vector>

#include "tests.h"
#include "lossy_values.h"
#include "../compensated.h"

/**
 * @file Tests for functionality which interfaces with libstdc++
 */
//============================================================================================


/**
 * @test Test the compensated::value::accumulate function
 */
TEST(compensated_test, accumulate)
{
    std::vector<double> v;
    v.push_back(1.0);
    v.push_back(2.0);
    v.push_back(3.0);
    v.push_back(4.0); // sum is 10.0
    compensated::value<double> test{0.0};
    test.accumulate(v.begin(), v.end());
    double result = test;
    EXPECT_DOUBLE_EQ(result, 10.0);
}

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4:fenc=utf-8 :