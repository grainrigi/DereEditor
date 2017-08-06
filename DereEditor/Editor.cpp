#include "Editor.h"
#include "GUITextureButton.h"
#include "Deleste.h"

void Editor::updateNoteState() {
	m_playableNotes.clear();
	m_changeTempos.clear();
	double plevNoteTick = 0;
	double plevNoteTime = m_deleste.getOffset();
	double currentTempo = m_deleste.getTempo();
	int currentMeasureAbsoluteTick = 0;
	int measureIndex = 0;
	for (auto& measure : getMeasures()) {
		std::sort(measure->getNotes().begin(), measure->getNotes().end(), [](const std::shared_ptr<Note>& a, const std::shared_ptr<Note>& b)
		{
			return a->Tick < b->Tick;
		});
		for (auto& note : measure->getNotes()) {
			double currentNoteAbsoluteTick = currentMeasureAbsoluteTick + note->Tick;
			note->Time = plevNoteTime + (currentNoteAbsoluteTick - plevNoteTick) / 192 * 60 / currentTempo;
			plevNoteTick = currentNoteAbsoluteTick;
			plevNoteTime = note->Time;
			if (typeid(*note) == typeid(PlayableNote))
			{
				note->setPosition(getLaneOffset() + std::dynamic_pointer_cast<PlayableNote>(note)->FinishPos * m_LaneWidth, static_cast<int>(-currentNoteAbsoluteTick / 192 * getBeatPerHeight()));
				m_playableNotes.push_back(std::dynamic_pointer_cast<PlayableNote>(note));
			}
			else if (typeid(*note) == typeid(ChangeTempo)) {
				note->setPosition(getLaneOffset() + m_LaneWidth*m_laneCount, static_cast<int>(-currentNoteAbsoluteTick / 192 * getBeatPerHeight()));
				currentTempo = std::dynamic_pointer_cast<ChangeTempo>(note)->getTempo();
				m_changeTempos.push_back(std::dynamic_pointer_cast<ChangeTempo>(note));
			}
		}
		++measureIndex;

		currentMeasureAbsoluteTick += measure->getLengthTick();
	}

	for (auto currentNote = m_playableNotes.begin(); currentNote != m_playableNotes.end(); ++currentNote) {
		(*currentNote)->PlevNote = nullptr;
		(*currentNote)->NextNote = nullptr;
		switch ((*currentNote)->getNoteType())
		{
		case NoteType::LFlick:
		case NoteType::RFlick:
		{
			auto connectNote = std::find_if(m_playableNotes.begin(), m_playableNotes.end(), [&](std::shared_ptr<PlayableNote> x) {
				return x->Time > (*currentNote)->Time &&  x->Channel == (*currentNote)->Channel && x->Time - (*currentNote)->Time < 1.0
					&& x->PlevNote == nullptr
					&& (x->getNoteType() == NoteType::LFlick || x->getNoteType() == NoteType::RFlick)
					&& ((*currentNote)->Channel % 4 < 2 ? (
					(*currentNote)->getNoteType() == NoteType::LFlick ?
						x->FinishPos < (*currentNote)->FinishPos :
						x->FinishPos >(*currentNote)->FinishPos) : true);
			});
			if (connectNote != m_playableNotes.end()) {
				(*currentNote)->NextNote = (*connectNote);
				(*connectNote)->PlevNote = (*currentNote);
			}
		}
		break;
		case NoteType::Long:
		{
			auto connectNote = std::find_if(m_playableNotes.begin(), m_playableNotes.end(), [&](std::shared_ptr<PlayableNote> x) {return x->Time > (*currentNote)->Time && x->FinishPos == (*currentNote)->FinishPos; });
			if (connectNote != m_playableNotes.end())
				(*currentNote)->NextNote = (*connectNote);
		}
		break;
		case NoteType::Slide:
		{
			auto connectNote = std::find_if(m_playableNotes.begin(), m_playableNotes.end(), [&](std::shared_ptr<PlayableNote> x) {return x->Time > (*currentNote)->Time && x->Channel == (*currentNote)->Channel; });
			if (connectNote != m_playableNotes.end())
				(*currentNote)->NextNote = (*connectNote);
		}
		break;
		}
	}
	m_stateChanged = true;
}

void Editor::updateMeasureState() {
	m_stateChanged = true;
	Measure::updateMeasureState(getMeasures());
}

double Editor::getBeatPerHeight() {
	return m_zoom * m_beatPerHeight;
}

//TODO:可変ウィンドウサイズ対応

int Editor::getLaneOffset() {
	return m_LaneOffset;
}

