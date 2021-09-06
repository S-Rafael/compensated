/* encoding: UTF-8
 *
 * © Copyright 2021 Rafał M. Siejakowski <rs@rs-math.net>
 *
 * This software is licensed under the terms of the 3-Clause BSD License.
 * Please refer to the accompanying LICENSE file for the license terms.
 *
 *
 * See example/example.cpp for examples of usage.
=============================================================================================*/

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
 * First, we formulate the concept of "admissible" raw value types and two
 * special predicates:
 *
 * • being "real"    (a real number in the mathematical sense),
 * • being "complex" (a complex number in the mathematical sense).
 *
 * In order for a raw value type to be admissible for Kahan summation,
 * it must satisfy the following properties:
 *
 * 1) It must be assignable from the literal `0`.
 * 2) There must be an operator + and operator - defined for the raw type,
 *    both of which return a type convertible to the raw value type in question.
 *
 * Optionally, if the raw value type is "real" or "complex", then we
 * can use Neumaier's algorithm which improves on Kahan's.
 * The improved (Kahan-Neumaier) algorithm keeps the running compensation small.
 */

/**
 * @brief Whether the asignment from the literal `0` is syntactically correct
 */
template<typename T>
concept assignable_from_zero_literal = requires(T a) {a = 0;};

/**
 * @brief Whether a binary operator `+` exists for the type
 */
template<typename T>
concept has_binary_plus  = requires(T a, T b, T result) {result = a + b;};

/**
 * @brief Whether a binary operator `-` exists for the type
 */
template<typename T>
concept has_binary_minus = requires(T a, T b, T result) {result = a - b;};

/**
 * @brief Whether a unary operator `-` exists for the type
 */
template<typename T>
concept has_unary_minus = requires(T a, T result) {result = -a;};

/**
 * @brief Whether the basic Kahan summation algorithm can be implemented for the type
 */
template<typename T>
concept is_admissible = assignable_from_zero_literal<T>
						&& has_binary_plus<T>
						&& has_binary_minus<T>;

/**
 * @brief Whether the type has an overload of std::abs
 */
template<typename T>
concept has_std_abs = requires(T a)
{
	{std::abs(a)} -> std::three_way_comparable;
};

/**
 * @brief Whether the type has a reasonable public member function abs()
 */
template<typename T>
concept has_custom_abs = requires(T a)
{
	{a.abs()} -> std::three_way_comparable;
};

/**
 * @brief Whether the type represents a real number
 */
template<typename T>
concept is_real = std::three_way_comparable<T>
				  && (has_std_abs<T> || has_custom_abs<T>);

/* Formulate predicates that determine whether a type behaves
 * "like a complex number", i.e., it has public member functions
 * real(), imag() returning admissible real types and has a
 * two-argument constructor which accepts these types.
 */
template<typename T>
concept admissible_real = is_admissible<T> && is_real<T>;

template<typename T>
concept has_real_part = requires(T z)
{
	{z.real()} -> admissible_real;
};

template<typename T>
concept has_imag_part = requires(T z)
{
	{z.imag()} -> admissible_real;
};

template<typename T>
concept has_reconstructor = requires(T z) {z = T(z.real(), z.imag());};

/**
 * @brief Whether the type represents a complex number
 */
template<typename T>
concept is_complex = has_real_part<T>
					 && has_imag_part<T>
					 && has_reconstructor<T>;
//=============================================================================================
/**
 * @mainclass
 * class `value` - template class for elements with Kahan-Neumaier addition.
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
	// Copy/move constructors and assignment operators – defaulted.
	~value() = default;
private:
	// constructor which manually sets the members: for internal use only.
	value(V S, V C) : Sum{S}, Compensation{C} {};
public:
//=== Conversion operators ===
	inline operator V() const {return Sum + Compensation;}
//=== Equality comparison operators ===
	/**
	 * @brief operator== tries to determine if two objects represent the
	 * same value, even if represented differently
	 * @param other - right-hand side of comparison
	 * @return true on equality, false on inequality
	 */
	inline bool operator== (const value<V>& other) const
	{
		return (Sum - other.Sum == other.Compensation - Compensation);
	}
	inline bool operator!= (const value<V>& other) const
	{
		return !(operator==(other));
	}
	/**
	 * @brief operator== tries to compare the value represented by this
	 * object with a raw value
	 * @param value - right-hand side of comparison
	 * @return true on equality, false on inequality
	 */
	inline bool operator== (const V& value) const
	{
		return (Compensation == value - Sum) || (Sum == value - Compensation);
	}
	inline bool operator!= (const V& value) const
	{
		return !(operator==(value));
	}
