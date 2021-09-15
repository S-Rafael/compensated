/** encoding: UTF-8
 *
 * © Copyright 2021 Rafał M. Siejakowski <rs@rs-math.net>
 *
 * This software is licensed under the terms of the 3-Clause BSD License.
 * Please refer to the accompanying LICENSE file for the license terms.
 *
 */
#ifndef _TEST_CLASSES_H
#define _TEST_CLASSES_H

#include <complex> // For std::abs()

/**
 * @brief The real_with_custom_abs class provides an example
 * of a simple custom type which satisfies the constraints.
 */
class real_with_custom_abs
{
    double x;
public:
    real_with_custom_abs(double v) : x{v} {};
    inline double abs() const {return std::abs(x);};
    inline real_with_custom_abs operator+ (const real_with_custom_abs& other) const
    {
        return real_with_custom_abs(x + other.x);
    }
    inline real_with_custom_abs operator- (const real_with_custom_abs& other) const
    {
        return real_with_custom_abs(x - other.x);
    }
    inline bool is_zero() const {return x == 0.0;}
};

/**
 * @brief The custom_complex class provides a test case for a user-supplied
 * raw value type with member functions real() and imag()
 */
class custom_complex
{
    float x, y;
public:
    custom_complex(float real, float imag) : x{real}, y{imag} {};
    custom_complex(int n) : y{0} { x=static_cast<float>(n); };
    inline float real() const {return x;}
    inline float imag() const {return y;}
    inline custom_complex operator+ (const custom_complex& other) const
    {
        return custom_complex(x + other.x, y + other.y);
    }
    inline custom_complex operator- (const custom_complex& other) const
    {
        return custom_complex(x - other.x, y - other.y);
    }
};

/**
 * @brief The custom_gadget class provides a generic type, neither real nor
 * complex, which can be operated on via the Kahan algorithm
 */
class custom_gadget
{
    float x, y, z;
public:
    custom_gadget(float nx, float ny, float nz) : x{nx}, y{ny}, z{nz} {};
    custom_gadget(int n) : x{static_cast<float>(n)},
                           y{static_cast<float>(n)},
                           z{static_cast<float>(n)} {};
    inline custom_gadget operator+ (const custom_gadget& other) const
    {
        return custom_gadget(x + other.x, y + other.y, z + other.z);
    }
    inline custom_gadget operator- (const custom_gadget& other) const
    {
        return custom_gadget(x - other.x, y - other.y, z - other.z);
    }
    bool operator== (const custom_gadget& other) const
    {
        return (x == other.x) && (y == other.y) && (z == other.z);
    }
};

#endif // _TEST_CLASSES_H
// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4:fenc=utf-8 :