void Editor::setOrRemovePlaybleNote(std::vector<std::shared_ptr<Note>> &notes, int tick, int finishPos, NoteType type) {
	m_stateChanged = true;

	int startPos = -1;

	if (Input::Key1.pressed) {
		startPos = 0;
	}
	else if (Input::Key2.pressed) {
		startPos = 1;
	}
	else if (Input::Key3.pressed) {
		startPos = 2;
	}
	else if (Input::Key4.pressed) {
		startPos = 3;
	}
	else if (Input::Key5.pressed) {
		startPos = 4;
	}

	for (auto it = notes.begin(); it != notes.end();) {
		if (typeid(**it) == typeid(PlayableNote) && (*it)->Tick == tick && std::dynamic_pointer_cast<PlayableNote>(*it)->FinishPos == finishPos) {
			if (startPos == -1) {
				notes.erase(it);
			}
			else {
				std::dynamic_pointer_cast<PlayableNote>(*it)->setStartPos(startPos);
			}
			return;
		}
		else {
			++it;
		}
	}
	notes.push_back(std::make_shared<PlayableNote>(tick, startPos == -1 ? finishPos : startPos, finishPos, m_channel, type));
}

void Editor::setOrRemoveChangeTempo(std::vector<std::shared_ptr<Note>> &notes, int tick, double tempo) {
	m_stateChanged = true;
	for (auto it = notes.begin(); it != notes.end();) {
		if (typeid(**it) == typeid(ChangeTempo) && (*it)->Tick == tick) {
			notes.erase(it);
			return;
		}
		else {
			++it;
		}
	}
	notes.push_back(std::make_shared<ChangeTempo>(tick, tempo));
}

void Editor::changeZoomRatio() {
	m_zoom = m_gui.slider(L"zoom").value;
	m_gui.text(L"zoom-ratio").text = Format(static_cast<int>(m_zoom * 100), L"%");
	updateNoteState();
}

void Editor::addMeasure() {
	if (getMeasures().size() == MEASURE_MAX) {
		pauseMusic();
		MessageBox::Show(L"これ以上小節を追加出来ません。");
		return;
	}

	getMeasures().insert(getMeasures().begin() + m_selectedMeasure, std::make_shared<Measure>());
	m_selectedMeasure;
	updateMeasureState();
	updateNoteState();
}

void Editor::removeMeasure() {
	if (getMeasures().size() == MEASURE_MIN) {
		pauseMusic();
		MessageBox::Show(L"小節を全て削除することは出来ません。");
		return;
	}

	if (getMeasures()[m_selectedMeasure]->getNotes().size() > 0) {
		pauseMusic();
		if (MessageBox::Show(L"ノートが含まれています。小節を削除しますか？", MessageBoxStyle::YesNo) == MessageBoxCommand::Yes) {
			getMeasures().erase(getMeasures().begin() + m_selectedMeasure);
			if (getMeasures().size() == m_selectedMeasure) {
				--m_selectedMeasure;
			}
			updateMeasureState();
			updateNoteState();
		}
	}
	else {
		getMeasures().erase(getMeasures().begin() + m_selectedMeasure);
		if (getMeasures().size() == m_selectedMeasure) {
			--m_selectedMeasure;
		}
		updateMeasureState();
		updateNoteState();
	}
}

void Editor::setMeasureRhythm() {
	String str = m_gui.textField(L"measure-rhythm").text;
	if (str.indexOf(L"/") == String::npos) {
		pauseMusic();
		MessageBox::Show(L"4/4形式で入力してください");
		return;
	}
	auto nums = str.split(L'/');
	int numer = Parse<int>(nums[0]);
	int denom = Parse<int>(nums[1]);
	if (numer <= 0 || denom <= 0) {
		pauseMusic();
		MessageBox::Show(L"小節の長さは0以下に設定出来ません。");
		return;
	}
	else if (denom != 4 && denom != 8 && denom != 16 && denom != 32 && denom != 64) {
		pauseMusic();
		MessageBox::Show(L"分母は4,8,16,32,64のみが使用出来ます。");
		return;
	}
	getMeasures()[m_selectedMeasure]->Rhythm = std::make_shared<Rhythm>(numer, denom);
	updateMeasureState();
	updateNoteState();
}

void Editor::removeMeasureRhythm() {
	getMeasures()[m_selectedMeasure]->Rhythm = nullptr;
	updateMeasureState();
	updateNoteState();
}

void Editor::channelUp() {
	m_gui.text(L"channel").text = Format(++m_channel);
}

void Editor::channelDown() {
	if (m_channel != 0)
		m_gui.text(L"channel").text = Format(--m_channel);
}

