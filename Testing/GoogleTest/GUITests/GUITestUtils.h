#include <Windows.h>
#include <filesystem>

namespace GUITestUtils
{
	bool saveWindowImageAsPNG(HWND hwnd, const std::filesystem::path& filename);
	DWORD waitForInputIdleByHWND(HWND hwnd, DWORD dwMilliseconds = WAIT_TIMEOUT);
	HWND findForegroundDialog();
	void typeText(const wchar_t *text);
	void typeAltPlusKey(char key);
	std::filesystem::path getModuleFileName();
	std::filesystem::path getModuleFolder();
	void waitUntilClose(HWND hwnd);
	void waitUntilFocus(HWND hwnd);
	void selectMenu(HWND hwnd, unsigned id, bool async = false);
	inline void selectMenuAsync(HWND hwnd, unsigned id) { selectMenu(hwnd, id, true); };
	HWND execWinMerge(const std::string& args = "/noprefs /maxmize");

}

#define selectMenuAndSaveWindowImage(id) { selectMenu(id); saveImage(#id); }
#define selectOpenDialogMenuAndSaveDialogImage(id) ([this](){ HWND hwnd = selectOpenDialogMenu(id); saveForegroundDialogImage(#id); return hwnd; })()

class CommonTest : public testing::Test
{
public:
	void selectMenu(UINT id)
	{
		GUITestUtils::selectMenu(m_hwndWinMerge, id);
	}

	HWND selectOpenDialogMenu(UINT id)
	{
		GUITestUtils::selectMenuAsync(m_hwndWinMerge, id);
		HWND hwndDlg = GUITestUtils::findForegroundDialog();
		return hwndDlg;
	}

	static std::filesystem::path getScreenshotFolderPath()
	{
		return GUITestUtils::getModuleFolder() / L"../../../Build/Screenshot/";
	}

	static std::filesystem::path getScreenshotFilePath(const char *id = nullptr)
	{
		std::string basename = ::testing::UnitTest::GetInstance()->current_test_info()->test_case_name();
		basename.append(".");
		basename.append(::testing::UnitTest::GetInstance()->current_test_info()->name());
		if (id)
		{
			basename.append(".");
			basename.append(id);
		}
		std::filesystem::path path = getScreenshotFolderPath() / (basename + ".png");
		int i = 2;
		while (std::filesystem::exists(path))
		{
			path = getScreenshotFolderPath() / (basename + "(" + std::to_string(i) + ").png");
			++i;
		}
		return path;
	}

	void saveImage(const char *id = nullptr)
	{
		ASSERT_TRUE(GUITestUtils::saveWindowImageAsPNG(m_hwndWinMerge, getScreenshotFilePath(id)));
	}

	void saveForegroundDialogImage(const char *id = "")
	{
		HWND hwndDlg = GUITestUtils::findForegroundDialog();
		ASSERT_TRUE(hwndDlg != nullptr);
		if (hwndDlg)
			ASSERT_TRUE(GUITestUtils::saveWindowImageAsPNG(hwndDlg, getScreenshotFilePath(id)));
	}

protected:
	static HWND m_hwndWinMerge;
};
