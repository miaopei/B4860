#ifndef   UV_EVENT_LOOP_H
#define   UV_EVENT_LOOP_H

#include <thread>
#include <atomic>
#include <functional>
#include "uv.h"

namespace uv
{
using DefaultCallback = std::function<void()>;

class Async;
class EventLoop
{
public:
    enum Mode
    {
        Default,
        New
    };

    EventLoop();
    ~EventLoop();

    static EventLoop* DefaultLoop();

    int run();
    int runNoWait();
    bool isRunInLoopThread();
    void runInThisLoop(const DefaultCallback func);
    uv_loop_t* handle();

    static const char* GetErrorMessage(int status);

private:
    EventLoop(Mode mode);

    std::thread::id loopThreadId_;
    std::atomic<bool> isRun_;
    uv_loop_t* loop_;
    Async* async_;
};

using EventLoopPtr = std::shared_ptr<uv::EventLoop>;
}
#endif

