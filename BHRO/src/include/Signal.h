#ifndef UV_SIGNAL_H
#define UV_SIGNAL_H


#include <map>
#include <functional>

#include "EventLoop.h"
#include "Utilities.h"

namespace uv
{

using SignalHandle = std::function<void(int)>;

class Signal
{
public:
    Signal(EventLoop* loop,int sig, SignalHandle handle = nullptr);
    void close(DefaultCallback callback);
    virtual ~Signal();

    void setHandle(SignalHandle handle);
    bool handle(int signum);

    void closeComplete();
    static void Ignore(int sig);
private:
    uv_signal_t* signal_;
    SignalHandle handle_;
    DefaultCallback closeCallback_;
    static void onSignal(uv_signal_t* handle, int signum);
};

}
#endif
