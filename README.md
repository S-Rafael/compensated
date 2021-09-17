# The *Compensated* Library

*Compensated* is a tiny library providing a generic wrapper around additive
arithmetic types (typically, “floating point” values). The addition and
subtraction operations on the wrapped values use the Kahan and Kahan–Neumaier
running compensation algorithms, providing the best possible numerical
precision, regardless of how many values are summed.

## Main features

*  A generic header-only C++20 template library
*  Support for compensated addition and subtraction both with standard types
   (e.g., `float`, `double`, `std::complex<double>`, …), as well as custom types
*  Easy to use, see the attached documentation and example program
*  No external compile-time or link-time dependencies (other than the C++20
   standard library)
*  Permissive license: the 
   [3-Clause BSD License](https://opensource.org/licenses/BSD-3-Clause)

## Quick preview

Here is a simple demo program illustrating how *Compensated* works:

```cpp
#include <iostream> // For console input/output
#include <complex>  // For std::complex
#include "compensated/compensated.h" // Compensated

int main()
{
    std::complex<double> z(1e+30,  1e-30);
    std::complex<double> w(-1e-30, 1e+30);

    auto vanilla = z + w - z - w; // Mathematically zero
    std::cout << "Ordinary operations:    " 
              << vanilla.real() << std::endl;

    compensated::value cz{z};
    cz += w;
    cz = cz - z;
    cz -= w;
    std::cout << "Compensated operations: " 
              << cz.real() << std::endl;

    return 0;
}
``` 

On x86-64, the above program outputs: 
```
Ordinary operations:    1e-30
Compensated operations: 0
```
In other words, the numerical error is eliminated.

## Installation

Since *Compensated* is a header-only library, there's no need to compile
anything. However, the project is configured for use with **CMake**, which helps
build the unit tests and the example program.

To manually install the library, simply copy the files
```
compensated/
├── compensated.h
└── LICENSE
```
to wherever you need them to be.

## Documentation

The documentation for *Compensated* is [available in PDF
format](/doc/Compensated.pdf). The PDF uses a dark color theme.

## License

The *Compensated* library is released under the terms of the
[3-Clause BSD License](https://opensource.org/licenses/BSD-3-Clause).

© Copyright 2021 by [Rafał M. Siejakowski](https://rs-math.net)

### Acknowledgements

During the work on *Compensated*, Rafał Siejakowski was supported by grant
2018/12483-0 from the São Paulo Research Foundation (FAPESP).  All opinions,
assumptions and conclusions or recommendations expressed in this material are
the responsibility of the author and do not necessarily reflect the point of
view of the FAPESP.

