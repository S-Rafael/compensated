/** encoding: UTF-8
 *
 * © Copyright 2021 Rafał M. Siejakowski <rs@rs-math.net>
 *
 * This software is licensed under the terms of the 3-Clause BSD License.
 * Please refer to the accompanying LICENSE file for the license terms.
 *
 *
 * See example/example.cpp for examples of usage.
=============================================================================================*/

#ifndef __COMPENSATED_H__
#define __COMPENSATED_H__

// Check if we have C++20 or later:
#ifndef __cplusplus
#error C++ is required to compile the header compensated.h
#else
    #if __cplusplus < 202002L
    #error Compiling compensated.h requires C++20 or newer
    #endif
#endif

// Try to set precise float behavior for MSVC:
#ifdef _MSC_BUILD
#pragma float_control(precise, on, push)
#endif

// This library does NOT work with "fast math" or "unsafe math optimisations".
#ifdef __FAST_MATH__
#error Error: compensated.h does not work with fast math/unsafe optimizations.
#endif

// We include only C++20 standard library headers:
#include <concepts>
#include <complex>


namespace compensated
{
/*
 * First, we formulate the concept of `kahanizable` raw value types
 * and introduce two special predicates:
 *
 * • being "real"    (a real number in the mathematical sense),
 * • being "complex" (a complex number in the mathematical sense).
 *
 * In order for a raw value type to be `kahanizable`, i.e., admissible
 * for Kahan summation, it must satisfy the following properties:
 *
 * 1) It must be assignable from the integer literal `0`.
 * 2) There must be an operator + and operator - defined for the raw type,
 *    both of which return a type convertible to the raw value type.
 *
 * Optionally, if the raw value type is "real" or "complex", then we
 * can use Neumaier's algorithm which improves on Kahan's.
 * The improved (Kahan-Neumaier) algorithm keeps the running compensation
 * small by cancelling values of more similar orders of magnitude.
 */

/**
 * @brief Whether the basic Kahan summation algorithm can be implemented for the type
 */
template<typename T>
concept kahanizable = requires(T a, T b)
{
    a = 0; // Assignable from integer literal `zero`
    {a + b} -> std::convertible_to<T>; // has a binary plus
    {a - b} -> std::convertible_to<T>; // has a binary minus
};

/**
 * @brief Whether a unary operator `-` exists for the type
 */
template<typename T>
concept has_unary_minus = requires(T a)
{
    {-a} -> std::convertible_to<T>;
};

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

/*
 * Formulate a predicate saying that a given type behaves
 * "like a complex number", i.e., that it has public member
 * functions real(), imag() returning `kahanizable` real types
 * and has a 2-argument constructor which accepts these types.
 */
template<typename T>
concept std_real = kahanizable<T>
                   && std::three_way_comparable<T>
                   && has_std_abs<T>;

/**
 * @brief Whether the type represents a complex number
 */
template<typename T>
concept is_complex = requires(T z)
{
    {z.real()} -> std_real;    // Has a real part
    {z.imag()} -> std_real;    // Has an imaginary part
    z = T(z.real(), z.imag()); // Can be reconstructed from those
};

/**
 * @brief The concept of an iterator to a container with
 * elements of raw value type in it.
 */
template<typename It, typename V>
concept is_iterator_to = requires (It i, It j)
{
    {*i}     -> std::convertible_to<V>;
    {i != j} -> std::convertible_to<bool>;
    {i = ++j}; // can be incremented
};
//=============================================================================================
/**
 * @mainclass
 * class `value` - template class representing a value
 * with compensated Kahan/Kahan-Neumaier addition.
 * @param
 * The template parameter is the underlying "raw" value type.
 */
template<kahanizable V>
class value
{
private:
    V Sum = 0;           // the sum
    V Compensation = 0;  // the running compensation

public:
    // Constructors from nothing and from V:
    constexpr value() = default;
    explicit constexpr value(const V& initial_value)
        : Sum{initial_value}
    {
        Compensation = 0;
    }
    /*
     * Copy/move constructors and assignment operators: all defaulted.
     * This class is default-constructible, trivially copiable and movable
     */
    ~value() = default;

private:
    // Constructor which manually sets the members. For internal use only.
    explicit constexpr value(V S, V C) : Sum{S}, Compensation{C} {};

public:
//=== Conversion operators ===

    /**
     * @brief Conversion operator to the raw value type
     */
    inline constexpr operator V() const {return Sum + Compensation;}

    /**
     * @brief Assignment operator from raw value type
     */
    inline void operator= (V value)
    {
        Sum = value;
        Compensation = 0;
    }

    /**
     * @brief Provides an estimate of the error resulting from conversion
     * to the raw value type
     */
    inline V error(void) const
    {
        V converted = V(*this);
        return (Sum - converted) + Compensation;
    }

    /**
     * @brief Extracts the real part of a complex value
     */
    inline constexpr auto real(void) const
    requires is_complex<V>
    {
        return Sum.real() + Compensation.real();
    }

