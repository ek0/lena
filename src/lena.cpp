#include "lena.h"
#include "lenamanager.h"
#include "lenathreadtracer.h"

#include <Windows.h>
#include <cstdint>

namespace lena
{
    void Init(const LenaDebugger* debugger)
    {
        lena::LenaManager::GetInstance().Init(debugger);
    }

    void Cleanup()
    {
        lena::LenaManager::GetInstance().Cleanup();
    }

    void StopTracing()
    {
        // To be called within the callback of the tracer you want to stop
        uint32_t current_tid = GetCurrentThreadId();
        lena::LenaThreadTracer* tracer = lena::LenaManager::GetInstance().GetThreadTracer(current_tid);
        tracer->StopTracing();
    }

    void TraceThread(uint32_t tid, uintptr_t from, LenaCallback callback)
    {
        lena::LenaManager::GetInstance().TraceThread(tid, from, callback);
    }
} // namespace lena