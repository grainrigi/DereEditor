#pragma once

#include <string>

namespace StringUtil
{
	typedef std::wstring Str_;

	class StringTokenizer
	{
	public:
		typedef Str_::traits_type::char_type Char_;

		StringTokenizer();

		//Load the target string
		void LoadString(const Str_ &str);

		//Set the separator (this doesn't reset the tokenize state)
		void SetSeparator(Char_ separator);

		//Reset the tokenize state
		void ResetTokenizer(void);

		//Get/Set if include the separator in a token
		bool GetIfIncludeSeparator(void) const;
		void SetIfIncludeSeparator(bool val);


		//Get the next token
		Str_ Tokenize(void);

		//Get if the string is end
		bool isEOS(void) const;

	private:
		bool m_ifIncludeSep = false;
		Str_ m_target;
		Char_ m_sep = L'\0';
		Str_::iterator m_pos, m_end;

		Str_ tokenizeRange(Str_::iterator begin, Str_::iterator end);
	};
}