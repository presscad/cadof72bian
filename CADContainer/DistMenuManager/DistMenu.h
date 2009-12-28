#pragma once

void CreateMenus();

void CleanMenus();

BOOL LoadMenu(LPCTSTR lpMenuName);

BOOL UnLoadMenu(LPCTSTR lpMenuName);

BOOL FileExists(CString sFileName);

//__declspec(dllexport) int WINAPI ReCreateMenu(CStringArray* pArray);
