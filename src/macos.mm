#include "macos.h"
#include <AppKit/AppKit.h>

bool applicationIsInDarkMode()
{
	if (__builtin_available(macOS 10.14, *))
	{
		auto appearance = [NSApp.effectiveAppearance bestMatchFromAppearancesWithNames:
				@[ NSAppearanceNameAqua, NSAppearanceNameDarkAqua ]];
		return [appearance isEqualToString:NSAppearanceNameDarkAqua];
	}
	return false;
}

Macos::Macos()
{

}

bool Macos::isDarkMode()
{
	return applicationIsInDarkMode();
}
