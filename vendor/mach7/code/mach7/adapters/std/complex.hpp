//
//  Mach7: Pattern Matching Library for C++
//
//  Copyright 2011-2013, Texas A&M University.
//  Copyright 2014, Yuriy Solodkyy.
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions are met:
//
//      * Redistributions of source code must retain the above copyright
//        notice, this list of conditions and the following disclaimer.
//
//      * Redistributions in binary form must reproduce the above copyright
//        notice, this list of conditions and the following disclaimer in the
//        documentation and/or other materials provided with the distribution.
//
//      * Neither the names of Mach7 project nor the names of its contributors
//        may be used to endorse or promote products derived from this software
//        without specific prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
//  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
//  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
//  IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY
//  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
//  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
//  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
//  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
//  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

///
/// \file
///
/// This file is a part of Mach7 library test suite.
///
/// \author Yuriy Solodkyy <yuriy.solodkyy@gmail.com>
///
/// \see https://parasol.tamu.edu/mach7/
/// \see https://github.com/solodon4/Mach7
/// \see https://github.com/solodon4/SELL
///

#include <mach7/patterns/bindings.hpp>     // Mach7 support for bindings on arbitrary UDT
#include <complex>                         // std::complex<T>


// We introduce 2 layouts: cart (for cartesian, default since std::complex uses it by default) and plar (for polar).
enum { cart = mch::default_layout, plar = 1 };

// We can now provide bindings for each layout independently:

namespace mch ///< Mach7 library namespace
{
    ///@{
    /// We used to be able to write nicely like this:
    ///
    /// \code
    ///   template <typename T> struct bindings<std::complex<T>, cart> { Members(std::real<T>, std::imag<T>); };
    ///   template <typename T> struct bindings<std::complex<T>, plar> { Members(std::abs<T> , std::arg<T>);  };
    /// \endcode
    ///
    /// but C++ 11 introduced a new overload for numeric types T for
    /// std::real<T>, std::imag<T>, std::abs<T> and std::arg<T>, which messed up
    /// our nice syntax. You can still do it on your functions, but in case of
    /// complex we have to either take address of a corresponding member-function
    /// or do an explicit cast first to shrink the overload set:
    template <typename T> struct bindings<std::complex<T>, cart> { Members(std::complex<T>::real, std::complex<T>::imag); };
    template <typename T> struct bindings<std::complex<T>, plar>
    {
#if defined(_MSC_VER) && _MSC_VER==1800
        // VS2013 gets an ICE here when casting to reference, so we workaround
        // it with taking an address instead which Members handels as well.
        // We could have used address version and not have to check for VC, but
        // now that i've spent time figuring out the cause, i might as well
        // leave a trace of that investigation here for similar issues in the futre.
        Members((T (*)(const std::complex<T>&))&std::abs<T>, (T (*)(const std::complex<T>&))&std::arg<T>);
#else
        Members((T (&)(const std::complex<T>&))std::abs<T>, (T (&)(const std::complex<T>&))std::arg<T>);
#endif
    };
    ///@}
} // of namespace mch

//------------------------------------------------------------------------------

// Now we merry layout with the class via Mach7 library view class:
typedef mch::view<std::complex<double>,cart> cartesian;
typedef mch::view<std::complex<double>,plar> polar;

//------------------------------------------------------------------------------