double Editor::getScroll() {
	double currentTime = getMusic().streamPosSec();
	if (m_changeTempos.size() == 0) {
		return -(currentTime - m_deleste.getOffset()) * m_deleste.getTempo() / 60.0  * getBeatPerHeight();
	}
	else {
		if (currentTime < (*m_changeTempos.begin())->Time)
		{
			return Lerp(m_deleste.getOffset() * m_deleste.getTempo() / 60.0  * getBeatPerHeight(), (*m_changeTempos.begin())->getY(), currentTime / (*m_changeTempos.begin())->Time);
		}
		else if ((*(m_changeTempos.end() - 1))->Time <= currentTime)
		{
			return (*(m_changeTempos.end() - 1))->getY() - (currentTime - (*(m_changeTempos.end() - 1))->Time) * (*(m_changeTempos.end() - 1))->getTempo() / 60 * getBeatPerHeight();
		}
		else
		{
			for (auto it = m_changeTempos.begin(); it != m_changeTempos.end() - 1; it++) {
				if ((*it)->Time <= currentTime && currentTime < (*(it + 1))->Time)
				{
					double t = (currentTime - (*it)->Time) / ((*(it + 1))->Time - (*it)->Time);
					return Lerp((*it)->getY(), (*(it + 1))->getY(), t);
				}
			}
			return 0;
		}
	}
}

void Editor::playSE() {
	double currentTime = getMusic().streamPosSec();
	for (auto &note : m_playableNotes) {
		if (!note->IsJudged && note->Time - currentTime < 0.0) {
			switch (note->getNoteType())
			{
			case NoteType::Tap:
			case NoteType::Long:
				SoundAsset(L"tap").playMulti(m_deleste.getSEVolume());
				break;
			case NoteType::LFlick:
			case NoteType::RFlick:
				SoundAsset(L"flick").playMulti(m_deleste.getSEVolume());
				break;
			case NoteType::Slide:
				SoundAsset(L"slide").playMulti(m_deleste.getSEVolume());
				break;
			default:
				break;
			}
			note->IsJudged = true;
		}
	}
}

const Sound& Editor::getMusic() {
	return m_deleste.getMusic();
}

std::vector<std::shared_ptr<Measure>>& Editor::getMeasures() {
	return m_deleste.getMeasures();
}

void Editor::addLane() {
	//TODO:レーン削除
}

void Editor::removeLane() {
	//TODO:レーン追加
}

void Editor::divisionUp() {
	if (m_division != m_divisions.size() - 1) {
		++m_division;
		m_gui.text(L"division").text = Format(getDivision());
	}
}

void Editor::divisionDown() {
	if (m_division != 0) {
		--m_division;
		m_gui.text(L"division").text = Format(getDivision());
	}
}

int Editor::getDivision() {
	return m_divisions[m_division];
}

void Editor::resetBeatmap() {
	m_deleste = Deleste();
	updateNoteState();
	m_stateChanged = false;
}

void Editor::changeBeatmap(const Deleste& beatmap) {
	m_deleste = beatmap;
	updateNoteState();
	m_stateChanged = false;
}

void Editor::newFile() {
	if (m_stateChanged) {
		pauseMusic();
		auto result = MessageBox::Show(L"譜面が変更されています。保存しますか？", MessageBoxStyle::YesNoCancel);
		if (result == MessageBoxCommand::Yes) {
			saveFile();
			resetBeatmap();
		}
		else if (result == MessageBoxCommand::No) {
			resetBeatmap();
		}
		else {
			return;
		}
	}
	else {
		resetBeatmap();
	}
}

void Editor::openFile() {
	pauseMusic();
	if (const auto open = Dialog::GetOpen({ { L"Deleste ファイル (*.txt)",L"*.txt" } }))
	{
		Deleste beatmap(open.value());
		if (beatmap.hasError()) {
			String message;
			if (beatmap.getErrorMessages().size() > 20) {
				for (auto it = beatmap.getErrorMessages().begin(); it != beatmap.getErrorMessages().begin() + 20; ++it) {
					message.append(*it + L"\n");
				}
				message.append(L"(21個以上のエラーは省略しています。)\n");
			}
			else {
				for (auto& mes : beatmap.getErrorMessages()) {
					message.append(mes + L"\n");
				}
			}
			message.append(L"エラーを無視して読み込みますか？");
			if (MessageBox::Show(message, MessageBoxStyle::YesNo) == MessageBoxCommand::Yes) {
				changeBeatmap(beatmap);
			}
			else {
				return;
			}
		}
		else {
			changeBeatmap(beatmap);
		}
	}
}

void Editor::saveFile() {
	if (m_deleste.getSavePath().isEmpty) {
		saveAsFile();
	}
	else {
		m_deleste.save(m_deleste.getSavePath());
		m_stateChanged = false;
	}
}

void Editor::saveAsFile() {
	pauseMusic();
	if (const auto save = Dialog::GetSave({ { L"Deleste ファイル (*.txt)",L"*.txt" } }))
	{
		m_deleste.save(save.value());
		m_deleste.updateMusic();
		m_stateChanged = false;
	}
}

