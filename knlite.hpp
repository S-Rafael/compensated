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

// Try to set precise float behavior and push the setting to the stack:
#pragma float_control(precise, on, push)
// This library does NOT work with "fast math" or "unsafe math optimisations".
#ifdef __FAST_MATH__
#error Error: the knlite library (knlite.hpp) doesn't work with fast math/unsafe optimizations.
#endif

// We include only C++20 standard library headers:
#include <concepts>
#include <complex>

namespace kn
{

/*
 * Formulate the concept of admissible value types and two special predicates:
 * • being "real" (a real number in the mathematical sense),
 * • being "complex" (a complex number in the mathematical sense).
 *
 * In order for a value type to be admissible for Kahan-Neumaier summation,
 * it must satisfy the following properties:
 *
 * 1) It must be assignable from the literal `0`.
 * 2) There must be an operator + and operator - defined for the type,
 *    both of which return a type convertible to the value type in question.
 *
 * Optional requirements:
 *
 * • Being "real" or "complex" allows us to keep the running compensations small.
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

template<typename T>
concept is_complex = std::is_same_v<std::complex<float>, T>
					 || std::is_same_v<std::complex<double>, T>
					 || std::is_same_v<std::complex<long double>, T>;

template<typename T>
concept has_abs = requires(T a) {std::abs(a) > 0.0f;};

template<typename T>
concept is_real = std::three_way_comparable<T> && has_abs<T>;

/*
 * Declaration of the class `value` which incorporates the
 * Kahan-Neumaier algorithm via its overloaded operators.
 */

template<typename V>
requires is_admissible<V>
class value
{
private:
	const V zero = 0; 	    // This is a constant, so a static member.
	// There are only two private members that actually live in the object:
	V Sum = zero;           // the sum
	V Compensation = zero;  // the running compensation
public:
	// Constructors from nothing and from V:
	value() = default;
	value(V  initial_value) : Sum{initial_value} {Compensation = zero;}
	value(V& initial_value) : Sum{initial_value} {Compensation = zero;}
	// Copy, move constructors and assignment operators – defaulted.
	~value() = default;
private: // constructor which manually sets the members: for internal use only.
	value(V S, V C) : Sum{S}, Compensation{C} {};
public:
	// Conversion to type V:
	inline operator V() const {return Sum + Compensation;}

	// Equality comparison operators:
	inline bool operator== (const value<V>& other) const
	{
		return (Sum + Compensation == other.Sum + other.Compensation);
	}
	inline bool operator!= (const value<V>& other) const
	{
		return !(operator==(other));
	}
	inline bool operator== (const V& value) const
	{
		return (Sum + Compensation == value);
	}
	inline bool operator!= (const V& value) const
	{
		return !(operator==(value));
	}
	// Unary minus:
	inline value<V> operator- (void) const
	{
		/* This is a weird construction but we aren't assuming
		 * that the type V has a unary minus. If it does, then the
		 * compiler should figure this out (hopefully).
		 */
		return value<V>(zero - Sum, zero - Compensation);
	}

// ===== Kahan-Neumaier summation operators
// --- Real case
	inline value<V> operator+ (const V& increment) const
	requires is_real<V>
	{
		V naive_sum = Sum + increment;
		if (std::abs(Sum) > std::abs(increment))
		{
			/* In this case, we have a large sum to which a small increment
			 * is added. Therefore, the compensation is computed by cancelling
			 * the large sum with the naive addition of the increment.
			 */
			return value<V>(naive_sum,
							Compensation + (Sum - naive_sum) + increment);
		}
		else
		{
			/* In this case, the roles swap: the increment is larger than
			 * the old sum, so we use the increment for the cancellation.
			 */
			return value<V>(naive_sum,
							Compensation + (increment - naive_sum) + Sum);
		}
	}

	inline void operator+= (const V& increment)
	requires is_real<V>
	{
		V naive_sum = Sum + increment;
		if (std::abs(Sum) > std::abs(increment)) // See comments in operator+
			Compensation = Compensation + (Sum - naive_sum) + increment;
		else
			Compensation = Compensation + (increment - naive_sum) + Sum;
		Sum = naive_sum;
	}

	inline value<V> operator+ (const value<V>& other) const
	requires is_real<V>
	{   // re-use previously defined operators:
		value<V> result = operator+(other.Sum);
		result += other.Compensation;
		return result;
	}

	inline void operator+= (const value<V>& other)
	requires is_real<V>
	{   // re-use previosly defined operators:
		operator+=(other.Sum);
		operator+=(other.Compensation);
	}


// --- Complex case
// --- The case of neither real nor complex V
// ------ Operator minus is defined in terms of the above and unary minus:
inline value<V> operator- (const V& increment) const
{
	return operator+(zero-increment);
}
inline void operator-= (const V& increment)
{
	operator+=(zero-increment);
}
inline value<V> operator- (const value<V>& other) const
{
	return operator+(-other);
}
inline void operator-= (const value<V>& other)
{
	operator+=(-other);
}
};



// is_complex
// is_real
// not complex and not real




} // namespace kn
#pragma float_control(pop)

#endif // _KNLITE_HPP

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=8:softtabstop=4 :
