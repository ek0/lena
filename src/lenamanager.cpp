//#include "breakpoint.h"
#include "lenamanager.h"

#include <Windows.h>
#include <TlHelp32.h>

#define SET_TRAP_FLAG(ctx) ctx->EFlags |= (1 << 8);
#define UNSET_TRAP_FLAG(ctx) ctx->EFlags &= ~(1 << 8);

namespace lena
{
    LONG LenaExceptionHandler(LPEXCEPTION_POINTERS exception)
    {
        int status = 0;
        uint32_t tid = 0;
        const LenaDebugger* debugger = nullptr;

        if(exception->ExceptionRecord->ExceptionCode == EXCEPTION_SINGLE_STEP)
        {
            tid = GetCurrentThreadId();
            CONTEXT* ctx = exception->ContextRecord;
            LenaThreadTracer* current_tracer = LenaManager::GetInstance().GetThreadTracer(tid);
            debugger = LenaManager::GetInstance().GetDebugger();

            // First hit is hardware breakpoint, not trap flag exception
            // Removing breakpoint and setting trap flag
            if(!current_tracer->IsTracing())
            {
                // First instruction, beginning tracing. Removing HW breakpoint.
                debugger->UnsetHardwareBreakpoint(ctx, tid);
            }
            status = current_tracer->ExecuteCallback(ctx);
            // Handling return condition
            switch(status)
            {
            case LENA_CONTINUE_TRACING:
                SET_TRAP_FLAG(ctx);
                current_tracer->SetTracing(true);
                break;
            case LENA_CONTINUE_SEARCH:
                return EXCEPTION_CONTINUE_SEARCH;
            case LENA_STOP_TRACING:
                // Stop tracing
                UNSET_TRAP_FLAG(ctx);
                current_tracer->SetTracing(false);
                // Resetting tracer?
                debugger->SetHardwareBreakpoint(ctx, current_tracer->GetStartAddress(), current_tracer->GetThreadId());
                break;
            case LENA_DELETE_TRACER:
                UNSET_TRAP_FLAG(ctx);
                LenaManager::GetInstance().RemoveThreadTracer(tid);
                break;
            }
            return EXCEPTION_CONTINUE_EXECUTION;
        }
        return EXCEPTION_CONTINUE_SEARCH;
    }

    LenaManager::LenaManager()
    {
        debugger_ = nullptr;
        exception_handler_ = nullptr;
    }

    void LenaManager::Init(const LenaDebugger* debugger)
    {
        debugger_ = debugger;
        exception_handler_ = AddVectoredExceptionHandler(0x1, LenaExceptionHandler);
    }

    void LenaManager::Cleanup()
    {
        RemoveVectoredExceptionHandler(exception_handler_);
    }

    LenaThreadTracer* LenaManager::GetThreadTracer(uint32_t tid)
    {
        return tracers_.at(tid);
    }

    const LenaDebugger* LenaManager::GetDebugger() const
    {
        return debugger_;
    }

    void LenaManager::RemoveThreadTracer(uint32_t tid)
    {
        delete tracers_.at(tid);
        tracers_.erase(tid);
    }

    int LenaManager::SetHardwareBreakpoint(uintptr_t address, uint32_t thread_id)
    {
        int status = 0;
        HANDLE thread;
        CONTEXT ctx;

        thread = OpenThread(THREAD_ALL_ACCESS, 0, thread_id);
        if(thread == NULL)
        {
            // error
            status = 0;
            return 0;
        }
        if(SuspendThread(thread) == -1)
        {
            // error
            status = 0;
            goto exit;
        }
        ctx.ContextFlags = CONTEXT_ALL;
        if(GetThreadContext(thread, &ctx) == 0)
        {
            // error
            status = 0;
            goto exit;
        }
        // Setting breakpoint
        debugger_->SetHardwareBreakpoint(&ctx, address, thread_id);
        if(SetThreadContext(thread, &ctx) == 0)
        {
            // error
            status = 0;
            goto exit;
        }
        if(ResumeThread(thread) == -1)
        {
            // error
            status = 0;
            goto exit;
        }
    exit:
        CloseHandle(thread);
        return status;
    }

    void SetThreadBreakpoint(LenaThreadTracer* tracer)
    {
        const LenaDebugger* debugger = LenaManager::GetInstance().GetDebugger();
        LenaManager::GetInstance().SetHardwareBreakpoint(tracer->GetStartAddress(), tracer->GetThreadId());
    }

    void LenaManager::TraceThread(uint32_t tid, uintptr_t from, LenaCallback callback)
    {
        DWORD  locTid;
        LenaThreadTracer* tracer;

        tracer = new LenaThreadTracer(tid);
        tracer->SetStartAddress(from);
        tracer->SetCallback(callback);
        tracers_.insert(std::pair<uint32_t, LenaThreadTracer*>(tid, tracer));
        if(tid != GetCurrentThreadId())
            SetHardwareBreakpoint(from, tid);
        else
        {
            HANDLE thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)SetThreadBreakpoint, (LPVOID)tracer, 0, &locTid);
            if(thread == 0)
                return;
            while(WaitForSingleObject(thread, INFINITE) != WAIT_OBJECT_0);
        }
    }
} // namespace lena