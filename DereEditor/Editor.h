#pragma once
#include "Measure.h"
#include "Note.h"
#include "PlayableNote.h"
#include "ChangeTempo.h"
#include "Deleste.h"

enum class EditType {
	None = -1,
	Measure,
	PlayableNote,
	ChangeTempo,
};

class Editor {
private:
	std::vector<std::shared_ptr<PlayableNote>> m_playableNotes;
	std::vector<std::shared_ptr<ChangeTempo>> m_changeTempos;
	const int m_beatPerHeight = 80;
	const int m_LaneOffset = 125;
	const int m_LaneWidth = 70;
	size_t m_division = 1;
	int m_channel = 0;
	int m_laneCount = 5;
	std::array<int, 8> m_divisions{ 4,8,16,32,64,12,24,48 };
	const int m_gridOffset = 60;
	const size_t MEASURE_MAX = 999;
	const size_t MEASURE_MIN = 1;
	double m_zoom = 1.0;
	size_t m_selectedMeasure = 0;
	GUI m_gui;
	GUI m_headerGui;
	Rect m_selectedRect;
	Point m_selectBegin;
	Point m_selectEnd;
	Deleste m_deleste;
	bool m_stateChanged = false;
	bool m_isActive = true;
	String m_delesteSimulatorPath;

	/*
	小節の更新がある時、必ず呼ぶ
	*/
	void updateMeasureState();

	/*
	ノート配置の更新がある時、必ず呼ぶ
	*/
	void updateNoteState();

	double getBeatPerHeight();

	int getLaneOffset();

	/*
	プレイアブルノートの設置、削除
	*/
	void setOrRemovePlaybleNote(std::vector<std::shared_ptr<Note>> &notes, int tick, int lane, NoteType type);

	/*
	テンポ変更の設置、削除
	*/
	void setOrRemoveChangeTempo(std::vector<std::shared_ptr<Note>> &notes, int tick, double tempo);

	void changeZoomRatio();

	void channelUp();
	void channelDown();

	void divisionUp();
	void divisionDown();
	int getDivision();

	void addMeasure();
	void removeMeasure();
	void setMeasureRhythm();
	void removeMeasureRhythm();

	void addLane();
	void removeLane();

	double getScroll();
	void playSE();

	void newFile();
	void openFile();
	void saveFile();
	void saveAsFile();

	void editHeader();
	void closeHeader();
	void saveHeader();

	void resetBeatmap();
	void changeBeatmap(const Deleste& beatmap);

	void playMusic();
	void pauseMusic();

	void runSimulator();

	const Sound& getMusic();

	std::vector<std::shared_ptr<Measure>>& getMeasures();

public:

	Editor();
	~Editor() = default;

	void update();
	void draw();
	void setDelesteSimulatorPath(const String& path);
};