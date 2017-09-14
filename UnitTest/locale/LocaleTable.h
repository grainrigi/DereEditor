#pragma once

#include <DereEditor/locale/Locale.h>
#include <DereEditor/locale/LocaleTableHead.h>

namespace Locale
{
	typedef std::unordered_map<Str_, uint32_t> LocaleStringDictionary;
	LOCALE_REG_START()
	LOCALE_REG(UI_BUTTON_OK, 1)
	LOCALE_REG(UI_BUTTON_NEW, 2)
	LOCALE_REG(UI_BUTTON_OPEN, 3)
	LOCALE_REG_END()
}