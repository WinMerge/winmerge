#include "pch.h"
#include <gtest/gtest.h>
#include "markdown.h"

namespace
{
	// The fixture for testing paths functions.
	class MarkdownTest : public testing::Test
	{
	protected:
		// You can remove any or all of the following functions if its body
		// is	empty.

		MarkdownTest()
		{
			// You can do set-up work for each test	here.
		}

		virtual ~MarkdownTest()
		{
			// You can do clean-up work	that doesn't throw exceptions here.
		}

		// If	the	constructor	and	destructor are not enough for setting up
		// and cleaning up each test, you can define the following methods:

		virtual void SetUp()
		{
			// Code	here will be called	immediately	after the constructor (right
			// before each test).
		}

		virtual void TearDown()
		{
			// Code	here will be called	immediately	after each test	(right
			// before the destructor).
		}

		// Objects declared here can be used by all tests in the test case for Foo.
	};

	TEST_F(MarkdownTest, MarkdownTest1)
	{
		const String html = 
			_T("<!DOCTYPE html>\r\n")
            _T("<html>\r\n")
            _T("<head>\r\n")
            _T("<meta charset=\"UTF-8\">\r\n")
            _T("<title>HTML5</title>\r\n")
            _T("</head>\r\n")
            _T("<body>\r\n")
            _T("<div id='abc'>HTML5 test</div>\r\n")
            _T("</body>\r\n")
            _T("</html>\r\n");

		CMarkdown::File fi(html.c_str(), html.length() * sizeof(tchar_t), CMarkdown::Html|CMarkdown::FileImage::Mapping|CMarkdown::FileImage::Octets);
		fi.Move("meta");
		EXPECT_EQ("meta", fi.GetTagName());
		std::string content = fi.GetAttribute("context");
		EXPECT_EQ("", content);
		std::string charset = fi.GetAttribute("charset");
		EXPECT_EQ("UTF-8", charset);

		CMarkdown &pt = fi.Move("div");
		EXPECT_EQ("div", pt.GetTagName());
		EXPECT_EQ("div id='abc'", pt.GetTagText());
		EXPECT_EQ("<div id='abc'>HTML5 test</div>", pt.GetOuterText());
		EXPECT_EQ("HTML5 test", pt.GetInnerText());
	}


}  // namespace
