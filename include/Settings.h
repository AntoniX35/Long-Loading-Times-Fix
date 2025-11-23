#pragma once

class Settings : public REX::Singleton<Settings>
{
public:
	void Load()
	{
		constexpr auto path = L"Data/F4SE/Plugins/LongLoadingTimesFix.ini";

		CSimpleIniA ini;
		ini.SetUnicode();

		ini.LoadFile(path);

		ini::get_value(ini, bLoadingFix, "Fixes", "bLoadingFix",
		  "## Fixes long loading times with black loading screens and when moving between locations using the elevator.\n#\n#");
		ini::get_value(ini, bQuickLoad, "Fixes", "bQuickLoad",
		  "## Speed up the loading of recently visited locations.\n#\n#");
		  
		ini::get_value(ini, bEnableVSync, "Limiter", "bEnableVSync",
		  "## Allows you to enable or disable vertical synchronization (VSync). Enabling ForceVSync in ENB will override this setting.\n#\n#  This setting replaces \"iPresentInterval\" in Fallout4Prefs.ini.\n#\n#");
		ini::get_value(ini, bDisableVSyncWhileLoading, "Limiter", "bDisableVSyncWhileLoading",
		  "## Disables VSync on loading screens to reduce loading times.\n#\n#  ForceVSync=true in ENB will override this setting (so be sure to set ForceVSync=false in enblocal.ini if you want to use this feature).\n#\n#");
		ini::get_value(ini, iInGameFPS, "Limiter", "iInGameFPS",
		  "## FPS limit in the game.\n#\n#");
		ini::get_value(ini, iLoadingScreenFPS, "Limiter", "iLoadingScreenFPS",
		  "## FPS limit on loading screens.\n#\n#  It is necessary to reduce the GPU load, since VSync will be disabled.\n#  You can set the maximum FPS your monitor supports.\n#\n#");

		ini::get_value(ini, bOnlyBlack, "LoadingScreens", "bOnlyBlack",
		  "## Replaces loading screens with a rotating 3D model with simple ones with a black background.\n#\n#");
		ini::get_value(ini, bOnly3D, "LoadingScreens", "bOnly3D",
		  "## Replaces loading screens with a black background with screens with a rotating 3D model.\n#\n#");
		ini::get_value(ini, bDisableAnimation, "LoadingScreens", "bDisableAnimation",
		  "## Disables all animations on loading screens. The display of overlays will be frozen.\n#\n#");
		ini::get_value(ini, fPostloadingMenuSpeed, "LoadingScreens", "fPostloadingMenuSpeed",
		  "## Allows you to reduce the time it takes to smoothly transition to and from the loading screen.\n#\n#  Also useful for fast loading, especially when loading a save created during a battle.\n#\n#");

		ini::get_value(ini, bTimeoutFix1, "Timeout", "bTimeoutFix1",
		  "## Don't wait for textures to fully load.\n#\n#  WARNING: Some textures may stuck at low resolution when using texture mods. Alternatively, you can use ENB with the settings:\n#    ForceVideoMemorySize=true\n#    VideoMemorySizeMb=12288\n#\n#");
		ini::get_value(ini, bTimeoutFix2, "Timeout", "bTimeoutFix2",
		  "## Don't wait for a full load of NPC faces.\n#\n#  WARINING: May cause brown faces (Rusty Face Bug) if face mods are installed.\n#  When using both fixes, there is no need to increase FPS on loading screens to reduce loading times.\n#\n#");

		(void)ini.SaveFile(path);
	}

	bool          bLoadingFix{ true };
	bool          bQuickLoad{ true };
	
	bool          bEnableVSync{ true };
	bool          bDisableVSyncWhileLoading{ true };
	
	bool          bOnlyBlack{ true };
	bool          bOnly3D{ true };
	bool          bDisableAnimation{ true };

	bool          bTimeoutFix1{ true };
	bool          bTimeoutFix2{ true };
	
	float          iInGameFPS{ 0.0f };
	float          iLoadingScreenFPS{ 60.0f };
	float          fPostloadingMenuSpeed{ 1.0f };
};