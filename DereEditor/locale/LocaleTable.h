#pragma once

#include <unordered_map>
#include "Locale.h"

#include "LocaleTableHead.h"

namespace Locale
{
	//Register the string id
	LOCALE_REG_START()
		//UI String
		LOCALE_REG(UI_OK, 0, L"OK")
		LOCALE_REG(UI_CANCEL, 1, L"キャンセル")
		LOCALE_REG(UI_NEW, 2, L"新規")
		LOCALE_REG(UI_OPEN, 3, L"開く")
		LOCALE_REG(UI_SAVE, 4, L"保存")
		LOCALE_REG(UI_FILE, 5, L"[ファイル]")
		LOCALE_REG(UI_OVERWRITE, 6, L"上書き保存")
		LOCALE_REG(UI_SAVEAS, 7, L"名前を付けて保存")
		LOCALE_REG(UI_EDIT_HEADER, 8, L"ヘッダー編集")
		LOCALE_REG(UI_PLAY_SIMULATOR, 9, L"シミュレーターで再生")
		LOCALE_REG(UI_GRID, 10, L"[グリッド]")
		LOCALE_REG(UI_ZOOM, 11, L"ズーム")
		LOCALE_REG(UI_DIVISION_NUMBER, 12, L"分割数")
		LOCALE_REG(UI_SHOW_INFORMATION, 13, L"情報を表示する")
		LOCALE_REG(UI_NOTE, 14, L"[ノート]")
		LOCALE_REG(UI_CHANNEL, 15, L"チャンネル")
		LOCALE_REG(UI_EXTENSION_NOTE, 16, L"[拡張ノート]")
		LOCALE_REG(UI_CHANGE_TEMPO, 17, L"テンポ変更")
		LOCALE_REG(UI_MEASURE, 18, L"[小節]")
		LOCALE_REG(UI_INSERT, 19, L"挿入")
		LOCALE_REG(UI_DELETE, 20, L"削除")
		LOCALE_REG(UI_CHANGE_MEASURE, 21, L"拍子変更")
		LOCALE_REG(UI_CHANGE, 22, L"変更")
		//Error String
		LOCALE_REG(ERR_MEASURE_CANNOT_ADD, 10000, L"これ以上小節を追加出来ません。")
		LOCALE_REG(ERR_MEASURE_CANNOT_DEL_ALL, 10001, L"小節を全て削除することは出来ません。")
		LOCALE_REG(ERR_ILLFORM_4_4, 10002, L"4/4形式で入力してください")
		LOCALE_REG(ERR_MEASURE_SIZE_BELOW_0, 10003, L"小節の長さは0以下に設定出来ません。")
		LOCALE_REG(ERR_DENOM_LIMITATION, 10004, L"分母は4,8,16,32,64のみが使用出来ます。")
		LOCALE_REG(ERR_NO_SIMULATOR, 10005, L"シミュレーターが存在しません。")

		//Other Message String
		LOCALE_REG(MSG_DEL_MEASURE_CONFIRM, 20000, L"ノートが含まれています。小節を削除しますか？")
		LOCALE_REG(MSG_SAVE_CONFIRM, 20001, L"譜面が変更されています。保存しますか？")
		LOCALE_REG(MSG_OPENFILE_DELESTE_FILTER, 20002, L"Deleste ファイル (*.txt)")
		LOCALE_REG(MSG_OMIT_ERROR_OVER_21, 20003, L"(21個以上のエラーは省略しています。)")
		LOCALE_REG(MSG_IGNORE_ERROR_CONFIRM, 20004, L"エラーを無視して読み込みますか？")

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