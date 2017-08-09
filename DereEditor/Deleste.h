#pragma once
#include "Note.h"
#include "Measure.h"
#include "PlayableNote.h"

namespace Util {
	static int gcd(int m, int n)
	{
		if (m < n)
			return gcd(n, m);
		while (n != 0)
		{
			int buf = m % n;
			m = n;
			n = buf;
		}
		return m;
	}
}

class DelesteLine {
private:
	int m_measure;
	int m_channel;
	int m_lengthTick;
	std::vector<std::shared_ptr<PlayableNote>> m_notes;

public:
	DelesteLine(int measure, int channel, int length, std::shared_ptr<PlayableNote> note);
	~DelesteLine() = default;
	int getChannel();
	std::vector<std::shared_ptr<PlayableNote>> &getNotes();
	const String toString();
};

struct DelesteHeader {
	String Title;
	String Lyricist;
	String Composer;
	String Mapper;
	String Background;
	String Song;
	String Lyrics;
	String Bpm;
	String Offset;
	String SongOffset;
	String MovieOffset;
	String Difficulty;
	String Level;
	String SongVolume;
	String SEVolume;
	String Attribute;
	String Brightness;
};

class Deleste
{
private:
	std::vector<std::shared_ptr<Measure>> m_measures;
	std::vector<String> m_errorMessages;
	String m_savePath;
	Sound m_music;
	DelesteHeader m_header;
	double m_tempo = 120.0;
	double m_offset = 0.0;
	double m_songOffset = 0.0;
	double m_songVolume = 1.0;
	double m_seVolume = 1.0;


	static void rationalize(double value, int *numer, int *denom) {

		int i = 0;
		while (Fmod(value, 1) != 0) {
			value *= 10;
			++i;
		}
		int n = static_cast<int>(value);
		int d = static_cast<int>(Pow(10, i));
		int g = Util::gcd(n, d);

		*numer = n / g;
		*denom = d / g;
	}

	void existMeasureCheck(size_t measure);

public:
	Deleste(String path);
	Deleste();
	~Deleste() = default;
	void save(const String& path);
	bool hasError();
	std::vector<String>& getErrorMessages();
	std::vector<std::shared_ptr<Measure>>& getMeasures();
	void error(const String &message);
	const String& getSavePath();
	const Sound& getMusic();
	double getOffset();
	double getTempo();
	double getSongVolume();
	double getSEVolume();
	void updateHeader();
	DelesteHeader& getHeader();
	void updateMusic();
};

