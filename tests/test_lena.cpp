#include "lena.h"

#include <stdio.h>
#include <iostream>
#include <Windows.h>

void f1(void)
{
    volatile int a = 2 + 2;
}

void f2()
{
    volatile int a = 2 + 2;
}

int PrintContext(CONTEXT* ctx)
{
    printf("rax=%016llx rbx=%016llx rcx=%016llx\n", ctx->Rax, ctx->Rbx, ctx->Rcx);
    printf("rdx=%016llx rsi=%016llx rdi=%016llx\n", ctx->Rdx, ctx->Rsi, ctx->Rdi);
    printf("r8 =%016llx r9 =%016llx r10=%016llx\n", ctx->R8, ctx->R9, ctx->R10);
    printf("r11=%016llx r12=%016llx r13=%016llx\n", ctx->R11, ctx->R12, ctx->R13);
    printf("r14=%016llx r15=%016llx rbp=%016llx\n", ctx->R14, ctx->R15, ctx->Rbp);
    printf("rsp=%016llx rip=%016llx\n", ctx->Rsp, ctx->Rip);
    if(ctx->Rip == (DWORD64)f2)
    {
        return LENA_STOP_TRACING;
    }
    return LENA_CONTINUE_TRACING;
}

// User can do more management in order to support multiple breakpoints.
void SetHardwareBreakpoint(CONTEXT* ctx, uintptr_t address, uint32_t thread_id)
{
    ctx->Dr0 = address;
    ctx->Dr7 |= 0x1;
}

void UnsetHardwareBreakpoint(CONTEXT* ctx, uint32_t thread_id)
{
    ctx->Dr0 = 0;
    ctx->Dr7 = ctx->Dr7 & 0xFFFFFFFFFFFFFFFE;
}

int main(int argc, char **argv)
{
    lena::LenaDebugger dbg;

    dbg.SetHardwareBreakpoint = SetHardwareBreakpoint;
    dbg.UnsetHardwareBreakpoint = UnsetHardwareBreakpoint;
    lena::Init(&dbg);
    lena::TraceThread(GetCurrentThreadId(), (uintptr_t)f1, PrintContext);
    f1();
    f2();
    f1();
    f2();
    lena::Cleanup();
    return 0;
}