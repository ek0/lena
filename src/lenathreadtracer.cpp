#include "lena.h"
#include "lenathreadtracer.h"

#include <cstdint>

namespace lena
{
    LenaThreadTracer::LenaThreadTracer(uint32_t tid)
    {
        trace_callback_ = nullptr;
        is_tracing_ = false;
        from_address_ = 0;
        tid_ = tid;
    }

    bool LenaThreadTracer::IsTracing() const
    {
        return is_tracing_;
    }

    void LenaThreadTracer::SetStartAddress(uintptr_t address)
    {
        from_address_ = address;
    }

    void LenaThreadTracer::SetTracing(bool tracing)
    {
        is_tracing_ = tracing;
    }

    void LenaThreadTracer::SetCallback(LenaCallback callback)
    {
        trace_callback_ = callback;
    }

    int LenaThreadTracer::ExecuteCallback(CONTEXT* ctx)
    {
        return trace_callback_(ctx);
    }

    uintptr_t LenaThreadTracer::GetStartAddress() const
    {
        return from_address_;
    }

    uint32_t LenaThreadTracer::GetThreadId() const
    {
        return tid_;
    }

    void LenaThreadTracer::StopTracing()
    {
        is_tracing_ = false;
    }
}