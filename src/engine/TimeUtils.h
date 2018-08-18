#pragma once

#include <string>
#include <functional>

unsigned long long GetTickCount();

void TimeAction(const std::string& message, std::function<void()>& runAction);
