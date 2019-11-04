#ifndef _LENADEBUGGER_
#define _LENADEBUGGER_
/*
    Debugger Interface.

    Lena relies on hardware breakpoints to initialize its tracers.
    Since hardware breakpoints are tied to the CPU, and to prevent incompatibilities
    with other SDKs that might use them, we prefer to let the user to write its own implementation.

    The use only has to implement callbacks to set and unset hardware breakpoints.

    The way the corresponding registers are managed are left to the user.
*/

#include <cstdint>
#include <Windows.h>

namespace lena
{
    struct LenaDebugger
    {
        // Callback handling the initialization of the hardware breakpoint.
        void (*SetHardwareBreakpoint)(CONTEXT* ctx, uintptr_t address, uint32_t thread_id);

        // Callback called when the hardware breakpoint is hit. In order to remove it.
        // This will only be called on exception.
        // NOTE: Not properly unsetting the hardware breakpoint can result in undefined behaviour
        void (*UnsetHardwareBreakpoint)(CONTEXT* context, uint32_t thread_id);
    };
}

#endif // _LENADEBUGGER_H_