#include "TimeUtils.h"

#include <chrono>
#include "battle_interface.h"

unsigned long long GetTickCount()
{
	using namespace std::chrono;
	return duration_cast<microseconds>(steady_clock::now().time_since_epoch()).count();
}

void TimeAction(std::string message, std::function<void()> runAction)
{
	auto startTime = GetTickCount(); auto endTime = GetTickCount();
	runAction();
	auto delta = endTime - startTime;
	Dialog::Message("", message + ": " + to_string(delta), Font::BIG, Dialog::YES);
}