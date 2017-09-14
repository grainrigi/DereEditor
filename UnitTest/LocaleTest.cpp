#include "stdafx.h"
#include "CppUnitTest.h"

#include "DereEditor/Locale.h"
#include "LocaleTable.h"
#include <memory>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Locale;

namespace UnitTest
{
	namespace {
		Str_ correct_ja = LR"(ja
UI_BUTTON_OK=OK
UI_BUTTON_NEW=新規作成
UI_BUTTON_OPEN=開く
)";
		Str_ correct_en = LR"(en
UI_BUTTON_OK=OK
UI_BUTTON_NEW=New
UI_BUTTON_OPEN=Open
)";
		Str_ wrong_format = LR"(
UI_BUTTON_OK=OK
UI_BUTTON_NEW=New
UI_BUTTON_OPEN=Open
)";
		Str_ wrong_count = LR"(ja
UI_BUTTON_OK=OK
UI_BUTTON_OPEN=開く
)";
	}

	TEST_CLASS(LocaleManagerTest)
	{
	public:

		//Check if loading and getting locale data works correctly
		TEST_METHOD(LocaleManager_Load_Get_Test)
		{
			LocaleManager loc;

			loc.LoadLocaleFromString(correct_ja);

			//test default loading
			Assert::AreEqual(L"OK", loc.GetLocaleString(UI_BUTTON_OK).c_str());
			Assert::AreEqual(L"新規作成", loc.GetLocaleString(UI_BUTTON_NEW).c_str());
			Assert::AreEqual(L"開く", loc.GetLocaleString(UI_BUTTON_OPEN).c_str());
		}

		//Check if loading multiple locales works correctly
		TEST_METHOD(LocaleManager_MultipleLoadTest)
		{
			LocaleManager loc;

			loc.LoadLocaleFromString(correct_ja);
			loc.LoadLocaleFromString(correct_en);

			//test default loading ("ja" is expected)
			Assert::AreEqual(L"OK", loc.GetLocaleString(UI_BUTTON_OK).c_str());
			Assert::AreEqual(L"新規作成", loc.GetLocaleString(UI_BUTTON_NEW).c_str());
			Assert::AreEqual(L"開く", loc.GetLocaleString(UI_BUTTON_OPEN).c_str());

			//change the locale to "en"
			loc.ChangeLocale(L"en");

			//test getting string ("en" is excepted)
			Assert::AreEqual(L"OK", loc.GetLocaleString(UI_BUTTON_OK).c_str());
			Assert::AreEqual(L"New", loc.GetLocaleString(UI_BUTTON_NEW).c_str());
			Assert::AreEqual(L"Open", loc.GetLocaleString(UI_BUTTON_OPEN).c_str());

			//change the locale to "ja"
			loc.ChangeLocale(L"ja");

			//test getting string ("ja" is expected)
			Assert::AreEqual(L"OK", loc.GetLocaleString(UI_BUTTON_OK).c_str());
			Assert::AreEqual(L"新規作成", loc.GetLocaleString(UI_BUTTON_NEW).c_str());
			Assert::AreEqual(L"開く", loc.GetLocaleString(UI_BUTTON_OPEN).c_str());
		}

		//Check if changing to non existing locale fails
		TEST_METHOD(LocaleManager_ChangeLocaleTest_Negative)
		{
			LocaleManager loc;

			loc.LoadLocaleFromString(correct_ja);
			loc.LoadLocaleFromString(correct_en);

			Assert::ExpectException<LocaleNotFoundException>([&]() {loc.ChangeLocale(L"nonexisting"); });
		}

		//Check if getting string from an empty manager fails
		TEST_METHOD(LocaleManager_EmptyTest_Negative)
		{
			LocaleManager loc;

			Assert::ExpectException<LocaleException>([&]() {loc.GetLocaleString(UI_BUTTON_OK); });
		}

	};

	TEST_CLASS(StringManagerTest)
	{
		static std::unique_ptr<LocaleStringManager> m_sm_ja;
		static std::unique_ptr<LocaleStringManager> m_sm_en;

	public:
		//Check if the initialization works correctly
		TEST_CLASS_INITIALIZE(StringManger_Initialize)
		{
			//Check if the initialization succeeds with a correct data
			m_sm_ja.reset(new LocaleStringManager(correct_ja));
			m_sm_en.reset(new LocaleStringManager(correct_en));

		}

		//Resource releasing test
		TEST_CLASS_CLEANUP(LocaleStringManager_Cleanup)
		{
			m_sm_ja.reset(nullptr);
			m_sm_en.reset(nullptr);
		}



		//Check if the initialization fails with a wrong data
		TEST_METHOD(LocaleStringManager_InitializeTest_Negative)
		{
			Assert::ExpectException<LocaleParseException>([&]() {LocaleStringManager a(wrong_format); });
			Assert::ExpectException<LocaleParseException>([&]() {LocaleStringManager a(wrong_count); });
		}


		//Check if the metadata has been loaded correctly
		TEST_METHOD(LocaleStringManager_MetadataTest)
		{
			Assert::AreEqual(3, m_sm_ja->GetCount());
			Assert::AreEqual(Str_(L"ja"), m_sm_ja->GetLocaleName());
			Assert::AreEqual(3, m_sm_en->GetCount());
			Assert::AreEqual(Str_(L"en"), m_sm_en->GetLocaleName());
		}

		//Check if the string table has been loaded correctly
		TEST_METHOD(LocaleStringManager_GetStringTest)
		{
			Assert::AreEqual(Str_(L"OK"), m_sm_ja->GetLocaleString(UI_BUTTON_OK));
			Assert::AreEqual(Str_(L"新規作成"), m_sm_ja->GetLocaleString(UI_BUTTON_NEW));
			Assert::AreEqual(Str_(L"開く"), m_sm_ja->GetLocaleString(UI_BUTTON_OPEN));
		}

		//Check if the SM throws an exception on requesting a non existing string
		TEST_METHOD(LocaleStringManager_GetStringTest_Negative)
		{
			Assert::ExpectException<LocaleNotFoundException>([&]() { m_sm_ja->GetLocaleString(9999); });
			Assert::ExpectException<LocaleNotFoundException>([&]() { m_sm_en->GetLocaleString(9999); });
		}
	};

	//StringManagerTest static members
	std::unique_ptr<LocaleStringManager> StringManagerTest::m_sm_ja;
	std::unique_ptr<LocaleStringManager> StringManagerTest::m_sm_en;

	TEST_CLASS(LocaleTableTest)
	{
	public:
		TEST_METHOD(LocaleTable_ValuesTest)
		{
			Assert::AreEqual((size_t)3, loc_dic.size());
			Assert::AreEqual((uint32_t)UI_BUTTON_OK, loc_dic[L"UI_BUTTON_OK"]);
			Assert::AreEqual((uint32_t)UI_BUTTON_NEW, loc_dic[L"UI_BUTTON_NEW"]);
			Assert::AreEqual((uint32_t)UI_BUTTON_OPEN, loc_dic[L"UI_BUTTON_OPEN"]);
		}
	};
}

void Main(void)
{}