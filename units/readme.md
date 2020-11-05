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

Behind is `DBJ+FWK`. A little framework for SEH enabled apps. It catches Microsoft [SEH](https://en.wikipedia.org/wiki/Microsoft-specific_exception_handling_mechanisms), SE-s. And creates a minidump when it catches one.  Basically (almost) nothing can pass arround it. SE raised or C++ exception raised and any other kind of stack overflow, heap corruption, division with zero and all those dear little insects.

By default we build with no C++ exception and no RTTI. Users are informed what is the current situation of those, before app exits. (that info can be removed)

One can open that minidump file from a VisualStudio 2019 and pinpoint the cause of the SE being raised.

Let us repeat, in case it escaped your attention, SEH catches everything WIN32 can raise: division with zero, stack overflow and all those wird and wonderfull things.


