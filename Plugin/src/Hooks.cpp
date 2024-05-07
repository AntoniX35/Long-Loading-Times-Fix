#include "Hooks.h"

PerfCounter PerfCounter::m_Instance;
long long PerfCounter::perf_freq;
float PerfCounter::perf_freqf;

bool limit, disableVSync, vsync;
long long timing, CurrentFPS, FPSui, loadingFPSmax;
int* ipresentinterval;
float ingamefps, loadingscreenfps;

namespace LongLoadingTimesFix
{
	constexpr std::uint8_t NOP3[] = { 0x0F, 0x1F, 0x00 };
	constexpr std::uint8_t NOP4[] = { 0x0F, 0x1F, 0x40, 0x00 };
	constexpr std::uint8_t NOP6[] = { 0x66, 0x0F, 0x1F, 0x44, 0x00, 0x00 };
	constexpr std::uint8_t bytes2[] = { 0x00 };	
	uint32_t maxrefreshrate = 10000;
	constexpr std::uint8_t bytes4[] = { 0xEB };
	
	void LimiterFunc() {
		if (CurrentFPS > 0) {
			while (PerfCounter::deltal(timing, ::_Query_perf_counter())
				< CurrentFPS)
			{
				::Sleep(0);
			}
		}
		timing = ::_Query_perf_counter();
	}
		