    /**
     * @brief Extracts the imaginary part of a complex value
     */
    inline constexpr auto imag(void) const
    requires is_complex<V>
    {
        return Sum.imag() + Compensation.imag();
    }

//=== Equality comparison operators ===
// Note: the corresponding `!=` operators will be auto-generated
// by the C++20 rewriting mechanism.

    /**
     * @brief operator== tries to determine if two objects represent the
     * same mathematical value, even if represented differently
     * @param other - right-hand side of comparison
     * @return true on equality, false on inequality
     */
    inline constexpr bool operator== (const value<V>& other) const
    requires std::equality_comparable<V>
    {
        return (Sum - other.Sum == other.Compensation - Compensation);
    }

    /**
     * @brief operator== tries to compare the value represented by this
     * object with a raw value
     * @param value - raw value to compare with
     * @return true on equality, false on inequality
     */
    inline constexpr bool operator== (const V& value) const
    requires std::equality_comparable<V>
    {
        return (Compensation == value - Sum) || (Sum == value - Compensation);
    }

//=== Unary minus ===
    /**
     * @brief Unary minus - for raw value types possessing a unary minus
     */
    inline constexpr value<V> operator- (void) const
    requires has_unary_minus<V>
    {
        return value<V>(-Sum, -Compensation);
    }

    /**
     * @brief Unary minus - for raw value types without a unary minus
     */
    inline constexpr value<V> operator- (void) const
    requires (! has_unary_minus<V>)
    {
        V zero = 0;
        // We use subtraction from zero since there is no unary minus for V
        return value<V>(zero - Sum, zero - Compensation);
    }

// === Kahan-Neumaier summation operators (on the right) ===
// --- Real case ---
    /**
     * @brief Add an element of type V using the Kahan-Neumaier addition
     * (real case supported by std::abs)
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
                            Compensation + ((Sum - naive_sum) + increment));
        }
        else
        {
            /* In this case, the roles swap: the increment is larger than
             * the old sum, so we use the increment for the cancellation.
             */
            return value<V>(naive_sum,
                            Compensation + ((increment - naive_sum) + Sum));
        }
    }

    /**
     * @brief Add an element of type V using the Kahan-Neumaier addition
     * (real case with user-supplied abs() member)
     */
    inline value<V> operator+ (const V& increment) const
    requires is_real<V> && has_custom_abs<V> && (!has_std_abs<V>)
    { // See comments for the version with std::abs for explanation
        V naive_sum = Sum + increment;
        if (Sum.abs() > increment.abs())
            return value<V>(naive_sum,
                            Compensation + ((Sum - naive_sum) + increment));
        else
            return value<V>(naive_sum,
                            Compensation + ((increment - naive_sum) + Sum));
    }

    /**
     * @brief Add in-place an element of type V using the Kahan-Neumaier addition
     * (real case supported by std::abs)
     */
    inline void operator+= (const V& increment)
    requires is_real<V> && has_std_abs<V>
    {
        V naive_sum = Sum + increment;
        if (std::abs(Sum) > std::abs(increment)) // See comments in operator+
            Compensation = Compensation + ((Sum - naive_sum) + increment);
        else
            Compensation = Compensation + ((increment - naive_sum) + Sum);
        Sum = naive_sum;
    }

    /**
     * @brief Add in-place an element of type V using the Kahan-Neumaier addition
     * (real case with user-supplied abs() member)
     */
    inline void operator+= (const V& increment)
    requires is_real<V> && has_custom_abs<V> && (!has_std_abs<V>)
    {
        V naive_sum = Sum + increment;
        if (Sum.abs() > increment.abs()) // See comments in operator+
            Compensation = Compensation + ((Sum - naive_sum) + increment);
        else
            Compensation = Compensation + ((increment - naive_sum) + Sum);
        Sum = naive_sum;
    }

// --- Complex case
    /**
     * @brief Add an element of type V using the Kahan-Neumaier addition
     * (complex case)
     */
    inline value<V> operator+ (const V& increment) const
    requires is_complex<V>
    {
        V naive_sum = Sum + increment;
        auto inc_real = increment.real();
        auto inc_imag = increment.imag();

        // Real and imaginary parts of the update to Compensation:
        decltype(inc_real) comp_update_real, comp_update_imag;

        // Compute the update to the real part of the compensation
        if (std::abs(Sum.real()) > std::abs(inc_real))
            comp_update_real = (Sum.real() - naive_sum.real()) + inc_real;
        else
            comp_update_real = (inc_real - naive_sum.real()) + Sum.real();

        // Compute the update to the imaginary part of the compensation
        if (std::abs(Sum.imag()) > std::abs(inc_imag))
            comp_update_imag = (Sum.imag() - naive_sum.imag()) + inc_imag;
        else
            comp_update_imag = (inc_imag - naive_sum.imag()) + Sum.imag();

        return value<V>(naive_sum,
                        Compensation + V(comp_update_real, comp_update_imag));
    }

    /**
     * @brief Add in-place an element of type V using the Kahan-Neumaier addition
     * (complex case)
     */
    inline void operator+= (const V& increment)
    requires is_complex<V>
    {
        V naive_sum = Sum + increment;
        auto inc_real = increment.real();
        auto inc_imag = increment.imag();

        // Real and imaginary parts of the update to Compensation:
        decltype(inc_real) comp_update_real, comp_update_imag;

        // Compute the update to the real part of the compensation
        if (std::abs(Sum.real()) > std::abs(inc_real))
            comp_update_real = (Sum.real() - naive_sum.real()) + inc_real;
        else
            comp_update_real = (inc_real - naive_sum.real()) + Sum.real();

        // Compute the update to the imaginary part of the compensation
        if (std::abs(Sum.imag()) > std::abs(inc_imag))
            comp_update_imag = (Sum.imag() - naive_sum.imag()) + inc_imag;
        else
            comp_update_imag = (inc_imag - naive_sum.imag()) + Sum.imag();

        // Update in-place:
        Sum = naive_sum;
        Compensation = Compensation + V(comp_update_real, comp_update_imag);
    }

