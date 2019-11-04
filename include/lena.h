#ifndef _LENA_H_
#define _LENA_H_

#include "lenadebugger.h"

#include <cstdint>
#include <Windows.h>

#define LENA_CONTINUE_TRACING 0x0 // Continue tracing the current thread
#define LENA_STOP_TRACING     0x1 // Stop tracing and reset the thread tracer
#define LENA_DELETE_TRACER    0x2 // Stop tracing and delete the current tracer
#define LENA_CONTINUE_SEARCH  0x3 // When you don't want to handle the exception

// Called on every instruction executed.
typedef int (*LenaCallback)(CONTEXT* ctx);

namespace lena
{
    void Init(const LenaDebugger*);
    void Cleanup();
    void TraceThread(uint32_t, uintptr_t, LenaCallback);
}

#endif // _LENA_H_