void Editor::editHeader() {
	pauseMusic();

	m_headerGui.textField(L"title").setText(m_deleste.getHeader().Title);
	m_headerGui.textField(L"lyricist").setText(m_deleste.getHeader().Lyricist);
	m_headerGui.textField(L"composer").setText(m_deleste.getHeader().Composer);
	m_headerGui.textField(L"mapper").setText(m_deleste.getHeader().Mapper);
	m_headerGui.textField(L"background").setText(m_deleste.getHeader().Background);
	m_headerGui.textField(L"song").setText(m_deleste.getHeader().Song);
	m_headerGui.textField(L"lyrics").setText(m_deleste.getHeader().Lyrics);
	m_headerGui.textField(L"bpm").setText(m_deleste.getHeader().Bpm);
	m_headerGui.textField(L"offset").setText(m_deleste.getHeader().Offset);
	m_headerGui.textField(L"song-offset").setText(m_deleste.getHeader().SongOffset);
	m_headerGui.textField(L"movie-offset").setText(m_deleste.getHeader().MovieOffset);
	m_headerGui.textField(L"difficulty").setText(m_deleste.getHeader().Difficulty);
	m_headerGui.textField(L"level").setText(m_deleste.getHeader().Level);
	m_headerGui.textField(L"song-volume").setText(m_deleste.getHeader().SongVolume);
	m_headerGui.textField(L"se-volume").setText(m_deleste.getHeader().SEVolume);
	m_headerGui.textField(L"attibute").setText(m_deleste.getHeader().Attibute);
	m_headerGui.textField(L"brightness").setText(m_deleste.getHeader().Brightness);

	m_gui.show(false);
	m_headerGui.show(true);
	m_isActive = false;
}

void Editor::saveHeader() {

	m_deleste.getHeader().Title = m_headerGui.textField(L"title").text;
	m_deleste.getHeader().Lyricist = m_headerGui.textField(L"lyricist").text;
	m_deleste.getHeader().Composer = m_headerGui.textField(L"composer").text;
	m_deleste.getHeader().Mapper = m_headerGui.textField(L"mapper").text;
	m_deleste.getHeader().Background = m_headerGui.textField(L"background").text;

	if (m_headerGui.textField(L"song").text != m_deleste.getHeader().Song) {
		m_deleste.getHeader().Song = m_headerGui.textField(L"song").text;
		m_deleste.updateMusic();
	}

	m_deleste.getHeader().Lyrics = m_headerGui.textField(L"lyrics").text;
	m_deleste.getHeader().Bpm = m_headerGui.textField(L"bpm").text;
	m_deleste.getHeader().Offset = m_headerGui.textField(L"offset").text;
	m_deleste.getHeader().SongOffset = m_headerGui.textField(L"song-offset").text;
	m_deleste.getHeader().MovieOffset = m_headerGui.textField(L"movie-offset").text;
	m_deleste.getHeader().Difficulty =  m_headerGui.textField(L"difficulty").text;
	m_deleste.getHeader().Level = m_headerGui.textField(L"level").text;
	m_deleste.getHeader().SongVolume = m_headerGui.textField(L"song-volume").text;
	m_deleste.getHeader().SEVolume = m_headerGui.textField(L"se-volume").text;
	m_deleste.getHeader().Attibute = m_headerGui.textField(L"attibute").text;
	m_deleste.getHeader().Brightness = m_headerGui.textField(L"brightness").text;
	m_deleste.updateHeader();
	updateNoteState();

	closeHeader();
}

void Editor::closeHeader() {
	m_headerGui.show(false);
	m_gui.show(true);
	m_isActive = true;
}

void Editor::playMusic() {
	m_gui.get<GUITextureButtonWrapper>(L"play-music").setTexture(TextureAsset(L"icons")(32, 0, 32, 32));
	getMusic().play();
}

void Editor::pauseMusic() {
	m_gui.get<GUITextureButtonWrapper>(L"play-music").setTexture(TextureAsset(L"icons")(0, 0, 32, 32));
	getMusic().pause();
}

void Editor::runSimulator() {
	pauseMusic();
	if (!FileSystem::IsFile(m_delesteSimulatorPath)) {
		MessageBox::Show(L"シミュレーターが存在しません。");
		return;
	}
	if (m_deleste.getSavePath().isEmpty) {
		saveAsFile();
	}
	System::CreateProcess(m_delesteSimulatorPath,L" \""+m_deleste.getSavePath()+L"\"");
}

