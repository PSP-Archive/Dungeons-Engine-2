// calog.cpp

/*
Dungeons Engine 2

Copyright (C) 2008 Christoph Arnold "charnold" (http://www.caweb.de / charnold@gmx.de)

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "calog.h"
#include "paths.h"
#include "memory.h"
#include <fstream>
using namespace std;

ofstream calog;

void clearLog()
{
	char szFile[256];
	sprintf(szFile, "%s/log.txt", SYSTEM_PATH);
	calog.open(szFile, ios::out|ios::trunc);
	calog.clear();
	calog.close();
}

void appendLog()
{
	char szFile[256];

	if (!strcmp("", SYSTEM_PATH))
		sprintf(szFile, "log.txt");
	else
		sprintf(szFile, "%s/log.txt", SYSTEM_PATH);

	calog.open(szFile, ios::out|ios::app);
}

void closeLog()
{
	calog.clear();
	calog.close();
}

void writeLog(const char* szLine)
{
	appendLog();
	calog << szLine << char(13) << char(10);
	closeLog();
}

void writeFreeRam(const char* szLine)
{
	float freeRam = getFreeRAM();
	appendLog();
	calog << szLine << ": " << freeRam << " MB free" << char(13) << char(10);
	closeLog();
}

int cleanupLine(char *line)
{
	// getline auf PSP lässt char(13) als letztes Zeichen stehen
	// Grund: endl auf PC erzeugt 0x0D 0x0A, endl auf PSP erzeugt nur 0x0A

	int iLen;
	iLen = (int)strlen(line);
	if (line[iLen-1] == char(13)) {
		line[iLen-1] = 0;
		iLen--;
	}
	return iLen;
}
