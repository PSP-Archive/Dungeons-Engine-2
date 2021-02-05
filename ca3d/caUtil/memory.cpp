// memory.cpp

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

#include "memory.h"
#include "plattform_includes.h"
#include "calog.h"
#include <map>

using namespace std;

namespace memoryInspector
{
	map<void*, int> allocatedMemory;

	int iAllocatedMemory;
	
	void reset()
	{
		iAllocatedMemory = 0;
		allocatedMemory.clear();
	}

	void allocate(void* ptr, int size)
	{
		allocatedMemory.insert(make_pair(ptr, size));
		iAllocatedMemory += size;
	}

	void free(void* ptr)
	{
		map<void*, int>::iterator it = allocatedMemory.find(ptr);
		
		if (it != allocatedMemory.end())
		{
			iAllocatedMemory -= (*it).second;
			allocatedMemory.erase(it);
		}
		else
		{
			cout << "didn't find allocated memory for ptr: " << ptr << endl;
		}
	}

	void report(char *text)
	{
		cout << "****************************************************************************" << endl;
		cout << "Reporting allocated memory: " << iAllocatedMemory << " byte (" << text << ")" << endl;
		cout << "****************************************************************************" << endl;
	}
	
};

#if __APPLE_CC__ | _WIN32

void* memoryAllocate(unsigned int size)
{
	void* val;
	val = malloc(size);
	
//	memoryInspector::allocate(val, size);
	
	return val;
}

void memoryFree(void* m)
{

//	memoryInspector::free(m);

	free(m);
}

float getFreeRAM()
{
	return 0;
}

void printMemoryInfo(char* szInfotext)
{
}

#elif _PSP

void* memoryAllocate(unsigned int size)
{
	void* val;
	val = memalign(64, size);
	
	if (!val) {

		clock_t frameticks, currentticks, startticks = clock();
		float fElapsedTime = 0;

		pspDebugScreenInit();
		pspDebugScreenPrintf("out of memory, program will exit now ...");
		
		while (1) {
			currentticks = clock();
			frameticks = (currentticks - startticks);
			fElapsedTime += float(frameticks/(float)CLOCKS_PER_SEC);
			startticks = currentticks;

			if (fElapsedTime > 6)
				sceKernelExitGame();
		}

	}

	return val;
}

void memoryFree(void* m)
{
	free(m);
}

float getFreeRAM()
{
	float ram; 
	int ramAdd[320]; 

	for (int i=0; i<320; i++)
	{ 
		ramAdd[i] = (int)malloc(100000);
		if (ramAdd[i] == 0)
		{
			ram = (float)i; 
			for (int z=i; z>=0; z--)
			{
				free((void*)ramAdd[z]);
			}
			break;
		}
	}
	return ram/10;
}

void printMemoryInfo(char* szInfotext)
{
	char text[200];
	struct mallinfo mi;

	// int arena;    /* total space allocated from system */
	// int ordblks;  /* number of non-inuse chunks */
	// int smblks;   /* unused -- always zero */
	// int hblks;    /* number of mmapped regions */
	// int hblkhd;   /* total space in mmapped regions */
	// int usmblks;  /* unused -- always zero */
	// int fsmblks;  /* unused -- always zero */
	// int uordblks; /* total allocated space */
	// int fordblks; /* total non-inuse space */
	// int keepcost; /* top-most, releasable (via malloc_trim) space */

	mi = mallinfo();
	sprintf(text, "%s: arena: %i, ordblks: %i, hblks: %i, hblkhd: %i, uordblks: %i, fordblks: %i, keepcost: %i", 
		szInfotext, mi.arena,  mi.ordblks,  mi.hblks,  mi.hblkhd,  mi.uordblks,  mi.fordblks,  mi.keepcost);
	writeLog(text);
}

#endif
