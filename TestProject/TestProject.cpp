// TestProject.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <shlobj_core.h>
void PrintLogicalDrives() {
	char c = 'A';
	DWORD drivers = GetLogicalDrives();
	for (size_t i = 0; i < 26; i++)
	{
		int r = (drivers >> i) & 1;
		if (r) {
			std::cout << "磁盘" << c << "存在" << std::endl;
		}
		c += 1;
	}
}
int main()
{
	std::wstring strLocalAppDataPath = L"";
	TCHAR *szWinDir = nullptr;
	if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_ComputerFolder, 0, NULL, &szWinDir))) {
		strLocalAppDataPath = szWinDir;
		std::wcout << strLocalAppDataPath << std::endl;
		CoTaskMemFree(szWinDir);
		std::wcout << strLocalAppDataPath << std::endl;
		std::wcout << szWinDir << std::endl;
	}
}

