#include "TimeUtils.h"

#include <chrono>
#include "battle_interface.h"

unsigned long long GetTickCount()
{
    using namespace std::chrono;
    return duration_cast<microseconds>(steady_clock::now().time_since_epoch()).count();
}

void TimeAction(const std::string& message, std::function<void()> &runAction)
{
    const auto startTime = GetTickCount();
    runAction();
    const auto endTime = GetTickCount();
    auto delta = endTime - startTime;
    Dialog::Message("", message + ": " + to_string(delta), Font::BIG, Dialog::YES);
}