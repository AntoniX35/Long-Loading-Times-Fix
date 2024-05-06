#pragma once

class Config
{
public:
	class Fixes
	{
	public:
		inline static DKUtil::Alias::Boolean bLoadingFix{ "bLoadingFix", "Fixes" };
		inline static DKUtil::Alias::Boolean bQuickLoad{ "bQuickLoad", "Fixes" };
	};
	
	class Limiter
	{
	public:
		inline static DKUtil::Alias::Boolean bEnableVSync{ "bEnableVSync", "Limiter" };
		inline static DKUtil::Alias::Boolean bDisableVSyncWhileLoading{ "bDisableVSyncWhileLoading", "Limiter" };
		inline static DKUtil::Alias::Integer iInGameFPS{ "iInGameFPS", "Limiter" };
		inline static DKUtil::Alias::Integer iLoadingScreenFPS{ "iLoadingScreenFPS", "Limiter" };
	};
	
	class LoadingScreens
	{
	public:
		inline static DKUtil::Alias::Boolean bOnlyBlack{ "bOnlyBlack", "LoadingScreens" };
		inline static DKUtil::Alias::Boolean bOnly3D{ "bOnly3D", "LoadingScreens" };
		inline static DKUtil::Alias::Boolean bDisableAnimation{ "bDisableAnimation", "LoadingScreens" };
		inline static DKUtil::Alias::Double fPostloadingMenuSpeed{ "fPostloadingMenuSpeed", "LoadingScreens" };
	};
	
	class Timeout
	{
	public:
		inline static DKUtil::Alias::Boolean bTimeoutFix1{ "bTimeoutFix1", "Timeout" };
		inline static DKUtil::Alias::Boolean bTimeoutFix2{ "bTimeoutFix2", "Timeout" };
	};

	static void Load()
	{
		static auto Config = COMPILE_PROXY("LongLoadingTimesFix.ini");
		Config.Bind(Fixes::bLoadingFix, true);
		Config.Bind(Fixes::bQuickLoad, true);
		Config.Bind(Limiter::bEnableVSync, true);
		Config.Bind(Limiter::bDisableVSyncWhileLoading, false);
		Config.Bind(Limiter::iInGameFPS, 0);
		Config.Bind(Limiter::iLoadingScreenFPS, 0);
		Config.Bind(LoadingScreens::bOnlyBlack, false);
		Config.Bind(LoadingScreens::bOnly3D, false);
		Config.Bind(LoadingScreens::bDisableAnimation, false);
		Config.Bind(LoadingScreens::fPostloadingMenuSpeed, 1.0f);
		Config.Bind(Timeout::bTimeoutFix1, false);
		Config.Bind(Timeout::bTimeoutFix2, false);
		Config.Load();
	}
};