// --- The case of V neither real nor complex - plain Kahan algorithm
    /**
     * @brief Add an element of type V (neither real nor complex)
     * using plain Kahan summation
     */
    inline value<V> operator+ (const V& increment) const
    requires (!is_real<V>) && (!is_complex<V>)
    {   // plain Kahan
        V naive_sum = Sum + increment;
        return value<V>(naive_sum,
                        Compensation + ((Sum - naive_sum) + increment));
    }

    /**
     * @brief Add in-place an element of type V (neither real nor complex)
     * using plain Kahan summation
     */
    inline void operator+= (const V& increment)
    requires (!is_real<V> && !is_complex<V>)
    {   // plain Kahan
        V naive_sum = Sum + increment;
        Compensation = Compensation + ((Sum - naive_sum) + increment);
        Sum = naive_sum;
    }

// === Operators that are common to all cases
    /**
     * @brief Adds an element of the same type
     */
    inline value<V> operator+ (const value<V>& other) const
    {   // re-use previously defined operators:
        return operator+(other.Sum) + other.Compensation;
    }

    /**
     * @brief Adds in-place an element of the same type
     */
    inline void operator+= (const value<V>& other)
    {   // re-use previosly defined operators:
        operator+=(other.Sum);
        operator+=(other.Compensation);
    }

    /**
     * @brief Adds an entire collection of raw value types to the
     * present object. The collection is described by a pair of iterators
     * of templated iterator type `It`. We require the iterator type `It`
     * to behave like an iterator to V, i.e., to satisfy the concept
     * is_iterator_to<It, V>
     * @param first - the iterator to the beginning of the collection
     * @param last  - the iterator to "one-past" last element of collection
     */
    template<typename It>
    requires is_iterator_to<It, V>
    inline void accumulate(It first, It last)
    {
        for (auto iter = first; iter != last; ++iter)
            operator+=(*iter);
    }
// --- Variants of operator `-`
    /**
     * @brief Subtracts a raw value from the value object
     */
    inline value<V> operator- (const V& increment) const
    requires has_unary_minus<V>
    {
        return operator+(-increment);
    }

    /**
     * @brief Subtracts a raw value from the value object
     */
    inline value<V> operator- (const V& increment) const
    requires (!has_unary_minus<V>)
    {
        V zero = 0;
        return operator+(zero-increment);
    }

    /**
     * @brief Subtracts in-place a raw value from the value object
     */
    inline void operator-= (const V& increment)
    requires has_unary_minus<V>
    {
        operator+=(-increment);
    }

    /**
     * @brief Subtracts in-place a raw value from the value object
     */
    inline void operator-= (const V& increment)
    requires (!has_unary_minus<V>)
    {
        V zero = 0;
        operator+=(zero-increment);
    }

    /**
     * @brief Subtracts another value object from the current one
     */
    inline value<V> operator- (const value<V>& other) const
    {
        return operator+(-other);
    }

    /**
     * @brief Subtracts in-place another value object from the current one
     */
    inline void operator-= (const value<V>& other)
    {
        operator+=(-other);
    }
}; // class value

// ==== Left operators: V + value<V>, V - value<V>
/**
 * @brief Operator `+` for adding a raw value on the left
 */
template<kahanizable V>
inline value<V> operator+(V raw, value<V> kn)
{
    return kn + raw;
}

/**
 * @brief Operator `-` for subtracting from a raw value
 */
template<kahanizable V>
inline value<V> operator-(V raw, value<V> kn)
{
    return (-kn) + raw;
}
// ==== Left equality comparison operator: V == value<V>
/**
 * @brief Operator `==` with raw value on the left
 */
template<kahanizable V>
inline value<V> operator==(V raw, value<V> kn)
{
    return (kn == raw);
}
// Note: operator!= will be auto-generated through C++20 "rewriting"

} // namespace kn

#ifdef _MSC_BUILD
#pragma float_control(pop)
#endif

#endif // __COMPENSATED_H__

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4:fenc=utf-8 :