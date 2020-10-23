# ubench Units or utest Units

You need to define:

`DBJ_USE_UBENCH`   or   `DBJ_USE_UTEST`

We mix ubench and utest units but conditionaly compile one or the other.

```cpp
// program.cpp

#ifdef DBJ_USE_UBENCH
#include "ubench.h/ubench.h"
 UBENCH_STATE();
#elif DBJ_USE_UTEST
#include "ubench.h/utest.h"
 UTEST_STATE();
#else 
#error "Neither DBJ_USE_UBENCH nor DBJ_USE_UTEST, are defined?"
#endif

// user code start here
// this is called from framework
// framework is where SE handling is implemented
extern "C" int program (int argc , char ** argv ) 
{
#ifdef DBJ_USE_UBENCH
     return ubench_main(argc, argv);
#elif DBJ_USE_UTEST
     return utest_main(argc, argv);
#else
#error "Neither DBJ_USE_UBENCH nor DBJ_USE_UTEST, are defined?"
#endif
}

```

Behind is `fwk`. A little framework for windows console apps. It catches Microsoft SEH, SE-s. And creates a minidump when it catches one.  Basically nothing can pass arround it. SE raised or C++ exception raised.

By default we build with no excpetion and no RTTI. In debug builds you are informed what is the current situation of those, before app exits.

One can open that minidump file from a VisualStudio 2019 and pinpoint the cause of the SE being raised.

In case it escaped your attention, SEH catches everything WIN32 can raise: division with zero, stack overflow and all those wird and wonderfull things.

> NOTE: default compiler is clang-cl.exe.
