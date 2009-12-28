#pragma once

void CreateToolbars();

void CleanToolbars();

BOOL FileExists(CString sFileName);

__declspec(dllexport) int WINAPI ReCreateToolBar(CStringArray* pArray);
