/*
Copyright (C) 2002-2015 Electronic Arts, Inc.  All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

1.  Redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer.
2.  Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in the
documentation and/or other materials provided with the distribution.
3.  Neither the name of Electronic Arts, Inc. ("EA") nor the names of
its contributors may be used to endorse or promote products derived
from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY ELECTRONIC ARTS AND ITS CONTRIBUTORS "AS IS" AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL ELECTRONIC ARTS OR ITS CONTRIBUTORS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef INCLUDED_eastdarg_H
#define INCLUDED_eastdarg_H


#include <EABase/eabase.h>
#include <stdarg.h>


// VA_ARG_COUNT
//
// Returns the number of arguments passed to a macro's ... argument.
// This applies to macros only and not functions.
//
// Example usage:
//    assert(VA_ARG_COUNT() == 0);
//    assert(VA_ARG_COUNT(a) == 1);
//    assert(VA_ARG_COUNT(a, b) == 2);
//    assert(VA_ARG_COUNT(a, b, c) == 3);
//
#if !defined(VA_ARG_COUNT)
	#define VA_ARG_COUNT(...)                         VA_ARG_COUNT_II((VA_ARG_COUNT_PREFIX_ ## __VA_ARGS__ ## _VA_ARG_COUNT_POSTFIX,32,31,30,29,28,27,26,25,24,23,22,21,20,19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0))
	#define VA_ARG_COUNT_II(__args)                   VA_ARG_COUNT_I __args
	#define VA_ARG_COUNT_PREFIX__VA_ARG_COUNT_POSTFIX ,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,0
	#define VA_ARG_COUNT_I(_0,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21,_22,_23,_24,_25,_26,_27,_28,_29,_30,_31,N,...) N
#endif


// va_copy
//
// va_copy is required by C++11
// C++11 and C99 require va_copy to be #defined and implemented.
// http://en.cppreference.com/w/cpp/utility/variadic/va_copy
//
// Example usage:
//     void Func(char* p, ...){
//         va_list args, argsCopy;
//         va_start(args, p);
//         va_copy(argsCopy, args);
//           (use args)
//           (use argsCopy, which acts the same as args)
//         va_end(args);
//         va_end(argsCopy);
//     }
//
#ifndef va_copy
	#if defined(__va_copy) // GCC and others define this for non-C99 compatibility.
		#define va_copy(dest, src) __va_copy((dest), (src))
	#else
		// This may not work for some platforms, depending on their ABI.
		// It works for Microsoft x86,x64, and PowerPC-based platforms.
		#define va_copy(dest, src) memcpy(&(dest), &(src), sizeof(va_list))
	#endif
#endif
 
 
 
// va_list_reference
//
// va_list_reference is not part of the C or C++ standards.
// It allows you to pass a va_list by reference to another
// function instead of by value. You cannot simply use va_list&
// as that won't work with many va_list implementations because 
// they are implemented as arrays (which can't be passed by
// reference to a function without decaying to a pointer).
//
// Example usage:
//     void Test(va_list_reference args){
//         printf("%d", va_arg(args, int));
//     }
//     void Func(char* p, ...){
//         va_list args;
//         va_start(args, p);
//         Test(args); // Upon return args will be modified.
//         va_end(args);
//     }
#ifndef va_list_reference
		// This is required for platform ABIs in which va_list is a struct or pointer.
		#define va_list_reference va_list&
#endif




#endif /* Header include guard */



