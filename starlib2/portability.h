/* This software is copyright (c) 2001 Board of Regents, University of
 * Wisconsin. All Rights Reserved.
 */
#ifndef PORTABILITY_GUARD
#define PORTABILITY_GUARD

///////////////////////////////////////
//          portability.h (local)
//          ---------------------
//  Sets up stuff that will vary from compiler
//  to compiler.
//
//  Mostly it's for including the right headers.
//////////////////////////////////////


#include <limits.h>
#include <iostream>

#undef ARCH_DEFINED

#ifdef IS_IRIX

    // Sets a flag saying that an archetecture has been defined.
#   define ARCH_DEFINED

#   define NEEDS_EXPLICIT_TEMPLATES
#   include <strings.h>
#   include <bool.h>
#   include <mstring.h>
#   include <string>

#   define PORTABLE_VECTOR_INCLUDE <vector.h>

    // This annoying macro is needed because Irix's mstring
    // misnames the "erase" method as "remove" and my
    // code needs to handle this anomoly portably:
#   define erase_str(s) (s).remove

    // The syntax for instantiating template classes:
#   define TEMPLATE_INSTANTIATE(cls,T,name) class name : cls<T> {};

#endif

#ifdef IS_GCC

    // Sets a flag saying that an archetecture has been defined.
#   define ARCH_DEFINED

// Version 3 and up of GNUC no longer need to instantiate templates:
#   define NEEDS_EXPLICIT_TEMPLATES

#   include <string>
#   include <string.h>

#   define PORTABLE_VECTOR_INCLUDE <vector>
    using namespace std;

    // This annoying macro is needed because Irix's mstring
    // misnames the "erase" method as "remove" and my
    // code needs to handle this anomoly portably:
#   define erase_str(s) (s).erase

    // The syntax for instantiating template classes:
#   define TEMPLATE_INSTANTIATE(cls,T,name) template class cls<T>; /*name*/

#endif

#ifndef ARCH_DEFINED
    // If none of the archetectures above have been defined, error
    // out and tell the programmer, by putting in a deliberate error.

    // -----------------------------------------------------------------
    // THIS SYNTAX ERROR IS DELIBERATE, PLEASE SEE BELOW FOR EXPLANATION
    // -----------------------------------------------------------------

#include "Deliberate error: Unspecified compiler please see portability.h"
#undef PORTABILITY_GUARD
// Cause infinite recursion by including myself, thus repeating the same
// error over and over and over, or if the compiler is smart, making it
// just detect the recursion and quit outright.  The compiler should at
// least get tired and quit eventually from the number of errors, even if it
// doesn't quit right away.
// (stuff like "#pragma"s and "#error" to get it to quit are not portable.)
#include "portability.h"

    // -----------------------------------------------------------------
    // THIS SYNTAX ERROR IS DELIBERATE, PLEASE SEE BELOW FOR EXPLANATION
    // -----------------------------------------------------------------

    // If you see this errror, then no understood compiler archetecture
    // was defined with a commandline -D... option.  This piece of code
    // gets #ifndef'ed out when a valid archetecture is defined.
    //
    // To correct the situation.:  Do one of the following:
    //
    // 1 - Read the above #ifdefs and select the one appropriate to
    // your archetecture and define it in your makefile in the flags
    // to the C compiler, for example:
    //     CC -DIS_IRIX, or g++ -DIS_GCC
    //
    // 2 - If none of the existing defines is correct for your system,
    // you can start work on a port by copying the section below and
    // adjusting it to fit the settings for your archetecture/compiler.
    // If you get this to work, please contact the starlib maintainers
    // to inform them of your port so it can be made public in the next
    // version.
#else

#ifdef IS_GENERIC_TEMPLATE
    // (This is the template to copy from when you wish to modify
    // this for some OS not listed above.)
    //
    // To support a new OS not previously supported:
    //     1 - Make a copy of this template.
    //     2 - Rename the ifdef to #ifdef IS_OSNAMEHERE
    //     3 - Change according to what the comments say below.
    //     4 - edit Makefile and add a section for your new OS
    //         that, among other things, adds -DIS_OSNAMEHERE to
    //         $(CPPFLAGS).
    //     5 - Note that multiple defines are allowed, for example,
    //         -DIS_LINUX and -DIS_GCC, however they should make some
    //         sense (turning on IS_LINUX and IS_IRIX at the same time
    //         makes no sense.)

    // Uncomment if your system needs template classes
    // to be explicitly declared:
//#   define NEEDS_EXPLICIT_TEMPLATES

    // Ansi-C string library (strcpy(), strlen(), etc...)
    // Uncomment whichever is appropriate:
//#   include <string.h>
//#   include <cstring.h>

    // If your compiler does not include boolean types implicitly:
//#   include <bool.h>

    // C++ string class library:
    // uncomment whichever is appropriate:
//#   include <mstring.h> // Older Irix compilers
//#   include <string.h>  // (typically if 'cstring.h' is the Ansii C strings.)
//#   include <string>    // (typically if 'string.h' is the Ansii C strings.)

    // The syntax for instantiating template classes.  Alter as you need
    // to for your compiler: the parameters are:
    //
    //     cls = the class's base name.
    //
    //     T = the type to instantiate on.
    //
    //     name = a dummy identifier to use if you need to make a variable
    //            of this new class type or something like that.
    // example:
    //     class cls<T>  name;
    //
#   define TEMPLATE_INSTANTIATE(cls,T,name) class name : cls<T> {};

#endif //IS_GENERIC_TEMPLATE

#endif //ARCH_DEFINED

#endif //PORTABILITY_GUARD

