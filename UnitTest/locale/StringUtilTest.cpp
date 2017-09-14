#include "CppUnitTest.h"

#include "DereEditor/locale/StringUtil.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace StringUtil;

namespace UnitTest
{
	Str_ testdata_lf = LR"(testdata
testing testing

end of data)";

	Str_ testdata_nonsep = L"testdatawithoutsep";


	TEST_CLASS(StringTokenizerTest)
	{
	public:
		TEST_METHOD(StringTokenizer_GetSetTest)
		{
			StringTokenizer tok;
			
			//Set true
			tok.SetIfIncludeSeparator(false);
			Assert::AreEqual(false, tok.GetIfIncludeSeparator());

			//Set false
			tok.SetIfIncludeSeparator(true);
			Assert::AreEqual(true, tok.GetIfIncludeSeparator());
		}

		TEST_METHOD(StringTokenizer_NonIncludeSeparatorTest)
		{
			StringTokenizer tok;

			tok.LoadString(testdata_lf);
			tok.SetSeparator(L'\n');
			tok.SetIfIncludeSeparator(false);

			Assert::AreEqual(false, tok.isEOS());

			//Line reading test
			Assert::AreEqual(L"testdata", tok.Tokenize().c_str());
			Assert::AreEqual(false, tok.isEOS());

			Assert::AreEqual(L"testing testing", tok.Tokenize().c_str());
			Assert::AreEqual(false, tok.isEOS());

			Assert::AreEqual(L"", tok.Tokenize().c_str());
			Assert::AreEqual(false, tok.isEOS());

			Assert::AreEqual(L"end of data", tok.Tokenize().c_str());
			Assert::AreEqual(true, tok.isEOS());

			//Empty emission
			Assert::AreEqual(L"", tok.Tokenize().c_str());
		}

		TEST_METHOD(StringTokenizer_IncludeSeparatorTest)
		{
			StringTokenizer tok;

			tok.LoadString(testdata_lf);
			tok.SetSeparator(L'\n');
			tok.SetIfIncludeSeparator(true);

			Assert::AreEqual(false, tok.isEOS());

			//Line reading test
			Assert::AreEqual(L"testdata\n", tok.Tokenize().c_str());
			Assert::AreEqual(false, tok.isEOS());

			Assert::AreEqual(L"testing testing\n", tok.Tokenize().c_str());
			Assert::AreEqual(false, tok.isEOS());

			Assert::AreEqual(L"\n", tok.Tokenize().c_str());
			Assert::AreEqual(false, tok.isEOS());

			Assert::AreEqual(L"end of data", tok.Tokenize().c_str());
			Assert::AreEqual(true, tok.isEOS());

			//Empty emission
			Assert::AreEqual(L"", tok.Tokenize().c_str());
		}

		TEST_METHOD(StringTokenizer_ModeChangeTest)
		{
			StringTokenizer tok;

			tok.LoadString(testdata_lf);
			tok.SetSeparator(L'\n');
			tok.SetIfIncludeSeparator(false);

			Assert::AreEqual(false, tok.isEOS());

			//Line reading test
			Assert::AreEqual(L"testdata", tok.Tokenize().c_str());
			Assert::AreEqual(false, tok.isEOS());

			//Change the mode
			tok.SetIfIncludeSeparator(true);

			Assert::AreEqual(L"testing testing\n", tok.Tokenize().c_str());
			Assert::AreEqual(false, tok.isEOS());

			Assert::AreEqual(L"\n", tok.Tokenize().c_str());
			Assert::AreEqual(false, tok.isEOS());

			//Change the mode
			tok.SetIfIncludeSeparator(false);

			Assert::AreEqual(L"end of data", tok.Tokenize().c_str());
			Assert::AreEqual(true, tok.isEOS());

			//Empty emission
			Assert::AreEqual(L"", tok.Tokenize().c_str());
		}

		TEST_METHOD(StringTokenizer_NonSeparatorDataTest)
		{
			StringTokenizer tok_i, tok_n;

			//Include separator test
			tok_i.LoadString(testdata_nonsep);
			tok_i.SetSeparator(L'\n');
			tok_i.SetIfIncludeSeparator(true);

			Assert::AreEqual(false, tok_i.isEOS());

			Assert::AreEqual(L"testdatawithoutsep", tok_i.Tokenize().c_str());
			Assert::AreEqual(true, tok_i.isEOS());

			//Empty emission
			Assert::AreEqual(L"", tok_i.Tokenize().c_str());

			//NonInclude separator test
			tok_n.LoadString(testdata_nonsep);
			tok_n.SetSeparator(L'\n');
			tok_n.SetIfIncludeSeparator(true);

			Assert::AreEqual(false, tok_n.isEOS());

			Assert::AreEqual(L"testdatawithoutsep", tok_n.Tokenize().c_str());
			Assert::AreEqual(true, tok_n.isEOS());

			//Empty emission
			Assert::AreEqual(L"", tok_n.Tokenize().c_str());
		}

		TEST_METHOD(StringTokenizer_ResetTest)
		{
			StringTokenizer tok;

			tok.LoadString(testdata_lf);
			tok.SetSeparator(L'\n');
			tok.SetIfIncludeSeparator(false);

			Assert::AreEqual(false, tok.isEOS());

			//Line reading test
			Assert::AreEqual(L"testdata", tok.Tokenize().c_str());
			Assert::AreEqual(false, tok.isEOS());

			Assert::AreEqual(L"testing testing", tok.Tokenize().c_str());
			Assert::AreEqual(false, tok.isEOS());

			Assert::AreEqual(L"", tok.Tokenize().c_str());
			Assert::AreEqual(false, tok.isEOS());

			Assert::AreEqual(L"end of data", tok.Tokenize().c_str());
			Assert::AreEqual(true, tok.isEOS());

			//Empty emission
			Assert::AreEqual(L"", tok.Tokenize().c_str());

			//Reset
			tok.ResetTokenizer();
			Assert::AreEqual(false, tok.isEOS());

			//Line reading test
			Assert::AreEqual(L"testdata", tok.Tokenize().c_str());
			Assert::AreEqual(false, tok.isEOS());

			Assert::AreEqual(L"testing testing", tok.Tokenize().c_str());
			Assert::AreEqual(false, tok.isEOS());

			Assert::AreEqual(L"", tok.Tokenize().c_str());
			Assert::AreEqual(false, tok.isEOS());

			Assert::AreEqual(L"end of data", tok.Tokenize().c_str());
			Assert::AreEqual(true, tok.isEOS());

			//Empty emission
			Assert::AreEqual(L"", tok.Tokenize().c_str());
		}

		TEST_METHOD(StringTokenizer_SetSeparatorTest)
		{
			StringTokenizer tok;

			tok.LoadString(testdata_lf);
			tok.SetSeparator(L'\n');
			tok.SetIfIncludeSeparator(false);

			//Line reading test
			Assert::AreEqual(L"testdata", tok.Tokenize().c_str());
			Assert::AreEqual(false, tok.isEOS());

			//Change the separator
			tok.SetSeparator(L' ');

			//Tokenize test
			Assert::AreEqual(L"testing", tok.Tokenize().c_str());
			Assert::AreEqual(false, tok.isEOS());

			Assert::AreEqual(L"testing\n\nend", tok.Tokenize().c_str());
			Assert::AreEqual(false, tok.isEOS());

			//Change the separator
			tok.SetSeparator(L'\n');

			Assert::AreEqual(L"of data", tok.Tokenize().c_str());
			Assert::AreEqual(true, tok.isEOS());

			//Empty emission
			Assert::AreEqual(L"", tok.Tokenize().c_str());
		}
	};
}