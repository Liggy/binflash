#pragma once

#define UseThread 1
struct ThreadInfo
{
   bool finished;
   bool flash; // true for flashing - false for dumping
   char* filename;
   char* device;
   ErrorCode rc;
};


DWORD WINAPI ThreadHandler(LPVOID lpParam);
LRESULT CALLBACK MainDialog(HWND, UINT, WPARAM, LPARAM);
void ProcessMessages(bool* finished);
