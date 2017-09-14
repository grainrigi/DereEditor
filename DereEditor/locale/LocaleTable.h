#pragma once

#include <unordered_map>
#include "Locale.h"

#include "LocaleTableHead.h"

namespace Locale
{
	typedef std::unordered_map<Str_, uint32_t> LocaleStringDictionary;

	//Register the string id
	LOCALE_REG_START()
	LOCALE_REG(UI_BUTTON_OK, 1)
	LOCALE_REG(UI_BUTTON_NEWFILE, 2)
	LOCALE_REG(UI_BUTTON_OPEN, 3)
	LOCALE_REG_END()
}


/*
このヘッダでは、マクロを利用してロケール用定数を生成します。
LocaleTable.cpp内では、文字列名から文字列IDを引くことのできるunordered_mapを生成します。
それ以外では、文字列名と文字列IDの対応付けを表すenumを生成します。
※わざわざenumを生成する理由
プログラム内からの文字列取得の際に文字列名をキーとすることもできますが、
その場合存在しない文字列名を指定しているかどうかが分からなくなってしまうからです。
*/