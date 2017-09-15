#pragma once

#ifdef LOCALE_CPP
#define LOCALE_REG_START() \
	LocaleStringDictionary loc_dic = {
#elif defined LOCALE_CPP_DEFV
#define LOCALE_REG_START() \
	DefaultValueDictionary def_dic = {
#else
#define LOCALE_REG_START() \
	extern LocaleStringDictionary loc_dic; \
	extern DefaultValueDictionary def_dic; \
	enum {
#endif

#ifdef LOCALE_CPP
#define LOCALE_REG(arg1,arg2,arg3) \
	std::make_pair(L#arg1, arg2),
#elif defined LOCALE_CPP_DEFV
#define LOCALE_REG(arg1,arg2,arg3) \
	std::make_pair(arg2,arg3),
#else
#define LOCALE_REG(x,y,z) \
	x = y,
#endif

#ifdef LOCALE_CPP
#define LOCALE_REG_END() \
	};
#elif defined LOCALE_CPP_DEFV
#define LOCALE_REG_END() \
	};
#else
#define LOCALE_REG_END() \
	};
#endif

namespace Locale
{
	typedef std::unordered_map<Str_, uint32_t> LocaleStringDictionary;
	typedef std::unordered_map<uint32_t, Str_> DefaultValueDictionary;
}