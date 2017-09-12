#include "StringUtil.h"
#include <cassert>

namespace StringUtil
{

	StringTokenizer::StringTokenizer()
	{
	}

	void StringTokenizer::LoadString(const Str_  &str)
	{
		m_target = str;
		ResetTokenizer();
	}

	void StringTokenizer::SetSeparator(Char_ separator)
	{
		m_sep = separator;
	}

	void StringTokenizer::ResetTokenizer(void)
	{
		m_pos = std::begin(m_target);
		m_end = std::end(m_target);
	}

	bool StringTokenizer::GetIfIncludeSeparator(void) const
	{
		return m_ifIncludeSep;
	}

	void StringTokenizer::SetIfIncludeSeparator(bool val)
	{
		m_ifIncludeSep = val;
	}

	Str_ StringTokenizer::Tokenize(void)
	{
		//Search for separator
		auto it = std::find(m_pos, m_end, m_sep);
		auto pos = m_pos;
		
		if(it != m_end) //separator found
		{
			//Update the position
			m_pos = it + 1;

			assert(*(m_pos - 1) == m_sep);
			assert(*it == m_sep);

			return tokenizeRange(pos, it);
		}
		else //separator not found
		{
			m_pos = m_end;
			return Str_(pos, m_end);
		}
	}

	bool StringTokenizer::isEOS(void) const
	{
		return m_pos == std::end(m_target);
	}

	Str_ StringTokenizer::tokenizeRange(Str_::iterator begin, Str_::iterator end)
	{
		if(m_ifIncludeSep)
		{
			return Str_(begin, end + 1);
		}
		else
		{
			return Str_(begin, end);
		}
	}

}