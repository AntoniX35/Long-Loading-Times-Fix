#pragma once

#include "Settings.h"
#include "PerfCounter.h"

namespace LongLoadingTimesFix
{		
	void LimterFunc();
	
	void Install();
}
	
class MenuWatcher : public RE::BSTEventSink<RE::MenuOpenCloseEvent>
{
	virtual RE::BSEventNotifyControl ProcessEvent(const RE::MenuOpenCloseEvent& evn, RE::BSTEventSource<RE::MenuOpenCloseEvent>*) override;
};