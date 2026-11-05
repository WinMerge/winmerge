/**
 * @file  NonInteractiveTest.cpp
 *
 * @brief Tests for unattended runs (/noninteractive).
 *
 * An unattended run must end on its own: nobody is there to answer a prompt.
 */

#include "pch.h"
#include <fstream>

namespace
{

	using namespace GUITestUtils;

	class NonInteractiveTest : public testing::Test
	{
	protected:
		void SetUp() override
		{
			m_dir = std::filesystem::temp_directory_path() / L"WinMergeNonInteractiveTest";
			std::filesystem::remove_all(m_dir);
			std::filesystem::create_directories(m_dir);
		}

		void TearDown() override
		{
			std::error_code ec;
			std::filesystem::remove_all(m_dir, ec);
		}

		std::filesystem::path write(const wchar_t* name, const std::string& text)
		{
			const std::filesystem::path path = m_dir / name;
			std::ofstream(path, std::ios::binary) << text;
			return path;
		}

		static std::string read(const std::filesystem::path& path)
		{
			std::ifstream file(path, std::ios::binary);
			return std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
		}

		/** @brief Auto-merge the left and the right file into the middle one, unattended. */
		bool autoMerge(const std::wstring& options, const std::filesystem::path& output, const wchar_t* extension = L".txt")
		{
			const std::wstring args = L"/noprefs /noninteractive /minimize /u /am " + options +
				L" /o \"" + output.wstring() + L"\" \"" + (m_dir / L"left").wstring() + extension + L"\" \"" +
				(m_dir / L"middle").wstring() + extension + L"\" \"" + (m_dir / L"right").wstring() + extension + L"\"";
			return execWinMergeAndWait(args, 30000);
		}

		std::filesystem::path m_dir;
	};

	const wchar_t* const WithResultPane = L"/cfg Settings/MergeResultPaneEnabled=1";
	const wchar_t* const WithoutResultPane = L"/cfg Settings/MergeResultPaneEnabled=0";

	TEST_F(NonInteractiveTest, AutoMergeWritesOutputAndExits)
	{
		write(L"middle.txt", "a\nb\nc\nd\ne\n");
		write(L"left.txt", "a2\nb\nc\nd\ne\n");
		write(L"right.txt", "a\nb\nc\nd\ne2\n");
		for (const wchar_t* options : { WithResultPane, WithoutResultPane })
		{
			const std::filesystem::path output = m_dir / L"output.txt";
			std::filesystem::remove(output);
			ASSERT_TRUE(autoMerge(options, output)) << "WinMerge did not exit";
			EXPECT_EQ("a2\nb\nc\nd\ne2\n", read(output));
		}
	}

	TEST_F(NonInteractiveTest, AutoMergeWithConflictExits)
	{
		write(L"middle.txt", "a\nb\nc\nd\ne\n");
		write(L"left.txt", "a2\nb\nc1\nd\ne\n");
		write(L"right.txt", "a\nb\nc2\nd\ne2\n");
		for (const wchar_t* options : { WithResultPane, WithoutResultPane })
		{
			const std::filesystem::path output = m_dir / L"output.txt";
			std::filesystem::remove(output);
			ASSERT_TRUE(autoMerge(options, output)) << "WinMerge did not exit";
			// what could be merged is written; the conflict is still in the output
			const std::string merged = read(output);
			EXPECT_NE(std::string::npos, merged.find("a2\n"));
			EXPECT_NE(std::string::npos, merged.find("e2\n"));
		}
	}

	TEST_F(NonInteractiveTest, ImageAutoMergeWritesOutputAndExits)
	{
		// three images of which only the right one differs
		const std::filesystem::path data = getModuleFolder() / L"..\\..\\Data\\Compare";
		const wchar_t* const image = L"file123_diff3only.png";
		ASSERT_TRUE(std::filesystem::exists(data / L"Dir1" / image));
		std::filesystem::copy_file(data / L"Dir1" / image, m_dir / L"left.png");
		std::filesystem::copy_file(data / L"Dir2" / image, m_dir / L"middle.png");
		std::filesystem::copy_file(data / L"Dir3" / image, m_dir / L"right.png");

		const std::filesystem::path output = m_dir / L"output.png";
		ASSERT_TRUE(autoMerge(L"", output, L".png")) << "WinMerge did not exit";
		EXPECT_TRUE(std::filesystem::exists(output));
	}

}
