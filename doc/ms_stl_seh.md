
# 1. Windows and C++ exceptions

> (c) 2020 by dbj@dbj.org -- LICENSE_DBJ -- https://dbj.org/license_dbj/

- [1. Windows and C++ exceptions](#1-windows-and-c-exceptions)
    - [1.0.1. The current MS STL status](#101-the-current-ms-stl-status)
  - [1.1. Usage](#11-usage)
  - [1.2. Themes and Issues](#12-themes-and-issues)
    - [1.2.1. MS STL and SEH and C++ exceptions](#121-ms-stl-and-seh-and-c-exceptions)
      - [1.2.1.1. Down the Rabbit Hole](#1211-down-the-rabbit-hole)
      - [1.2.1.2. Into the realm of Windows](#1212-into-the-realm-of-windows)
      - [1.2.1.3. Use Watson instead?](#1213-use-watson-instead)
  - [1.3. SEH friendly, standard Windows C++, you can try](#13-seh-friendly-standard-windows-c-you-can-try)
    - [1.3.1. COM, C++ and /kernel builds](#131-com-c-and-kernel-builds)
  - [1.4. Conclusion(s)](#14-conclusions)


What seems to be the issue? The issue seems to be it is not clearly documented how to use standard C++ and MS STL without C++ exceptions, on Windows. Many people are even unaware they can use any MS STL without exceptions.

Since C++98 one can use standard C++ feature macros:

| Feature	| Macro name |	Value |	Header
|-----------|------------|--------|-------
| Run-time type identification (dynamic_cast, typeid)|	__cpp_rtti	| 199711 |	predefined
| Exception handling |	__cpp_exceptions | 199711 | predefined

On Windows builds we can use these CL predefined macros:

```cpp
// defined if /kernel switch is used
#ifdef _KERNEL_MODE
printf(  ": _KERNEL_MODE is defined");
#else // ! _KERNEL_MODE
printf(  ": _KERNEL_MODE is NOT defined");
#endif // !_KERNEL_MODE

// depends on the _KERNEL_MODE
#if _HAS_EXCEPTIONS
printf(  ": _HAS_EXCEPTIONS == 1");
#else
printf(  ": _HAS_EXCEPTIONS == 0");
#endif // _HAS_EXCEPTIONS

#if _CPPRTTI 
printf(  ": _CPPRTTI == 1");
#else
printf(  ": _CPPRTTI == 0");
#endif // ! _CPPRTTI

#if _CPPUNWIND 
printf(  ": _CPPUNWIND == 1");
#else // ! _CPPUNWIND 
printf(  ": _CPPUNWIND == 0");
#endif //! _CPPUNWIND 
```
<span id="sehbuild" name="sehbuild">Ditto, the "SEH build" I might define as:
</span>

```cpp
#define SEH_BUILD (_HAS_EXCEPTIONS == 0) 
```
Windows OS is written in C. WIN32 is C API. 

> SEH is always available to any Windows based code.

What is **Windows no C++ exceptions build** ? That is cl.exe C/C++ build without any `/EH` switch or with a famous `/kernel` switch. [SEH](https://docs.microsoft.com/en-us/cpp/cpp/structured-exception-handling-c-cpp?view=vs-2019) is intrinsic to Windows. SEH is in the foundations of Windows programming. 

### 1.0.1. The current MS STL status

On the official side, things are happening around the issue of MS STL and "SEH builds":

- https://github.com/microsoft/STL/issues/1289
- https://github.com/microsoft/STL/issues/639


My findings and thoughts are also in comment or snippets and the samples. What is perhaps interesting in here is to use this project, to try and check different parts of MS STL behaviour, in the SEH builds, with no C++ exceptions whatsover. And then follow through debugger, to where is that code taking you into the depths of the vast MS STL code base.

## 1.1. Usage

Of course this is strictly Windows code. If SEH exception is raised, and if it is caught, the new "minidump" dmp file is created. You are informed where is it saved; what is the full path. 

Since SEH is intrinsic to Windows and CL.exe, that always works and catches all potential SE's. C or C++, exceptions or no exceptions available.

> Using `/kernel` switch  wit cl.exe makes c++ keywords `try`,`throw` and `catch` into compilation errors.

To open that minidump file created, you need **Visual Studio**. After which [in the upper right corner](https://docs.microsoft.com/en-us/visualstudio/debugger/using-dump-files?view=vs-2019), you will spot the link to the native debugging entitled ["Debug With Native Only"](https://docs.microsoft.com/en-us/visualstudio/debugger/media/dbg_dump_summarypage.png?view=vs-2019). Click on that and soon you will be pushed to the point where the actual C++ or SEH exception was thrown from. Thus you need Visual Studio too.

![vstudio_minidump_dialogue](https://docs.microsoft.com/en-us/visualstudio/debugger/media/dbg_dump_summarypage.png?view=vs-2019)

Generating minidump is one very powerful feature.

## 1.2. Themes and Issues

**Standard and SEH**

Standard C++ specification does not mention SEH, or Windows. It seems (at least to me) author of C++ (one Mr B. Stroustrup) has expressed explicit dislike for MSFT SEH, in this very recent paper: [P1947R0]( 
http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2019/p1947r0.pdf). 

My opinion: There is a **lot** of customers who are "windows only" orineted, and a lot of projects serving them. Especially those customers needing low level Windows "server side" high performance components. In that situation one has to do as Windows customer demands.

But what that exactly means for developers if in constant need of MS STL? Is standard C++ standard library, implemented by Microsoft team, aka MS STL, capable to works if no C++ exception throwing is designed and implemented? (circa 2020 OCT)? 

Let us look into the MS STL code.

### 1.2.1. MS STL and SEH and C++ exceptions

From inside MS STL (as far as I can see), only eight exceptions are thrown. And they are thrown by calling eight `[[noreturn]]` functions. Not using the `throw` keyword.

All 8 are in (if above defined) [SEH build](#sehbuild) is in effect, compiled as 8 SEH raising functions. 

Evident in existing MS STL source available, circa 2020 Q4. Implemented inside https://github.com/microsoft/STL/blob/master/stl/src/xthrow.cpp. 

Good, but what is calling those eight?

#### 1.2.1.1. Down the Rabbit Hole

Please note: MS STL using or not using C++ exceptions, depends on the `_HAS_EXCEPTIONS` inbuilt macro. (look into `vcruntim.h` arround line 100)

From wherever in MS STL, exception is to be thrown, one of those eight functions is called. As an example if you do this

```cpp
std::vector<bool> bv_{ true, true, true } ;
// throws standard C++ exception, but
// transparently becomes SE (Structured Exception)
// in case /kernel is used or there is simply no /EH switch
auto never = bv_.at(22);
```
you can follow with your debugger and see std vector `at()` is very simple:
```cpp
// <vector> #2590
    _NODISCARD reference at(size_type _Off) {
        if (size() <= _Off) {
            _Xran();
        }
        return (*this)[_Off];
    }
```
Where `Xran()` is one of the only two noreturn points available inside the `std::vector<T>` :
```cpp
// <vector> # 2837
    [[noreturn]] void _Xlen() const {
        _Xlength_error("vector<bool> too long");
    }
    // called from std::vector at() method
    [[noreturn]] void _Xran() const {
        _Xout_of_range("invalid vector<bool> subscript");
    }
```
Where that `_Xout_of_range()` is declared inside `<xutility>`, together with other no return friends
```cpp
// <xutility> #5817
[[noreturn]] _CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL _Xbad_alloc();
[[noreturn]] _CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL _Xinvalid_argument(_In_z_ const char*);
[[noreturn]] _CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL _Xlength_error(_In_z_ const char*);
[[noreturn]] _CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL _Xout_of_range(_In_z_ const char*);
[[noreturn]] _CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL _Xoverflow_error(_In_z_ const char*);
[[noreturn]] _CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL _Xruntime_error(_In_z_ const char*);
```
There is eight of them. Above six (and other two for: `bad_function_call` and `regex_error`) are defined (implemented) in the above mentioned xthrow.cpp. [Available here](https://github.com/microsoft/STL/blob/master/stl/src/xthrow.cpp), as part of MS STL open source. 

That is as far as you can follow through debugger while using VS Code.

Let us go back to our `_Xout_of_range` function  inside `xthrow.cpp`
```cpp
// xthrow.cpp # 24
[[noreturn]] _CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL _Xout_of_range(_In_z_ const char* const _Message) {
    _THROW(out_of_range(_Message));
}
```
And that _THROW is defined in relation to, do we have, or do we not have C++ exceptions available in the current build. Meaning: depending on what was the value of the `_HAS_EXCEPTIONS` macro. 

All is revealed in the following section of `<yvals.h>`
```cpp
//<yvals.h> # 447
// EXCEPTION MACROS
#if _HAS_EXCEPTIONS
#define _TRY_BEGIN try {
#define _CATCH(x) \
    }             \
    catch (x) {
#define _CATCH_ALL \
    }              \
    catch (...) {
#define _CATCH_END }

#define _RERAISE  throw
#define _THROW(x) throw x

#else // _HAS_EXCEPTIONS
#define _TRY_BEGIN \
    {              \
        if (1) {
#define _CATCH(x) \
    }             \
    else if (0) {
#define _CATCH_ALL \
    }              \
    else if (0) {
#define _CATCH_END \
    }              \
    }

#ifdef _DEBUG
#define _RAISE(x) _invoke_watson(_CRT_WIDE(#x), __FUNCTIONW__, __FILEW__, __LINE__, 0)
#else // _DEBUG
#define _RAISE(x) _invoke_watson(nullptr, nullptr, nullptr, 0, 0)
#endif // _DEBUG

#define _RERAISE
#define _THROW(x) x._Raise()
#endif // _HAS_EXCEPTIONS
```
`_invoke_watson()`? Yes, I know, all will be explained in time. Next, if we look back into `xthrow.cpp` :
```cpp
// xthrow.cpp # 24
[[noreturn]] _CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL _Xout_of_range(_In_z_ const char* const _Message) {
    _THROW(out_of_range(_Message));
}
```
You will quickly understand, in case of no C++ exceptions that becomes
```cpp
// xthrow.cpp # 24
[[noreturn]] _CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL _Xout_of_range(_In_z_ const char* const _Message) {
    out_of_range(_Message)._Raise() ;
}
```

Which means on the instance of `std::out_of_range` exception type, there has to be this little peculiar `_Raise()` method. 

And, for some people, this is the point of contention where MS STL is leaving the realm of ISO C++ and entering the realm of Windows C++ and SEH. 

#### 1.2.1.2. Into the realm of Windows

If irrelevant for this analysis, code is left out in the analysis bellow. Back to the MS STL `<exception>`:

```cpp
#if _HAS_EXCEPTIONS

// <exception> # 32
#include <malloc.h>
#include <vcruntime_exception.h>
```
One can understand in case of C++ exceptions in MS STL realm, the file `<vcruntime_exception.h>` gets involved. If curious please follow that path. That is where `std::exception` is actually implemented. Back to `<exception>` at hand:
```cpp
// <exception> # 63
#else // !  _HAS_EXCEPTIONS
```
That is the point where MS STL SEH world begins. For `_HAS_EXCEPTIONS == 0`, there is this global `_Raise_handler` function pointer.
```cpp
using _Prhand = void(__cdecl*)(const exception&);
// <exception> # 76
extern _CRTIMP2_PURE_IMPORT _Prhand _Raise_handler; // function pointer to the raise handler
```
And in MS STL there is complete separate version (gasp!) of `std::exception` in existence for the `_HAS_EXCEPTIONS == 0` scenarios. It begins here:
```cpp
// <exception> # 81
class exception { // base of all library exceptions
public:
    static _STD _Prhand _Set_raise_handler(_STD _Prhand _Pnew) { // register a handler for _Raise calls
        const _STD _Prhand _Pold = _STD _Raise_handler;
        _STD _Raise_handler      = _Pnew;
        return _Pold;
    }
```
That `_Set_raise_handler` public method on the MS STL `std::exception` is obviously called (from somewhere) to set the global raise handler, in the `_HAS_EXCEPTIONS == 0` scenario.

In the `cl` compiler, the `throw` keyword is forbidden in the `/kernel` aka `_HAS_EXCEPTIONS == 0` scenario. 

And here is this above mentioned `_Raise()` method in existence on the MS STL `std::exception`. In no C++ exceptions scenarios, it is called instead of throwing the exceptions class instance. 

```cpp    
// <exception> # 106
[[noreturn]] void __CLR_OR_THIS_CALL _Raise() const 
{ // raise the exception
    if (_STD _Raise_handler) {
        (*_STD _Raise_handler)(*this); // call raise handler if present
    }
    _Doraise(); // call the protected virtual
    _RAISE(*this); // raise this exception
}
```
While the `_Doraise` is just a little bit further down:
```cpp
protected:
    virtual void __CLR_OR_THIS_CALL _Doraise() const {} // perform class-specific exception handling
}; // eof std::exception for   _HAS_EXCEPTIONS == 0 scenario
// <exception> # 198
#endif // _HAS_EXCEPTIONS
```
And, this is the end of the non standard MS STL exception class that exists whenever `_HAS_EXCEPTIONS == 0`. 
`bad_alloc`, `bad_array_new_length`, `bad_exception` also have different SEH versions for the `_HAS_EXCEPTIONS == 0` scenario. 

Ditto. The MS STL std exception `_Raise()` method uses two levels of indirection: `_Raise_handler` global function pointer and protected `_Doraise` method, before eventually calling the `_RAISE` macro. Remember all of of that is inside `[[noreturn]] void _Raise() const` method on the non standard MS STL version of `std::exception`.

And if we quickly dive back into `<yvals.h>` mentioned above, we shall understand, for `_HAS_EXCEPTIONS == 0` scenario,  `_RAISE(x)` macro is defined as:
```cpp
// <yvals.h> # 475
#ifdef _DEBUG
#define _RAISE(x) _invoke_watson(_CRT_WIDE(#x), __FUNCTIONW__, __FILEW__, __LINE__, 0)
#else // _DEBUG
#define _RAISE(x) _invoke_watson(nullptr, nullptr, nullptr, 0, 0)
#endif // _DEBUG
```
I think it might be safe to assume the global pointer, used inside `_Raise()` method does the work:

```cpp
    if (_STD _Raise_handler) {
        (*_STD _Raise_handler)(*this); // call raise handler if present
    }
```
Why is that "safe to assume" my dear Watson?

#### 1.2.1.3. Use Watson instead? 

"Dr Watson" is coming [straight from the Windows lore](https://devblogs.microsoft.com/oldnewthing/20051114-00/?p=33353). And possibility of using that in 2020 for MS STL is interesting, to put it mildly. 

Ok then, is it really true good doctor is called? If you do native debugging on the core dump (the one [DBJ+FWK](https://github.com/dbj-data/dbj-fwk) also creates), you can see the source of the SE raised, is indeed in `xthrow.cpp`. Dutifully reported as "Microsoft C++ exception". But not much elase. One does not know if Dr Watson called or not. Looking into the source we can see it is supposedly being called from that `_Raise()` method. But only as the third line of defence. Here it is again:

```cpp    
// <exception> # 106
[[noreturn]] void __CLR_OR_THIS_CALL _Raise() const 
{ 
    // this is very likely always used to raise SE
    if (_STD _Raise_handler) {
        (*_STD _Raise_handler)(*this); // call raise handler if present
    }
    // this is currently an empty protected method
    _Doraise();
    // this is _invoke_watson() not
    _RAISE(*this); // raise the structured exception
}
```

Here is the explanation of his role in this thriller. Once upon a time, Dr Watson was guiding customers into the wonderful kingdom of this little known and peculiar office of "Windows Error Reporting"; to the inner circle of Windows Elders, known under the acronym of [WER](https://docs.microsoft.com/en-us/windows/win32/wer/windows-error-reporting).

[WER](https://en.wikipedia.org/wiki/Windows_Error_Reporting) is Windows Legacy. WER basically was that place from where you can call back to daddy and complain. 

*"... enables users to notify Microsoft of application faults, kernel faults, unresponsive applications, and other application specific problems. Microsoft can use the error reporting feature to provide customers with troubleshooting information, solutions, or updates for their specific problems. Developers can use this infrastructure to receive information that can be used to improve their applications..."*

That AFAIK is still in use but mostly in the twilight zone also called: "important customers". 

Windows as you know it today is actually Windows NT. And one of the foundation stones of Win NT are "Structured Exceptions" aka [SEH](https://en.wikipedia.org/wiki/Microsoft-specific_exception_handling_mechanisms#SEH). Thus we can sober up, achieve SEH we want and simply use:

```cpp
// get around a good doctor
// WIN32 API
RaiseException( YOUR_SE_UID , EXCEPTION_NONCONTINUABLE, 0, {});
```
What DBJ+FWK does is simply catch all, any and every SE on the application top level. WIN32 is calling the above whenever there is a reason. In case of stack overflow, division with zero and all the system level true "exception" one can not hope to handle without SEH.

Ditto, in case of [SE caught](https://docs.microsoft.com/en-us/windows/win32/debug/using-an-exception-handler), what [DBJ+FWK](https://github.com/dbj-data/dbj-fwk) does is create and save a minidump. And looking into that minidump file with Visual Studio, you can pinpoint the issue that made the application misbehave. That includes every possible issue, not just C++ exceptions being thrown. 

*And that is very powerful*. DBJ+FWK simply has this standard SE aware main (hint: all possible four versions). That is **not** very complicated and has a lot of benefits. 

Please do understand SE is inbuilt in Windows and in the CL compiler, where there are SE intrinsics too. Including the [keywords added](https://docs.microsoft.com/en-us/windows/win32/debug/abnormaltermination), available to both C and C++. That works in every possible Windows build.

## 1.3. SEH friendly, standard Windows C++, you can try 

Now you know how MS STL, SEH mechanism and design works. You can try that too in your C++ Windows, SEH friendly code. An simple example:

```cpp
// all your code is in the 'my' namespace
namespace my {
    // my::constants
    inline namespace constants {
        enum error_type { 
        error_ctype,
        error_syntax
    };
    } 
```
After defining few error id's, you will define the exception class. Lets call it `error`. Importantly `my::error` does not inherit from `std::exception`:
```cpp
struct error final 
{ 
    error () = default ;
    const char * msg_ {"unknown"};
    const char * what () const noexcept { return msg_ ; 
    explicit  error( constants::error_type ex_ ) noexcept : err_(ex_)   
    { 
        /* here make the message by the code */ 
    }

    my::constants::error_type code() const 
    {  return err_ ;    }

    // raise the SE from here
    [[noreturn]] void raise() const { 
        RaiseException( YOUR_SE_UID , EXCEPTION_NONCONTINUABLE , 0, {});
    }

} ; // eof error 
```
You can enjoy reading about [`RaiseException`](https://docs.microsoft.com/en-us/windows/win32/api/errhandlingapi/nf-errhandlingapi-raiseexception) if you wish to pass your details and such. Going further. You  will always use the `MY_THROW` macro, instead of using the C++ `throw` keyword:
```cpp
// x is the instance of my::error
#if _HAS_EXCEPTIONS == 0
      MY_THROW(x) x.raise() ;
#else
      MY_THROW(x) throw x ;
#endif
```
That is where and how we flip/flop between SEH and C++ unwinding. Lastly, there is always a function that does the raise, and does not return. A level of indirection to improve the change-ability of the design always has to exist:
```cpp
[[noreturn]] inline void __cdecl 
    error_throw (const constants::error_type code_) 
    {
        MY_THROW( error(code_) ) ;
    }
} // eof my ns
```
In any case and always. To enjoy the SEH benefits, your Windows main should always be "SEH enabled", and it has to look something like this:
```cpp
// The "Standard" main()
// this works in all kinds of Windows builds
extern "C" int main (int argc, char ** argv) 
{
     __try 
    { 
        __try {
           my::error_throw( constants::error_type::error_syntax ) ;
        }
        __finally {
            // will be always visited
        }
    }
    __except ( EXCEPTION_EXECUTE_HANDLER ) 
    { 
/*
aleternatively:
    DWORD exception_code_{};
     . . .
__except (exception_code_ = GetExceptionCode(), EXCEPTION_EXECUTE_HANDLER )
     . . .
     use that code to process specific SE's
*/        
        // your code here
    }
    return 0 ;
}
```
That will always work c++ exception or no C++ exceptions. In case you want C++ exceptions you can not mix that in the same function, so you just call some entry point function, into your standard C++ app from the `main()` above.

You next level of mastership is to learn about and use [`GetExceptionInformation`](https://docs.microsoft.com/en-us/windows/win32/debug/getexceptioninformation). A macro which is noting less than a real keyword in the CL compiler.

Recap! If you build with `/EHsc` your app will be: c++ exceptions **and** SEH enabled. That is: using `cl`, SEH intrinsics are **always** enabled. (And are declared in `<excpt.h>`)

> In Windows C/C++, SEH is always there

 ### 1.3.1. COM, C++ and /kernel builds

 Slight but very relevat detour.

[MSFT COM](https://en.wikipedia.org/wiki/Component_Object_Model) predates C++ standardizations. Just like SEH does. Since 1993 (gasp!) working on Windows code, one was meeting COM head-on, sooner or latter. And only those who like pain, use the COM natively through its C API. Naturally there was several notable Microsoft teams attempts to encapsualte that inside some C++ code.

Thus some "few years after", ["Compiler COM Support"](https://docs.microsoft.com/en-us/cpp/cpp/compiler-com-support?redirectedfrom=MSDN&view=vs-2019) was designed, implemented and inbuilt straight into a CL compiler. Where it is in existence to this day. And, that was designed to use C++ exceptions. Not SEH. Which is strange.

In 2020 Q4, if and when attempting using `<comdef.h>` types, in what we have defined above as "SEH builds", C++ exceptions are replaced with SEH.Transparently.

You need to know right now, [SEH is happening in there "by accident"](https://github.com/MicrosoftDocs/cpp-docs/issues/2494#issuecomment-701200395). 

Pleas do not rely on `<comdef.h>` + `/kernel` ("SEH builds") combination until further notice. "Compiler COM Support" and SEH is accidental combination that apparently just happens to work, 2020 Q4.

My opinion? MSFT ["Compiler COM Support"](https://docs.microsoft.com/en-us/cpp/cpp/compiler-com-support?redirectedfrom=MSDN&view=vs-2019) I do like, it is C++, it is simple and it "just works". But in these halcion days of ISO C++, somebody has decided \<comdef.h> and friends will firmly stay inside c++ exceptions territory. Which is weird since the whole the MS STL can switch easily to SEH builds from C++ exceptions.

Reminbder: Easily but not transparently. In the [SEH build](#sehbuild), keywords `try`,`throw` and `catch` do not compile.

## 1.4. Conclusion(s)

Many C++ afficionados are, but I am not that perturbed with MS STL apparent dual personality and ability to transparently switch of C++ exceptions.

Neither many projects should be usurped. Why not? Let me repeat the facts. Most of the customers are on Windows. WIN32 is primary Windows API. WIN32 is C. Same as UTF16 (`wchar_t`) is standard on windows, not `char`, or this '\\' is path delimiter and not this '/', in the same manner, SEH is intrinsic on Windows, not C++ exceptions.

For Windows developments, SEH is the norm. Embrace it.
