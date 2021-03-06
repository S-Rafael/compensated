/** encoding: UTF-8
 *
 * © Copyright 2021 Rafał M. Siejakowski <rs@rs-math.net>
 *
 * This software is licensed under the terms of the 3-Clause BSD License.
 * Please refer to the accompanying LICENSE file for the license terms.
 *
 */

/**
 * @file This file declares the class compensated_test which inherits
 * from testing::Test class of the GoogleTest suite.
 */

#ifndef __TESTS_H__
#define __TESTS_H__

#include "gtest/gtest.h"

/**
 * @class The compensated_test class inherits from Google testing
 * ::testing::Test class
 */
class compensated_test : public ::testing::Test
{
public:
    /**
     * @brief compensated_test class constructor
     */
    compensated_test() {::testing::Test::SetUpTestCase();}

    /**
     * @brief Destructor of class compensated_test
     */
    ~compensated_test() override {::testing::Test::TearDownTestCase();}
};


#endif // __TESTS_H__

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4:fenc=utf-8 :
