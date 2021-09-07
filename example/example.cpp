/** encoding: UTF-8
 *
 * © Copyright 2021 Rafał M. Siejakowski <rs@rs-math.net>
 *
 * This software is licensed under the terms of the 3-Clause BSD License.
 * Please refer to the accompanying LICENSE file for the license terms.
 *
 *
 * This is an example/demo program illustrating the usage of the knlite library.
=============================================================================================*/

#include <iostream> // For console input/output
#include <complex>  // For std::complex

#include "../knlite.hpp"

/**
 *  We declare a very simple custom class to demonstrate
 *  the generic nature of the knlite library.
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
	cout << endl << "Examples of usage of knlite:" << endl << endl;

// === Basic demonstration ===
	/* We try to perform naive calculations on doubles
	 * to see that we lose precision:
	 */
	double expected_zero = huge + tiny - huge - tiny;
	if (expected_zero != 0.0)
		cout << "Adding floating point numbers naively may lead to a loss of precision." << endl
			 << "For example, " << expected_zero << " != 0\n" << endl;

	// Now try the same with knlite
	kn::value<double> kn_test = huge;
	kn_test += tiny;
	kn_test -= huge;
	kn_test -= tiny;
	if (kn_test == 0.0)
		cout << "Using the class `kn::value<double>`, the same calculation results in" << endl
			 << kn_test << " == 0" << endl << endl;

// === Demonstration with complex numbers ===
	cdbl z {huge, tiny};
	cdbl w {tiny, huge};
	kn::value<cdbl> kn_complex = z;
	kn_complex += w;
	kn_complex -= z;
	kn_complex -= w;
	if (kn_complex == 0.0)
		cout << "Using the class `kn::value<std::complex<double>>`, we get:" << endl
			 << "[Real]: " << kn_complex.real() << " == 0" << endl
			 << "[Imag]: " << kn_complex.imag() << " == 0" << endl << endl;

// === Demonstration of overloaded operators on kn::value
	kn::value<cdbl> kn_z(z);
	kn::value<cdbl> kn_w(w);
	auto kn_result = kn_z + kn_w - kn_z - kn_w;

	if (kn_result == 0.0)
		cout << "The same happens when we use the overloaded operators of `kn::value`:" << endl
			 << "[Real]: " << kn_result.real() << " == 0" << endl
			 << "[Imag]: " << kn_result.imag() << " == 0" << endl << endl;

// === Demonstration of left operators and mixing raw values with kn::value
	kn_result = z + kn_w - kn_z - w;
	if (kn_result == 0.0)
		cout << "Left addition and mixing in of raw types results in:" << endl
			 << "[Real]: " << kn_result.real() << " == 0" << endl
			 << "[Imag]: " << kn_result.imag() << " == 0" << endl << endl;

// === Demonstration of usage with custom classes
	my_point tiny_point, huge_point;
	tiny_point.x = tiny_point.y = tiny_point.z = tiny;
	huge_point.x = huge_point.y = huge_point.z = huge;
	kn::value<my_point> kn_tiny(tiny_point);
	kn::value<my_point> kn_huge(huge_point);
	auto pt_result = my_point(kn_huge + kn_tiny - kn_huge - kn_tiny);
	cout << "Example with a custom class `my_point`:" << endl
		 << "pt_result.x == " << pt_result.x << " == 0" << endl
		 << "pt_result.y == " << pt_result.y << " == 0" << endl
		 << "pt_result.z == " << pt_result.z << " == 0" << endl << endl;
	return 0;
}

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=8:softtabstop=4:enc=utf-8 :
