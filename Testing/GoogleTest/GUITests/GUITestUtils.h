#include <Windows.h>
#include <filesystem>
#include <set>

namespace GUITestUtils
{
	time_t getStartTime();
	bool saveWindowImageAsPNG(HWND hwnd, const std::filesystem::path& filename);
	DWORD waitForInputIdleByHWND(HWND hwnd, DWORD dwMilliseconds = WAIT_TIMEOUT);
	HWND findForegroundDialog();
	void typeText(const wchar_t *text);
	void typeAltPlusKey(char key);
	void typeKey(unsigned char vk);
	std::filesystem::path getModuleFileName();
	std::filesystem::path getModuleFolder();
	bool isMenuItemChecked(HWND hwnd, int id);
	void waitUntilClose(HWND hwnd);
	void waitUntilFocus(HWND hwnd);
	void waitUntilProcessExit(HWND hwnd);
	void selectMenu(HWND hwnd, unsigned id, bool async = false);
	inline void selectMenuAsync(HWND hwnd, unsigned id) { selectMenu(hwnd, id, true); };
	HWND execWinMerge(const std::string& args = "/noprefs /maxmize");
	const std::set<int> languages();
}

#define selectMenuAndSaveWindowImage(id) selectMenuAndSaveWindowImageHelper(id, #id)
#define selectOpenDialogMenuAndSaveDialogImage(id) selectOpenDialogMenuAndSaveDialogImageHelper(id, #id)

class CommonTest : public testing::TestWithParam<int>
{
public:
	void selectMenu(UINT id)
	{
		GUITestUtils::selectMenu(m_hwndWinMerge, id);
	}

	void selectMenuAsync(UINT id)
	{
		GUITestUtils::selectMenu(m_hwndWinMerge, id, true);
	}

	void selectMenuAndSaveWindowImageHelper(UINT id, const char *str)
	{
		selectMenu(id);
		Sleep(500);
		if (GUITestUtils::isMenuItemChecked(m_hwndWinMerge, id))
			saveImage((std::string(str) + ".Checked").c_str());
		else
			saveImage(str);
	}

	HWND selectOpenDialogMenu(UINT id)
	{
		GUITestUtils::selectMenuAsync(m_hwndWinMerge, id);
		HWND hwndDlg = GUITestUtils::findForegroundDialog();
		return hwndDlg;
	}

	HWND selectOpenDialogMenuAndSaveDialogImageHelper(UINT id, const char *str)
	{
		HWND hwnd = selectOpenDialogMenu(id);
		Sleep(500);
		saveForegroundDialogImage(str);
		return hwnd;
	}

	std::filesystem::path getScreenshotFolderPath()
	{
		std::filesystem::path screenshotFolder;
		char buf[256];
		struct tm tm;
		time_t t = GUITestUtils::getStartTime();
		localtime_s(&tm, &t);
		strftime(buf, sizeof buf, "%FT%H%M%S", &tm);
		screenshotFolder = GUITestUtils::getModuleFolder() / L"..\\..\\..\\Build\\GUITests\\Screenshots" / buf / std::to_wstring(GetParam());
		std::error_code ec;
		std::filesystem::create_directories(screenshotFolder, ec);
		return screenshotFolder;
	}

	std::filesystem::path getScreenshotFilePath(const char *id = nullptr)
	{
		std::string basename = ::testing::UnitTest::GetInstance()->current_test_info()->test_case_name();
		basename.append(".");
		basename.append(::testing::UnitTest::GetInstance()->current_test_info()->name());
		if (id && id[0])
		{
			basename.append(".");
			basename.append(id);
		}
		for (auto& c : basename)
		{
			if (c == '/')
				c = '_';
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

	void saveForegroundWindowImage(const char *id = "")
	{
		HWND hwnd = GetForegroundWindow();
		ASSERT_TRUE(hwnd != nullptr);
		if (hwnd)
			ASSERT_TRUE(GUITestUtils::saveWindowImageAsPNG(hwnd, getScreenshotFilePath(id)));
	}

protected:
	HWND m_hwndWinMerge = nullptr;
};
