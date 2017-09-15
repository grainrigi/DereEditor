#pragma once

#include <unordered_map>
#include "StringUtil.h"

namespace Locale {
	typedef std::wstring Str_;
	class LocaleManager;
	class LocaleStringManager;

	//LocaleManager
	//	*Manages locale data and provide string access.
	class LocaleManager
	{
	public:
		LocaleManager();

		//Load locale data
		//	*If no locale data has been loaded,
		//	 the loaded data is set to the default locale.
		//*Exceptions
		//	*LocaleParseError - parsing error(illegal format etc.)
		void LoadLocaleFromString(const Str_ &str);

		//Switch locale
		//*Exceptions
		//	*LocaleNotFoundException - the specified locale cannot be found.
		void ChangeLocale(const Str_ &loc);

		//Get the locale specific string
		//*Exceptions
		//	*LocaleException - no locale has been loaded
		//	*LocaleNotFoundException - the specified id is not contained in this locale
		Str_ GetLocaleString(uint32_t id) const;

	private:
		std::unordered_map<Str_, LocaleStringManager> m_locales;
		LocaleStringManager *m_current = nullptr;

		bool isNoLocale(void) const;
	};

	//LocaleStringManager
	//	*Deal with the string table where locale data is contained.
	class LocaleStringManager
	{
	public:
		LocaleStringManager(void);
		LocaleStringManager(const Str_ &data);

		void LoadLocaleData(const Str_ &data);

		Str_ GetLocaleName(void) const;
		
		Str_ GetLocaleString(uint32_t id) const;

		int GetCount(void) const;

	private:
		std::unordered_map<uint32_t, Str_> m_table;
		Str_ m_locname;

		class CountableStringTokenizer : public StringUtil::StringTokenizer
		{
			//Add tokenize counting feature
		public:
			int GetTokenizeCount(void) const;

			void LoadString(const Str_ &str);

			void ResetTokenizer(void);

			Str_ Tokenize(void);

		private:
			int m_count;
		};

		typedef CountableStringTokenizer Tokenizer;

		void parseHeader(Tokenizer &tok);

		bool parseDataLine(Tokenizer &tok);

		bool isLoaded(void) const;

		void cleanup(void);
	};

	class LocaleException : public std::runtime_error
	{
	public:
		LocaleException(void);
		LocaleException(const std::string &message);
	};

	class LocaleParseException : public LocaleException
	{
	public:
		LocaleParseException(void);
		LocaleParseException(const std::string &message);
		LocaleParseException(int line, const std::string &message);

		virtual const char *what() const override;

	private:
		std::string m_message;
	};

	class LocaleNotFoundException : public LocaleException
	{
	public:
		LocaleNotFoundException(const std::string &);
	};

	/* free functions */

	//Get the string id from string name
	uint32_t getStringID(const Str_ &key);

	//Get the number of strings
	int getRequiredEntryCount(void);
}