Editor::Editor() :
	m_gui(GUIStyle::Default),
	m_headerGui(GUIStyle::Default)
{
	//GUI初期化
	//ヘッダー
	{
		WidgetStyle labelStyle;
		labelStyle.width = 120;

		WidgetStyle textFieldStyle;
		textFieldStyle.width = 230;

		m_headerGui.add(GUIText::Create(L"Title", labelStyle));
		m_headerGui.addln(L"title", GUITextField::Create(none, textFieldStyle));
		m_headerGui.add(GUIText::Create(L"Lyricist", labelStyle));
		m_headerGui.addln(L"lyricist", GUITextField::Create(none, textFieldStyle));
		m_headerGui.add(GUIText::Create(L"Composer", labelStyle));
		m_headerGui.addln(L"composer", GUITextField::Create(none, textFieldStyle));
		m_headerGui.add(GUIText::Create(L"Mapper", labelStyle));
		m_headerGui.addln(L"mapper", GUITextField::Create(none, textFieldStyle));
		m_headerGui.add(GUIText::Create(L"Background", labelStyle));
		m_headerGui.addln(L"background", GUITextField::Create(none, textFieldStyle));
		m_headerGui.add(GUIText::Create(L"Song", labelStyle));
		m_headerGui.addln(L"song", GUITextField::Create(none, textFieldStyle));
		m_headerGui.add(GUIText::Create(L"Lyrics", labelStyle));
		m_headerGui.addln(L"lyrics", GUITextField::Create(none, textFieldStyle));
		m_headerGui.add(GUIText::Create(L"Bpm", labelStyle));
		m_headerGui.addln(L"bpm", GUITextField::Create(none, textFieldStyle));
		m_headerGui.add(GUIText::Create(L"Offset", labelStyle));
		m_headerGui.addln(L"offset", GUITextField::Create(none, textFieldStyle));
		m_headerGui.add(GUIText::Create(L"SongOffset", labelStyle));
		m_headerGui.addln(L"song-offset", GUITextField::Create(none, textFieldStyle));
		m_headerGui.add(GUIText::Create(L"MovieOffset", labelStyle));
		m_headerGui.addln(L"movie-offset", GUITextField::Create(none, textFieldStyle));
		m_headerGui.add(GUIText::Create(L"Difficulty", labelStyle));
		m_headerGui.addln(L"difficulty", GUITextField::Create(none, textFieldStyle));
		m_headerGui.add(GUIText::Create(L"Level", labelStyle));
		m_headerGui.addln(L"level", GUITextField::Create(none, textFieldStyle));
		m_headerGui.add(GUIText::Create(L"SongVolume", labelStyle));
		m_headerGui.addln(L"song-volume", GUITextField::Create(none, textFieldStyle));
		m_headerGui.add(GUIText::Create(L"SEVolume", labelStyle));
		m_headerGui.addln(L"se-volume", GUITextField::Create(none, textFieldStyle));
		m_headerGui.add(GUIText::Create(L"Attibute", labelStyle));
		m_headerGui.addln(L"attibute", GUITextField::Create(none, textFieldStyle));
		m_headerGui.add(GUIText::Create(L"Brightness", labelStyle));
		m_headerGui.addln(L"brightness", GUITextField::Create(none, textFieldStyle));

		m_headerGui.add(GUIHorizontalLine::Create());

		m_headerGui.add(L"cancel", GUIButton::Create(L"キャンセル"));
		m_headerGui.button(L"cancel").style.margin.left = 130;
		m_headerGui.addln(L"save", GUIButton::Create(L"保存"));

		m_headerGui.style.showTitle = false;
		m_headerGui.style.borderRadius = 0;
		m_headerGui.style.width = 400;
		m_headerGui.setPos(Window::ClientRect().w - 400, 0);
		m_headerGui.show(false);
	}
	//メニュー
	{
		WidgetStyle labelStyle;
		labelStyle.width = 100;

		WidgetStyle textFieldStyle;
		textFieldStyle.width = 70;

		m_gui.addln(GUIText::Create(L"[ファイル]"));
		m_gui.add(L"new-file", GUIButton::Create(L"新規作成"));
		m_gui.add(L"open-file", GUIButton::Create(L"開く"));
		m_gui.addln(L"save-file", GUIButton::Create(L"上書き保存"));
		m_gui.add(L"saveas-file", GUIButton::Create(L"名前を付けて保存"));
		m_gui.addln(L"edit-header", GUIButton::Create(L"ヘッダー編集"));
		m_gui.addln(L"run-simulator", GUIButton::Create(L"シミュレーターで再生"));

		m_gui.add(GUIHorizontalLine::Create());

		m_gui.addln(GUIText::Create(L"[グリッド]"));
		m_gui.add(GUIText::Create(L"ズーム", labelStyle));
		m_gui.add(L"zoom", GUISlider::Create(0.2, 5.0, 1.0));
		m_gui.addln(L"zoom-ratio", GUIText::Create(L"100%"));
		m_gui.add(GUIText::Create(L"分割数", labelStyle));
		m_gui.add(L"division-down", GUIButton::Create(L"<"));
		m_gui.add(L"division", GUIText::Create(L"8"));
		m_gui.addln(L"division-up", GUIButton::Create(L">"));
		m_gui.add(GUIText::Create(L"情報を表示する", labelStyle));
		m_gui.addln(L"show-info", GUIToggleSwitch::Create(L"", L"", false));

		m_gui.add(GUIHorizontalLine::Create());

		m_gui.addln(GUIText::Create(L"[ノート]"));
		m_gui.add(GUIText::Create(L"チャンネル", labelStyle));
		m_gui.add(L"channel-down", GUIButton::Create(L"-"));
		m_gui.add(L"channel", GUIText::Create(L"0"));
		m_gui.addln(L"channel-up", GUIButton::Create(L"+"));

		m_gui.add(GUIHorizontalLine::Create(1));

		m_gui.addln(GUIText::Create(L"[拡張ノート]"));
		m_gui.add(GUIText::Create(L"テンポ変更", labelStyle));
		m_gui.addln(L"tempo", GUITextField::Create(none, textFieldStyle));
		m_gui.textField(L"tempo").setText(L"120");

		m_gui.add(GUIHorizontalLine::Create());

		m_gui.addln(GUIText::Create(L"[小節]"));
		m_gui.add(L"add-measure", GUIButton::Create(L"挿入"));
		m_gui.addln(L"remove-measure", GUIButton::Create(L"削除"));

		m_gui.add(GUIText::Create(L"拍子変更", labelStyle));
		m_gui.add(L"measure-rhythm", GUITextField::Create(none, textFieldStyle));
		m_gui.textField(L"measure-rhythm").setText(L"4/4");
		m_gui.add(L"set-rhythm", GUIButton::Create(L"変更"));
		m_gui.addln(L"remove-rhythm", GUIButton::Create(L"削除"));

		m_gui.add(GUIHorizontalLine::Create());

		m_gui.add(L"play-music", GUITextureButton::Create(TextureAsset(L"icons")(0, 0, 32, 32)));
		m_gui.add(L"music-time", GUIText::Create(L"00:00 / 00:00"));
		m_gui.text(L"music-time").style.margin.top = 10;
		m_gui.addln(L"seek-music", GUISlider::Create(0.0, 1.0, 0.0, 200));
		m_gui.slider(L"seek-music").style.margin.top = 8;

		m_gui.style.showTitle = false;
		m_gui.style.borderRadius = 0;
		m_gui.style.width = 400;
		m_gui.setPos(Window::ClientRect().w - 400, 0);
	}
}

