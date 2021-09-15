/** encoding: UTF-8
 *
 * © Copyright 2021 Rafał M. Siejakowski <rs@rs-math.net>
 *
 * This software is licensed under the terms of the 3-Clause BSD License.
 * Please refer to the accompanying LICENSE file for the license terms.
 *
 */

/** 
 * @file
 * This is an example/demo program illustrating the basic usage 
 * of the Compensated library.
 */

#include <iostream> // For console input/output
#include <complex>  // For std::complex

#include "compensated.h" // Include the Compensated library
#include "my_point.h"    // A simple custom class

/**
 * @brief main function of the example program
 * @return always returns 0
 */
int main(/* We don't need command line parameters */)
{
    using namespace std;
    using cdbl = std::complex<double>;

    // We define a huge number and a tiny number
    const double huge = 1.0e30;
    const double tiny = 1.0e-30;
    cout << endl << "Examples of usage of Compensated:" << endl << endl;

// === Basic demonstration ===
    /* We try to perform naive calculations on doubles
     * to see that we lose precision:
     */
    double expected_zero = huge + tiny - huge - tiny;
    if (expected_zero != 0.0)
        cout << "Adding floating point numbers naively may lead to a loss of precision." << endl
             << "For example, " << expected_zero << " != 0\n" << endl;

    // Now try the same with Compensated
    compensated::value<double> comp_test{huge};
    comp_test += tiny;
    comp_test -= huge;
    comp_test -= tiny;
    if (comp_test == 0.0)
        cout << "Using the class `compensated::value<double>`, the same calculation results in" << endl
             << comp_test << " == 0" << endl << endl;

// === Demonstration with complex numbers ===
    cdbl z {huge, tiny};
    cdbl w {tiny, huge};
    compensated::value<cdbl> comp_complex{z};
    comp_complex += w;
    comp_complex -= z;
    comp_complex -= w;
    if (comp_complex == 0.0)
        cout << "Using the class `compensated::value<std::complex<double>>`, we get:" << endl
             << "[Real]: " << comp_complex.real() << " == 0" << endl
             << "[Imag]: " << comp_complex.imag() << " == 0" << endl << endl;

// === Demonstration of overloaded operators on compensated::value
    compensated::value<cdbl> comp_z(z);
    compensated::value<cdbl> comp_w(w);
    auto comp_result = comp_z + comp_w - comp_z - comp_w;

    if (comp_result == 0.0)
        cout << "The same happens when we use the overloaded operators of `compensated::value`:" << endl
             << "[Real]: " << comp_result.real() << " == 0" << endl
             << "[Imag]: " << comp_result.imag() << " == 0" << endl << endl;

// === Demonstration of left operators and mixing raw values with compensated::value
    comp_result = z + comp_w - comp_z - w;
    if (comp_result == 0.0)
        cout << "Left addition and mixing in of raw types results in:" << endl
             << "[Real]: " << comp_result.real() << " == 0" << endl
             << "[Imag]: " << comp_result.imag() << " == 0" << endl << endl;

// === Demonstration of usage with custom classes
    my_point tiny_point, huge_point;
    tiny_point.x = tiny_point.y = tiny_point.z = tiny;
    huge_point.x = huge_point.y = huge_point.z = huge;
    compensated::value<my_point> comp_tiny(tiny_point);
    compensated::value<my_point> comp_huge(huge_point);
    auto pt_result = my_point(comp_huge + comp_tiny - comp_huge - comp_tiny);
    cout << "Example with a custom class `my_point`:" << endl
         << "pt_result.x == " << pt_result.x << " == 0" << endl
         << "pt_result.y == " << pt_result.y << " == 0" << endl
         << "pt_result.z == " << pt_result.z << " == 0" << endl << endl;
    return 0;
}

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4:fenc=utf-8 :
