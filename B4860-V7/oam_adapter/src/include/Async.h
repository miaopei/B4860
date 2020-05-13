#ifndef UV_ASYNC_H
#define UV_ASYNC_H

#include <memory>
#include <functional>
#include <queue>
#include <mutex>

#include "EventLoop.h"


namespace uv
{

using AsyncCallback = std::function<void()>;
class Async  : public std::enable_shared_from_this<Async>
{
public:
    

    Async(EventLoop* loop);
    virtual ~Async();

    void runInThisLoop(AsyncCallback callback);
    void process();

    void close(AsyncCallback callback);
private:
    std::mutex mutex_;
    uv_async_t* handle_;
    std::queue<AsyncCallback> callbacks_;


    static void Callback(uv_async_t* handle);

};


using AsyncPtr = std::shared_ptr<Async>;

}
#endif