void Editor::update() {

	//GUI関連
	if (m_gui.slider(L"zoom").hasChanged) {
		changeZoomRatio();
	}
	else if (m_gui.button(L"add-measure").pushed) {
		addMeasure();
	}
	else if (m_gui.button(L"remove-measure").pushed) {
		removeMeasure();
	}
	else if (m_gui.button(L"set-rhythm").pushed) {
		setMeasureRhythm();
	}
	else if (m_gui.button(L"remove-rhythm").pushed) {
		removeMeasureRhythm();
	}
	else if (m_gui.button(L"channel-up").pushed) {
		channelUp();
	}
	else if (m_gui.button(L"channel-down").pushed) {
		channelDown();
	}
	else if (m_gui.button(L"division-up").pushed) {
		divisionUp();
	}
	else if (m_gui.button(L"division-down").pushed) {
		divisionDown();
	}
	else if (m_gui.get<GUITextureButtonWrapper>(L"play-music").pushed)
	{
		if (getMusic().isPlaying()) {
			pauseMusic();
		}
		else {
			playMusic();
		}
	}
	else if (m_gui.slider(L"seek-music").hasChanged)
	{
		double seekTime = getMusic().lengthSec() * m_gui.slider(L"seek-music").value;
		getMusic().setPosSec(seekTime);
		for (auto &note : m_playableNotes)
			note->IsJudged = note->Time - seekTime < 0.0;
	}
	else if (m_gui.button(L"new-file").pushed) {
		newFile();
	}
	else if (m_gui.button(L"open-file").pushed) {
		openFile();
	}
	else if (m_gui.button(L"save-file").pushed) {
		saveFile();
	}
	else if (m_gui.button(L"saveas-file").pushed) {
		saveAsFile();
	}
	else if (m_gui.button(L"edit-header").pushed) {
		editHeader();
	}
	else if (m_gui.button(L"run-simulator").pushed) {
		runSimulator();
	}
	else if (m_headerGui.button(L"cancel").pushed) {
		closeHeader();
	}
	else if (m_headerGui.button(L"save").pushed) {
		saveHeader();
	}

	if (m_isActive) {
		if (!getMusic().isPlaying() && !Input::KeyShift.pressed && Mouse::Wheel() != 0) {
			double seekTime = getMusic().streamPosSec() - 0.1*Mouse::Wheel();
			seekTime =
				seekTime < 0 ? seekTime = 0 :
				getMusic().lengthSec() < seekTime ? getMusic().lengthSec() :
				seekTime;
			getMusic().setPosSec(seekTime);
			for (auto &note : m_playableNotes)
				note->IsJudged = note->Time - seekTime < 0.0;
		}

		if (Input::KeyShift.pressed && Mouse::Wheel() != 0) {
			m_gui.slider(L"zoom").setValue(m_gui.slider(L"zoom").value - 0.05 * Mouse::Wheel());
			changeZoomRatio();
		}

		m_gui.slider(L"seek-music").setValue(getMusic().streamPosSec() / getMusic().lengthSec());
		int currentTime = static_cast<int>(getMusic().streamPosSec());
		int musicLength = static_cast<int>(getMusic().lengthSec());
		m_gui.text(L"music-time").text = Format(L"{}:{} / {}:{}"_fmt, currentTime / 60, Pad(currentTime % 60, { 2, L'0' }), musicLength / 60, Pad(musicLength % 60, { 2, L'0' }));

		//TODO:キーショートカット
		if (Input::KeyA.clicked) {
			channelDown();
		}
		else if (Input::KeyD.clicked) {
			channelUp();
		}
		else if (Input::KeyW.clicked) {
			divisionUp();
		}
		else if (Input::KeyS.clicked) {
			divisionDown();
		}
		else if (Input::KeySpace.clicked) {
			if (getMusic().isPlaying()) {
				pauseMusic();
			}
			else {
				playMusic();
			}

		}
		else if (Input::KeyF5.clicked) {
			runSimulator();
		}

		if (Input::KeyControl.pressed) {
			if (Input::KeyS.clicked) {
				saveFile();
			}
			else if (Input::KeyZ.clicked) {
				//TODO:Undo
			}
			else if (Input::KeyY.clicked) {
				//TODO:Redo
			}
		}

		NoteType currentNoteType = NoteType::Tap;

		//ノート選択
		if (Input::KeyControl.pressed && Input::MouseL.clicked) {
			currentNoteType = NoteType::Slide;
		}
		else if (Input::KeyShift.pressed) {
			if (Input::MouseL.clicked) {
				currentNoteType = NoteType::LFlick;
			}
			else if (Input::MouseR.clicked) {
				currentNoteType = NoteType::RFlick;
			}
		}
		else {
			if (Input::MouseL.clicked) {
				currentNoteType = NoteType::Tap;
			}
			else if (Input::MouseR.clicked) {
				currentNoteType = NoteType::Long;
			}
		}

		if (Input::MouseL.clicked || Input::MouseR.clicked) {

			Vec2 mousePos = Mouse::PosF().movedBy(0, -Window::ClientRect().h + m_gridOffset + getScroll());

			int lane = -1;
			int offset = getLaneOffset() - m_LaneWidth / 2;
			EditType editType = EditType::None;

			if (0 <= mousePos.x && mousePos.x < offset) {
				editType = EditType::Measure;
			}
			else if (offset + m_laneCount * m_LaneWidth <= mousePos.x && mousePos.x < offset + (m_laneCount + 1) * m_LaneWidth) {
				editType = EditType::ChangeTempo;
			}
			else {
				for (int i = 0; i < m_laneCount; i++)
				{
					if (offset + i * m_LaneWidth <= mousePos.x && mousePos.x < offset + (i + 1) * m_LaneWidth)
					{
						lane = i;
						editType = EditType::PlayableNote;
					}
				}
			}

			switch (editType)
			{
			case EditType::PlayableNote:
			case EditType::ChangeTempo:
			{
				double gridHeightHalf = getBeatPerHeight() * 4 / getDivision() / 2;

				for (auto it = getMeasures().begin(); it != getMeasures().end(); ++it) {

					int lineCount = static_cast<int>(Ceil(getDivision() * (*it)->getLength()));
					double y = (*it)->BeginY;
					int tick = -1;

					//拍子を考慮して判定する
					//(分割線が小節開始線のみ?小節の中間:小節開始線-グリッドの高さ/2)以上、小節開始線と前の小節の最後の分割線の中間未満
					if (y - (lineCount == 1 ? (y - (*it)->EndY) / 2 : gridHeightHalf) <= mousePos.y && mousePos.y < y + (it == getMeasures().begin() ? gridHeightHalf : ((*(it - 1))->LastLineY - y) / 2))
						tick = 0;
					y -= gridHeightHalf * 2;

					//中間
					//分割線が小節開始線のみの場合、スキップ
					//分割線-グリッドの高さ/2の以上、分割線+グリッドの高さ/2未満で判定
					for (int i = 1; i < lineCount - 1; i++)
					{
						if (y - gridHeightHalf <= mousePos.y && mousePos.y < y + gridHeightHalf)
							tick = 192 / (getDivision() / 4) * i;
						y -= gridHeightHalf * 2;
					}
					//最後
					//分割線が小節開始線のみの場合、スキップ
					//分割線と小節終線(次の小節の開始線)の中間以上、分割線+グリッドの高さ/2未満で判定
					if (lineCount != 1 && y - (y - (*it)->EndY) / 2 <= mousePos.y && mousePos.y < y + gridHeightHalf)
						tick = 192 / (getDivision() / 4) * (lineCount - 1);

					if (tick != -1) {
						switch (editType)
						{
						case EditType::PlayableNote:
							setOrRemovePlaybleNote((*it)->getNotes(), tick, lane, currentNoteType);
							break;
						case EditType::ChangeTempo:
							setOrRemoveChangeTempo((*it)->getNotes(), tick, Parse<double>(m_gui.textField(L"tempo").text));
							break;
						}
					}
				}
				updateNoteState();
			}
			break;
			case EditType::Measure:
				for (auto it = getMeasures().begin(); it != getMeasures().end(); ++it) {
					if ((*it)->EndY <= mousePos.y && mousePos.y < (*it)->BeginY) {
						m_selectedMeasure = std::distance(getMeasures().begin(), it);
					}
				}
				break;
			}
		}
		playSE();
	}
}

