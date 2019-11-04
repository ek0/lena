#ifndef _LENAMANAGER_H_
#define _LENAMANAGER_H_

#include "lena.h"
#include "lenadebugger.h"
#include "lenathreadtracer.h"

#include <map>
#include <cstdint>

namespace lena
{
    class LenaManager
    {
        const LenaDebugger* debugger_;
        std::map<uint32_t, LenaThreadTracer*> tracers_;
        void* exception_handler_;

        LenaManager(LenaManager&) = delete;
        LenaManager& operator=(const LenaManager&) = delete;

    public:
        LenaManager();
        void Init(const LenaDebugger*);
        void Cleanup();
        void TraceThread(uint32_t, uintptr_t, LenaCallback);
        LenaThreadTracer* GetThreadTracer(uint32_t tid);
        const LenaDebugger* GetDebugger() const;
        int SetHardwareBreakpoint(uintptr_t address, uint32_t thread_id);
        void RemoveThreadTracer(uint32_t tid);
        // Singleton
        static LenaManager& GetInstance()
        {
            static LenaManager instance;
            return instance;
        }
    };
}

#endif // _LENAMANGER_H_