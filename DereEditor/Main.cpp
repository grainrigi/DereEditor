# include <Siv3D.hpp>
# include "Editor.h"
# include "locale/Locale.h"

namespace
{
	bool prepareLocale(const String &filename)
	{
		auto &locman = Locale::LocaleManager::instance();

		if (filename.isEmpty)
			return true;

		//Load specified locale file
		TextReader r(filename);

		if (!r.isOpened())
		{
			MessageBox::Show(L"locale file could not be loaded.");
			return true;
		}

		try {
			locman.LoadLocaleFromString(r.readAll().str());
		}
		catch(Locale::LocaleException ex)
		{
			MessageBox::Show(L"LocaleException : " + Widen(ex.what()));
			return false;
		}

		r.close();

		return true;
	}
}

void Main()
{
	//TODO:可変ウィンドウサイズ対応
	//Window::SetStyle(WindowStyle::Sizeable);
	Window::SetTitle(L"DereEditor");
	Window::Resize(1024, 768);

	//アセットロード
	TextureAsset::Register(L"notes", L"/2000");
	TextureAsset::Register(L"icons", L"/2001");
	SoundAsset::Register(L"tap", L"/3000");
	SoundAsset::Register(L"flick", L"/3001");
	SoundAsset::Register(L"slide", L"/3002");
	SoundAsset::Register(L"none", L"/3003");
	FontAsset::Register(L"editor", 16);

	//コンフィグロード
	INIReader config(L"config.ini");

	//ロケール準備
	if (!prepareLocale(config.get<String>(L"Locale.localefile")))
		return;

	Editor editor;
	editor.setDelesteSimulatorPath(config.get<String>(L"SimulatorPath.deleste"));

	Graphics::SetVSyncEnabled(true);

	while (System::Update()) {
		ClearPrint();
		Println(L"FPS:", Profiler::FPS());
		editor.update();
		editor.draw();
	}
}
