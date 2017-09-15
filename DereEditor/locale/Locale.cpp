#include "Locale.h"
#include "LocaleTable.h"

#include <sstream>

namespace Locale
{
	uint32_t getStringID(const Str_ &key)
	{
		auto it = loc_dic.find(key);
		if (it == std::end(loc_dic))
			return 0xffffffff;
		else
			return it->second;
	}
	
	Str_ getDefaultString(uint32_t id)
	{
		auto it = def_dic.find(id);
		if (it == std::end(def_dic))
			throw LocaleNotFoundException("The specified id is not contained in the string table.");
		else
			return it->second;
	}

	int getRequiredEntryCount(void)
	{
		return loc_dic.size();
	}

	LocaleStringManager::LocaleStringManager(void)
	{
	}

	LocaleStringManager::LocaleStringManager(const Str_ &data)
	{
		LoadLocaleData(data);
	}

	void LocaleStringManager::LoadLocaleData(const Str_ & data)
	{
		//cleanup if loaded
		if (isLoaded())
			cleanup();

		//parse the data
		Tokenizer line_tok;

		line_tok.LoadString(data);
		line_tok.SetSeparator(L'\n');
		line_tok.SetIfIncludeSeparator(false);

		//parse the header
		parseHeader(line_tok);

		//parse the data lines
		while (parseDataLine(line_tok));

		//Check if there are missing entries
		if (GetCount() < getRequiredEntryCount())
			throw LocaleParseException("one or more entry is missing");
	}

	Str_ LocaleStringManager::GetLocaleName(void) const
	{
		return m_locname;
	}

	Str_ LocaleStringManager::GetLocaleString(uint32_t id) const
	{
		auto it = m_table.find(id);
		if (it == m_table.end())
			throw LocaleNotFoundException("The specified id is not contained in the string table.");

		return it->second;
	}

	int LocaleStringManager::GetCount(void) const
	{
		return m_table.size();
	}

	void LocaleStringManager::parseHeader(Tokenizer & tok)
	{
		m_locname = tok.Tokenize();
		if (m_locname.size() == 0)
			throw LocaleParseException(1, "locale name must not be empty");
	}

	bool LocaleStringManager::parseDataLine(Tokenizer & tok)
	{
		auto line = tok.Tokenize();

		Tokenizer eqtok;
		eqtok.LoadString(line);
		eqtok.SetSeparator(L'=');
		
		//Read data
		auto key = eqtok.Tokenize();
		if (eqtok.isEOS()) //not a data line
			return !tok.isEOS();
		auto val = eqtok.Tokenize();

		//Get the integer id
		auto id = getStringID(key);

		//Insert the string pair
		if(id != -1)
			m_table.insert(std::make_pair(id, val));

		return !tok.isEOS();
	}

	bool LocaleStringManager::isLoaded(void) const
	{
		return m_locname.size() > 0;
	}

	void LocaleStringManager::cleanup(void)
	{
		//clean map
		m_table.clear();
		//clean locname
		m_locname.clear();
	}


	LocaleNotFoundException::LocaleNotFoundException(const std::string &err)
		: LocaleException(err)
	{
	}
	int LocaleStringManager::CountableStringTokenizer::GetTokenizeCount(void) const
	{
		return m_count;
	}
	void LocaleStringManager::CountableStringTokenizer::LoadString(const Str_ & str)
	{
		m_count = 0;
		this->StringUtil::StringTokenizer::LoadString(str);
	}
	void LocaleStringManager::CountableStringTokenizer::ResetTokenizer(void)
	{
		m_count = 0;
		this->StringUtil::StringTokenizer::ResetTokenizer();
	}
	Str_ LocaleStringManager::CountableStringTokenizer::Tokenize(void)
	{
		m_count++;
		return this->StringUtil::StringTokenizer::Tokenize();
	}
	LocaleParseException::LocaleParseException(void)
	{
	}
	LocaleParseException::LocaleParseException(const std::string & message)
		: m_message(message)
	{
	}
	LocaleParseException::LocaleParseException(int line, const std::string &message)
	{
		std::stringstream ss;

		ss << "at Line " << line << ": ";
		ss << message;

		m_message = ss.str();
	}
	const char * LocaleParseException::what() const
	{
		return m_message.c_str();
	}

	void LocaleManager::LoadLocaleFromString(const Str_ &str)
	{
		//Load locale
		LocaleStringManager sm(str);

		//Add locale
		auto it = m_locales.insert(std::make_pair(sm.GetLocaleName(), std::move(sm)));

		//Set default locale if needed
		if (isNoLocale())
			m_current = &(it.first->second);
	}

	void LocaleManager::ChangeLocale(const Str_ & loc)
	{
		auto it = m_locales.find(loc);

		if (it == std::end(m_locales))
			throw LocaleNotFoundException("the specified locale cannot be found");

		m_current = &it->second;
	}

	Str_ LocaleManager::GetLocaleString(uint32_t id) const
	{
		if (m_current == nullptr)
			return getDefaultString(id);

		return m_current->GetLocaleString(id);
	}

	bool LocaleManager::isNoLocale(void) const
	{
		return m_current == nullptr;
	}


	LocaleException::LocaleException(void)
		: std::runtime_error("")
	{
	}
	LocaleException::LocaleException(const std::string & message)
		: std::runtime_error(message)
	{
	}
}