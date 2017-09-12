#pragma once

#ifdef LOCALE_CPP
#define LOCALE_REG_START() \
	LocaleStringDictionary loc_dic = {
#else
#define LOCALE_REG_START() \
	extern LocaleStringDictionary loc_dic; \
	enum {
#endif

#ifdef LOCALE_CPP
#define LOCALE_REG(arg1,arg2) \
	std::make_pair(L#arg1, arg2),
#else
#define LOCALE_REG(x,y) \
	x = y,
#endif

#ifdef LOCALE_CPP
#define LOCALE_REG_END() \
	};
#else
#define LOCALE_REG_END() \
	};
#endif