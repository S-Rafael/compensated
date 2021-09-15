/** encoding: UTF-8
 *
 * © Copyright 2021 Rafał M. Siejakowski <rs@rs-math.net>
 *
 * This software is licensed under the terms of the 3-Clause BSD License.
 * Please refer to the accompanying LICENSE file for the license terms.
 *
 */
#ifndef __HUGETINY_H__
#define __HUGETINY_H__

#include <concepts>

/**
 * @file This file declares a utility function template which can
 * produce floats or doubles that are sufficiently huge or tiny so
 * that adding them demonstrates the lossy nature of uncompensated
 * floating-point addition.
 */

/**
 * @brief The value_type enum allows us to choose the type of
 * value we want to generate
 */
enum class value_type {huge, tiny};

/**
 * @brief get_lossy() returns a floating-point value that demonstrates
 * the lossy nature of floating-point addition
 */
template<typename T>
requires std::is_floating_point_v<T>
consteval T get_lossy(value_type v)
{
    auto half_bitsize = 4 * sizeof(T);
    T result = 1.0;
    T multiplier = (v == value_type::huge)? 2.0 : 0.5;
    for (unsigned p=0; p < half_bitsize; p++)
        result *= multiplier;
    return result;
}

/* Declare the constants that demonstrate the need for
 * compensated addition:
 */
const double huge_dbl = get_lossy<double>(value_type::huge);
const double tiny_dbl = get_lossy<double>(value_type::tiny);

const float huge_fl = get_lossy<float>(value_type::huge);
const float tiny_fl = get_lossy<float>(value_type::tiny);

#endif // __HUGETINY_H__
// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4:fenc=utf-8 :