void Editor::draw() {

	if (m_isActive) {
		Graphics2D::ClearTransform();

		const Mat3x2 scrollMatrix = Mat3x2::Identity().translate(0, Window::ClientRect().h - m_gridOffset - static_cast<int>(getScroll()));

		const Line divLine(getLaneOffset() - m_LaneWidth / 2, -0.5, getLaneOffset() - m_LaneWidth / 2 + m_LaneWidth * (m_laneCount + 1), -0.5);
		const Line laneLine(-0.5, 0, -0.5, Window::ClientRect().h);
		//m_rect.draw(Color(30, 30, 30));

		//レーン
		for (auto i = 0; i < m_laneCount; i++) {
			laneLine.movedBy(i * m_LaneWidth + getLaneOffset(), 0).draw(Palette::White);
		}

		laneLine.movedBy(m_laneCount * m_LaneWidth + getLaneOffset(), 0).draw(Color(50, 50, 50));

		Graphics2D::SetTransform(scrollMatrix);

		//小節
		double measureBeginY = 0;
		for (auto& measure : getMeasures()) {
			int lineCount = static_cast<int>(Ceil(getDivision() * measure->getLength()));

			double gridHeight = getBeatPerHeight() * 4 / getDivision();
			double y = measureBeginY;

			divLine.movedBy(0, static_cast<int>(y)).draw(Palette::Green);

			int emphasizeInterval = getDivision() / 4;

			for (int i = 1; i < lineCount; i++)
			{
				y -= gridHeight;
				divLine.movedBy(0, static_cast<int>(y)).draw(i % emphasizeInterval == 0 ? Palette::White : Palette::Gray);
			}

			measure->BeginY = measureBeginY;
			measure->LastLineY = y;
			measureBeginY -= getBeatPerHeight() * 4 * measure->getLength();
			measure->EndY = measureBeginY;
		}

		Graphics2D::ClearTransform();

		divLine.movedBy(0, Window::ClientRect().h - m_gridOffset).draw(Palette::Red);

		Graphics2D::SetTransform(scrollMatrix);

		//小節番号/拍子
		for (size_t i = 0; i < getMeasures().size(); ++i) {
			FontAsset(L"editor")(Pad(i, { 3, L'0' })).drawAt(divLine.begin.x - 35, getMeasures()[i]->BeginY);
			if (getMeasures()[i]->Rhythm != nullptr)
				FontAsset(L"editor")(getMeasures()[i]->Rhythm->toString()).drawAt(divLine.end.x + 40, getMeasures()[i]->BeginY);
		}

		//帯
		for (auto& note : m_playableNotes) {
			note->drawRibbon();
		}

		//ノート
		for (auto& note : m_playableNotes) {
			note->draw();
		}

		//ノート情報
		if (m_gui.toggleSwitch(L"show-info").isRight) {
			for (auto& note : m_playableNotes) {
				note->drawInfo();
			}
		}

		//テンポ変更
		for (auto& note : m_changeTempos) {
			note->draw();
		}

		//小節選択
		Triangle(divLine.begin.x - 75, getMeasures()[m_selectedMeasure]->BeginY, 15, 90_deg).draw(Palette::Red);

		Println(L"Notes:", m_playableNotes.size());
	}
}

void Editor::setDelesteSimulatorPath(const String& path) {
	m_delesteSimulatorPath = path;
}