/*
 * Copyright 2021 Rafał M. Siejakowski <rs@rs-math.net>
 *
 * This software is licensed under the terms of the 3-Clause BSD License.
 * Please refer to the accompanying LICENSE file for the license terms.
 *
 *
 * Please refer to example/example.cpp for examples of usage.
//=============================================================================*/

#ifndef _KNLITE_HPP
#define _KNLITE_HPP

// This library does NOT work with "fast math" or "unsafe math optimisations".
#ifdef __FAST_MATH__
#error Error: the knlite library (knlite.hpp) doesn't work with fast math/unsafe optimizations.
#endif

// Include only C++20 standard library headers:
#include <concepts>



namespace kn
{

/*
 * Formulate the concept of admissible value type.
 *
 * In order for a value type to be admissible for Kahan-Neumaier summation,
 * it must satisfy the following properties:
 *
 * 1) It must be assignable from the float literal `0.0f` or the integer literal `0`.
 * 2) There must be an operator + and operator - defined for the type,
 *    both of which return a type convertible to the value type in question.
 *
 * Optional requirements:
 *
 * • Three-way comparability allows keeping the running compensations small.
 *
 */
template<typename T>
concept assignable_from_zero_literal = requires(T a) {a = 0;};

template<typename T>
concept has_binary_plus  = requires(T a, T b, T result) {result = a + b;};

template<typename T>
concept has_binary_minus = requires(T a, T b, T result) {result = a - b;};

template<typename T>
concept is_admissible = assignable_from_zero_literal<T>
						&& has_binary_plus<T>
						&& has_binary_minus<T>;

// std::three_way_comparable<T>





} // namespace kn

#endif // _KNLITE_HPP

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=8:softtabstop=4 :
