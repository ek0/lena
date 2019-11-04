#ifndef _LENATHREADTRACER_H_
#define _LENATHREADTRACER_H_

namespace lena
{
    class LenaThreadTracer
    {
        LenaCallback trace_callback_;
        uintptr_t from_address_;
        bool is_tracing_;
        uint32_t tid_;

    public:
        LenaThreadTracer(uint32_t tid);
        bool IsTracing() const;
        void SetTracing(bool);
        void StopTracing();
        void SetStartAddress(uintptr_t address);
        void SetCallback(LenaCallback callback);
        int ExecuteCallback(CONTEXT* ctx);
        uintptr_t GetStartAddress() const;
        uint32_t GetThreadId() const;
    };
}

#endif