#include "PCH.h"
#include "Hooks.h"

bool limit, disableVSync, vsync;
long long timing1, timing2, CurrentFPS, FPSui, loadingFPSmax;
int* ipresentinterval;
float ingamefps, loadingscreenfps;

namespace LongLoadingTimesFix
{
	constexpr std::uint8_t NOP[] = { 0x90 };
	constexpr std::uint8_t NOP3[] = { 0x0F, 0x1F, 0x00 };
	constexpr std::uint8_t NOP4[] = { 0x0F, 0x1F, 0x40, 0x00 };
	constexpr std::uint8_t NOP6[] = { 0x66, 0x0F, 0x1F, 0x44, 0x00, 0x00 };
	constexpr std::uint8_t bytes2[] = { 0x00 };	
	uint32_t maxrefreshrate = 10000;
	constexpr std::uint8_t bytes4[] = { 0xEB };
	
	void LimiterFunc1() {
		if (loadingFPSmax > 0) {
			while (PerfCounter::deltal(timing1, ::_Query_perf_counter())
				< loadingFPSmax)
			{
				::Sleep(0);
			}
			timing1 = ::_Query_perf_counter();
		}
	}
	
	void LimiterFunc2() {
		if (CurrentFPS > 0) {
			while (PerfCounter::deltal(timing2, ::_Query_perf_counter())
				< CurrentFPS)
			{
				::Sleep(0);
			}
		}
		timing2 = ::_Query_perf_counter();
	}
		
	void Install()
	{
		const auto ui = RE::UI::GetSingleton();
		ui->GetEventSource<RE::MenuOpenCloseEvent>()->RegisterSink(new MenuWatcher());
		
		const auto settings = Settings::GetSingleton();
		
		if (settings->bDisableVSyncWhileLoading) {
			disableVSync = true;
		}
		if (settings->bDisableAnimation) {
			disableVSync = false;
			settings->iLoadingScreenFPS = 0;
		}
		REL::Relocation<std::uintptr_t> FPSLimiter1Address{ REL::ID(2227631), 0xE1};
		REL::Relocation<std::uintptr_t> FPSLimiter2Address{ REL::ID(2276834), 0xE};
		REL::Relocation<std::uintptr_t> QuickLoadAddress{ REL::ID(2275102), 0x536};
		REL::Relocation<std::uintptr_t> TimeoutFix1Address{ REL::ID(2229488), 0x267};
		REL::Relocation<std::uintptr_t> TimeoutFix2Address{ REL::ID(2275102), 0x341};
		REL::Relocation<std::uintptr_t> LoadingFix1Address{ REL::ID(2276834), 0x36};
		REL::Relocation<std::uintptr_t> BethesdaVsyncAddress{ REL::ID(4492363), 0x331};
		REL::Relocation<std::uintptr_t> BethesdaFPSCap1Address{ REL::ID(2228907), 0xC37}; 
		REL::Relocation<std::uintptr_t> BethesdaFPSCap2Address{ REL::ID(2228907), 0xC40};
		REL::Relocation<std::uintptr_t> VsyncAddress{ REL::ID(2704527), 0x40};
		REL::Relocation<std::uintptr_t> Only3DAddress{ REL::ID(2249217), 0x116};
		REL::Relocation<std::uintptr_t> DisableAddress{ REL::ID(2227631), 0x223};
		REL::Relocation<std::uintptr_t> OnlyBlackLoadingAddress{ REL::ID(2249232), 0x15};
		
		REL::Relocation<std::uintptr_t> LoadingMenuSpeedHookAddress{ REL::ID(2248711), 0x2E};
		REL::Relocation<float*> FrametimeAddress{ REL::ID(4803789), 0x21C };
		
		if (settings->iLoadingScreenFPS > 0) {
			timing1 = _Query_perf_counter();
			limit = true;
			loadingscreenfps = 1.0f / settings->iLoadingScreenFPS;
			loadingFPSmax = static_cast<long long>(loadingscreenfps * 1000000.0);
			auto& trampoline = F4SE::GetTrampoline();
			trampoline.write_call<5>(FPSLimiter1Address.address(), &LimiterFunc1);
			REL::safe_write(FPSLimiter1Address.address() + 0x5, &NOP, sizeof(NOP));
		
		}
		if (settings->iInGameFPS > 0) {
			timing2 = _Query_perf_counter();
			ingamefps = 1.0f / settings->iInGameFPS;
			auto& trampoline = F4SE::GetTrampoline();
			trampoline.write_call<5>(FPSLimiter2Address.address(), &LimiterFunc2);
			CurrentFPS = FPSui = static_cast<long long>(ingamefps * 1000000.0);
		}
		ipresentinterval = reinterpret_cast<int*>(VsyncAddress.address());
		REL::safe_write(BethesdaVsyncAddress.address(), &NOP4, sizeof(NOP4));
		REL::safe_write(BethesdaFPSCap1Address.address(), &maxrefreshrate, sizeof(maxrefreshrate));
		REL::safe_write(BethesdaFPSCap2Address.address(), &maxrefreshrate, sizeof(maxrefreshrate));
		if (settings->bEnableVSync) {
			*ipresentinterval = 1;
			vsync = true;
		}
		else {
			*ipresentinterval = 0;
		}
		if (settings->bTimeoutFix1) {
			constexpr std::uint8_t bytes1[] = { 0xEB, 0x39 };
			REL::safe_write(TimeoutFix1Address.address(), &bytes1, sizeof(bytes1));
		}
		if (settings->bTimeoutFix2) {
			REL::safe_write(TimeoutFix2Address.address(), &NOP4, sizeof(NOP4));
		}
		if (settings->bLoadingFix) {		
			REL::safe_write(LoadingFix1Address.address(), &bytes2, sizeof(bytes2));
		}
		if (settings->bOnlyBlack) {
			constexpr std::uint8_t bytes3[] = { 0xE9, 0xFC, 0x01, 0x00, 0x00, 0x90 };
			REL::safe_write(OnlyBlackLoadingAddress.address(), &bytes3, sizeof(bytes3));
		}
		if (settings->bOnly3D) {
			REL::safe_write(Only3DAddress.address(), &bytes4, sizeof(bytes4));
		}
		if (settings->bDisableAnimation) {			
			constexpr std::uint8_t NOP5[] = { 0x0F, 0x1F, 0x44, 0x00, 0x00 };
			REL::safe_write(DisableAddress.address(), &NOP5, sizeof(NOP5));
		}
		if (settings->bQuickLoad) {
			REL::safe_write(QuickLoadAddress.address(), &bytes4, sizeof(bytes4));
		}
		if (settings->fPostloadingMenuSpeed != 1.0f) {
			float menuSpeed = settings->fPostloadingMenuSpeed;
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
				
				Patch_Code patch(LoadingMenuSpeedHookAddress.address(), FrametimeAddress.address(), &menuSpeed);
				patch.ready();

				auto& trampoline = F4SE::GetTrampoline();
				trampoline.write_branch<5>(
					LoadingMenuSpeedHookAddress.address(),
					trampoline.allocate(patch));
				
				REL::safe_write(LoadingMenuSpeedHookAddress.address() + 0x5, &NOP3, sizeof(NOP3));
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
				CurrentFPS = 0;
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