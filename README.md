# Lena

Lena is a small SDK that allows you to quickly write effective tracers for windows. Written in C++ it allows for an easy integration in existing SDK.
Currently, we only support one tracer per thread.

## Usage

See `tests/test_lena.cpp` for a quick example.

## Upcoming features
- Support more tracer (up to 4 per thread?)
- Build a C API

# Documentation

## LenaInit
```c
void lena::Init(LenaDebugger* debugger)
```
Initialize the library. `LenaInit` must be called first in order to use tracing functionalities.

### Parameters
#### `debugger`
`LenaDebugger` that will be used to set and unset hardware breakpoints.

### Remarks
`Lena` uses debug registers internally to initialize its tracers. To avoid incompatibilities with external SDKs, the user must provide its own debugger managing the different hardware breakpoints himself.

## LenaDebugger
```c
struct lena::Debugger
{
    // Callback handling the initialization of the hardware breakpoint.
    void (*SetHardwareBreakpoint)(uintptr_t address, uint32_t thread_id);

    // Callback only called when the hardware breakpoint is hit.
    void (*UnsetHardwareBreakpoint)(CONTEXT* context, uintptr_t address, uint32_t thread_id);
};
```
User defined callbacks to set and unset hardware breakpoints. Hardware breakpoints are used internally to know when to start tracing.

### SetHardwareBreakpoint
When initializing a new tracer, `LenaManager` will call `LenaDebugger::SetHardwareBreakpoint`.
- `ctx`: Context of the thread to set the breakpoint to.
- `address`: desired address.
- `thread_id`: thread id of the thread to set the hardware breakpoint.

### UnsetHardwareBreakpoint
When the tracer hits the first instruction to trace, `LenaDebugger::UnsetHardwareBreakpoint` will be called with the current context.
- `ctx`: Context when the previously set breakpoint is set.

### Parameters
#### `debugger`

## LenaCleanup
```c
void lena::Cleanup()
```
Cleanup the library.

## LenaTraceThread
```c
void lena::TraceThread(uint32_t thread_id, uintptr_t from, LenaCallback callback);
```
### Parameters
#### `thread_id`
Thread ID of the thread you want to trace.

#### `from`
Address where to start tracing.

#### `callback`
A pointer to a `LenaCallback` function. This will be called before every instruction when tracing.

### Remarks
`Lena` makes usage of hardware breakpoint to know when to start tracing. In order to trace every thread, it is the user responsibility to iterate through all process' threads.

## LenaCallback
```c
int LenaCallback(CONTEXT *ctx)
```
Callback to be executed before every instruction.
### Parameters
#### `ctx`
Contains the current context of the thread.

### Return value
- `LENA_CONTINUE_TRACING` to continue tracing the current thread.
- `LENA_STOP_TRACING` to stop tracing the current thread and reset it to its initial state.
- `LENA_DELETE_TRACER` stop tracing and delete the current tracer (thus freeing a slot)
- `LENA_CONTINUE_SEARCH` pass the exception to the next exception handler (can be useful tracing 
application that throw `EXCEPTION_SINGLE_STEP`)

#### Remarks
Returning `LENA_STOP_TRACING` will not delete the tracer, but reset it. To delete your tracer after usage please the user must returns `LENA_DELETE_TRACER`. Not doing so might result in memory leaks as well.