//=== Unary minus ===
	/**
	 * @brief Unary minus - for raw value types possessing a unary minus
	 */
	inline value<V> operator- (void) const
	requires has_unary_minus<V>
	{
		return value<V>(-Sum, -Compensation);
	}
	/**
	 * @brief Unary minus - for raw value types without a unary minus
	 */
	inline value<V> operator- (void) const
	requires (! has_unary_minus<V>)
	{
		// We use subtraction from zero since there is no unary minus for V
		return value<V>(zero - Sum, zero - Compensation);
	}
// === Kahan summation operators (on the right) ===
// --- Real case ---
	/**
	 * @brief Add an element of type V (real case supported by std::abs)
	 */
	inline value<V> operator+ (const V& increment) const
	requires is_real<V> && has_std_abs<V>
	{
		V naive_sum = Sum + increment;
		if (std::abs(Sum) > std::abs(increment))
		{
			/* In this case, we have a large sum to which a small increment
			 * is added. Therefore, the compensation is computed by cancelling
			 * the large sum with the naive sum.
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
	/**
	 * @brief Add an element of type V (real case with user-supplied abs() member)
	 */
	inline value<V> operator+ (const V& increment) const
	requires is_real<V> && has_custom_abs<V> && (!has_std_abs<V>)
	{ // See comments for the version with std::abs for explanation
		V naive_sum = Sum + increment;
		if (Sum.abs() > increment.abs())
			return value<V>(naive_sum,
							Compensation + (Sum - naive_sum) + increment);
		else
			return value<V>(naive_sum,
							Compensation + (increment - naive_sum) + Sum);
	}
	/**
	 * @brief Increment in-place by an element of type V (real case supported by std::abs)
	 */
	inline void operator+= (const V& increment)
	requires is_real<V> && has_std_abs<V>
	{
		V naive_sum = Sum + increment;
		if (std::abs(Sum) > std::abs(increment)) // See comments in operator+
			Compensation = Compensation + (Sum - naive_sum) + increment;
		else
			Compensation = Compensation + (increment - naive_sum) + Sum;
		Sum = naive_sum;
	}
	/**
	 * @brief Increment in-place by an element of type V (real case with user-supplied
	 * abs() member)
	 */
	inline void operator+= (const V& increment)
	requires is_real<V> && has_custom_abs<V> && (!has_std_abs<V>)
	{
		V naive_sum = Sum + increment;
		if (Sum.abs() > increment.abs()) // See comments in operator+
			Compensation = Compensation + (Sum - naive_sum) + increment;
		else
			Compensation = Compensation + (increment - naive_sum) + Sum;
		Sum = naive_sum;
	}
// --- Complex case

// --- The case of neither real nor complex V


// --- Operators that are common to all cases
	/**
	 * @brief Adds an element of the same type
	 */
	inline value<V> operator+ (const value<V>& other) const
	{   // re-use previously defined operators:
		value<V> result = operator+(other.Sum);
		result += other.Compensation;
		return result;
	}
	/**
	 * @brief Adds in-place an element of the same type
	 */
	inline void operator+= (const value<V>& other)
	{   // re-use previosly defined operators:
		operator+=(other.Sum);
		operator+=(other.Compensation);
	}
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
}; // class value

// == Left operators: V + value<V>, V - value<V> ==
/**
 * @brief Operator `+` for adding a raw value on the left
 */
template<typename V>
requires is_admissible<V>
value<V> operator+(V raw, value<V> kn)
{
	return kn + raw; // Addition is commutative
}

/**
 * @brief Operator `-` for subtracting from a raw value
 */
template<typename V>
requires is_admissible<V>
value<V> operator-(V raw, value<V> kn)
{
	return (-kn) + raw;
}

} // namespace kn
#pragma float_control(pop)

#endif // _KNLITE_HPP

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=8:softtabstop=4:enc=utf-8 :