	void Install()
	{
		const auto ui = RE::UI::GetSingleton();
		ui->GetEventSource<RE::MenuOpenCloseEvent>()->RegisterSink(new MenuWatcher());
		if (*Config::Limiter::iInGameFPS > 0) {
			ingamefps = 1.0f / *Config::Limiter::iInGameFPS;
		}
		if (*Config::Limiter::iLoadingScreenFPS > 0) {
			loadingscreenfps = 1.0f / *Config::Limiter::iLoadingScreenFPS;
		}
		if (*Config::Limiter::bDisableVSyncWhileLoading) {
			disableVSync = true;
		}
		if (*Config::LoadingScreens::bDisableAnimation) {
			disableVSync = false;
			*Config::Limiter::iLoadingScreenFPS = 0;
		}
		REL::Relocation<std::uintptr_t> FPSLimiterAddressNG{ REL::ID(2276834), 0xE};
		REL::Relocation<std::uintptr_t> QuickLoadAddressNG{ REL::ID(2275102), 0x536};
		REL::Relocation<std::uintptr_t> TimeoutFix1AddressNG{ REL::ID(2229488), 0x267};
		REL::Relocation<std::uintptr_t> TimeoutFix2AddressNG{ REL::ID(2275102), 0x341};
		REL::Relocation<std::uintptr_t> LoadingFix1AddressNG{ REL::ID(2276834), 0x36};
		REL::Relocation<std::uintptr_t> BethesdaVsyncAddressNG{ REL::ID(2277018), 0x332};
		REL::Relocation<std::uintptr_t> BethesdaFPSCap1AddressNG{ REL::ID(2228907), 0xBC7}; 
		REL::Relocation<std::uintptr_t> BethesdaFPSCap2AddressNG{ REL::ID(2228907), 0xBD0};
		REL::Relocation<std::uintptr_t> VsyncAddressNG{ REL::ID(2704527), 0x40};
		REL::Relocation<std::uintptr_t> Only3DAddressNG{ REL::ID(2249217), 0x116};
		REL::Relocation<std::uintptr_t> DisableAddressNG{ REL::ID(2227631), 0x223};
		REL::Relocation<std::uintptr_t> OnlyBlackLoadingAddressNG{ REL::ID(2249232), 0x15};
		
		REL::Relocation<std::uintptr_t> LoadingMenuSpeedHookAddressNG{ REL::ID(2248711), 0x38};
		REL::Relocation<float*> FrametimeAddressNG{ REL::ID(2703181) };
		
		if (*Config::Limiter::iInGameFPS > 0 || *Config::Limiter::iLoadingScreenFPS > 0) {
			limit = true;
			timing = _Query_perf_counter();
			auto& trampoline = F4SE::GetTrampoline();
			trampoline.write_call<5>(FPSLimiterAddressNG.address(), &LimiterFunc);
			CurrentFPS = FPSui = static_cast<long long>(ingamefps * 1000000.0);
			loadingFPSmax = static_cast<long long>(loadingscreenfps * 1000000.0);
		}
		ipresentinterval = reinterpret_cast<int*>(VsyncAddressNG.address());
		REL::safe_write(BethesdaVsyncAddressNG.address(), &NOP4, sizeof(NOP4));
		REL::safe_write(BethesdaFPSCap1AddressNG.address(), &maxrefreshrate, sizeof(maxrefreshrate));
		REL::safe_write(BethesdaFPSCap2AddressNG.address(), &maxrefreshrate, sizeof(maxrefreshrate));
		
		if (*Config::Limiter::bEnableVSync) {
			*ipresentinterval = 1;
			vsync = true;
		}
		else {
			*ipresentinterval = 0;
		}
		if (*Config::Timeout::bTimeoutFix1) {
			constexpr std::uint8_t bytes1[] = { 0xEB, 0x39 };
			REL::safe_write(TimeoutFix1AddressNG.address(), &bytes1, sizeof(bytes1));
		}
		if (*Config::Timeout::bTimeoutFix2) {
			REL::safe_write(TimeoutFix2AddressNG.address(), &NOP4, sizeof(NOP4));
		}
		if (*Config::Fixes::bLoadingFix) {		
			REL::safe_write(LoadingFix1AddressNG.address(), &bytes2, sizeof(bytes2));
		}
		if (*Config::LoadingScreens::bOnlyBlack) {
			constexpr std::uint8_t bytes3[] = { 0xE9, 0xFC, 0x01, 0x00, 0x00, 0x90 };
			REL::safe_write(OnlyBlackLoadingAddressNG.address(), &bytes3, sizeof(bytes3));
		}
		if (*Config::LoadingScreens::bOnly3D) {
			REL::safe_write(Only3DAddressNG.address(), &bytes4, sizeof(bytes4));
		}
		if (*Config::LoadingScreens::bDisableAnimation) {			
			constexpr std::uint8_t NOP5[] = { 0x0F, 0x1F, 0x44, 0x00, 0x00 };
			REL::safe_write(DisableAddressNG.address(), &NOP5, sizeof(NOP5));
		}
		if (*Config::Fixes::bQuickLoad) {
			REL::safe_write(QuickLoadAddressNG.address(), &bytes4, sizeof(bytes4));
		}
		if (*Config::LoadingScreens::fPostloadingMenuSpeed != 1.0f) {
			float menuSpeed = *Config::LoadingScreens::fPostloadingMenuSpeed;
			{
				struct Patch_Code : Xbyak::CodeGenerator
				{
					Patch_Code(std::uintptr_t a_hookTarget, std::uintptr_t a_frameTimer, float* a_value)
					{
						Xbyak::Label retnLabel;
						Xbyak::Label valueLabel;
						Xbyak::Label timerLabel;

						movss(xmm0, dword[rip + valueLabel]);
						mov(rcx, ptr[rip + timerLabel]);
						mulss(xmm0, dword[rcx]);

						jmp(ptr[rip + retnLabel]);

						L(valueLabel);
						db(reinterpret_cast<Xbyak::uint8*>(a_value), sizeof(float));

						L(timerLabel);
						dq(a_frameTimer);

						L(retnLabel);
						dq(a_hookTarget + 0x8);
					}
				};
				
				Patch_Code patch(LoadingMenuSpeedHookAddressNG.address(), FrametimeAddressNG.address(), &menuSpeed);
				patch.ready();

				auto& trampoline = F4SE::GetTrampoline();
				trampoline.write_branch<5>(
					LoadingMenuSpeedHookAddressNG.address(),
					trampoline.allocate(patch));
				
				REL::safe_write(LoadingMenuSpeedHookAddressNG.address() + 0x5, &NOP3, sizeof(NOP3));
			}
		}
	}		
}

RE::BSEventNotifyControl MenuWatcher::ProcessEvent(const RE::MenuOpenCloseEvent& evn, RE::BSTEventSource<RE::MenuOpenCloseEvent>*) {
	
	if (evn.menuName == RE::BSFixedString("LoadingMenu")) {
		if (evn.opening) {
			if (disableVSync) {
				*ipresentinterval = 0;
			}
			if (limit) {
				CurrentFPS = loadingFPSmax;
			}
		}
		else {
			if (disableVSync) {
				if (!vsync) {
					*ipresentinterval = 0;
				}
				else {
					*ipresentinterval = 1;
				}
			}
			if (limit) {
				CurrentFPS = FPSui;
			}
		}
	}
	return RE::BSEventNotifyControl::kContinue;
}