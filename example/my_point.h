/** encoding: UTF-8
 *
 * © Copyright 2021 Rafał M. Siejakowski <rs@rs-math.net>
 *
 * This software is licensed under the terms of the 3-Clause BSD License.
 * Please refer to the accompanying LICENSE file for the license terms.
 *
 */

#ifndef __MY_POINT_H__
#define __MY_POINT_H__

/**
 * @file
 * This header defines a simple custom class which demonstrates
 * the generic nature of the Compensated library.
 */

/**
 * @brief The my_point struct behaves like a 3D-vector
 */
struct my_point
{
    double x=0, y=0, z=0;
    my_point() = default;
    constexpr my_point(int n) {x=y=z=static_cast<double>(n);}
    constexpr my_point operator+(const my_point& other) const
    {
        my_point result;
        result.x = x + other.x;
        result.y = y + other.y;
        result.z = z + other.z;
        return result;
    }
    constexpr my_point operator-(const my_point& other) const
    {
        my_point result;
        result.x = x - other.x;
        result.y = y - other.y;
        result.z = z - other.z;
        return result;
    }
};

#endif // __MY_POINT_H__
