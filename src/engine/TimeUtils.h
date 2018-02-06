#pragma once

#include <string>
#include <functional>

unsigned long long
GetTickCount();

void TimeAction(std::string message, std::function<void()> &runAction);
