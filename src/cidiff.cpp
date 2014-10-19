#include "ImgDiffBuffer.hpp"
#include <iostream>
#include <clocale>

int main(int argc, char* argv[])
{
	CImgDiffBuffer buffer;
	wchar_t filenameW[2][260];
	const wchar_t *filenames[2] = { filenameW[0], filenameW[1] };

	if (argc < 3)
	{
		std::wcerr << L"usage: cmdidiff image_file1 image_file2" << std::endl;
		exit(1);
	}

	FreeImage_Initialise();
	setlocale(LC_ALL, "");

	mbstowcs(filenameW[0], argv[1], strlen(argv[1]) + 1);
	mbstowcs(filenameW[1], argv[2], strlen(argv[2]) + 1);

	if (!buffer.OpenImages(2, filenames))
	{
		std::wcerr << L"cmdidiff: could not open files. (" << filenameW[0] << ", " << filenameW[1] << L")" << std::endl;
		exit(1);
	}

	buffer.CompareImages();
	buffer.SaveDiffImageAs(1, L"diff.png");
	buffer.CloseImages();

	return 0;
}


