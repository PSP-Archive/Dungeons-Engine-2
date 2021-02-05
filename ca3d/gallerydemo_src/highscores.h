// highscores.h

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

#ifndef CA_HIGHSCORES
#define CA_HIGHSCORES

#include <vector>
#include <ctime>

#include "../caUtil/caUtil.h"

struct score_t
{
	float fScore;
	time_t time;
};

class CHighscores
{
	std::vector<score_t> scores;
	float makeChksum();

public:
	void fill();
	void show(CFont *pFont, unsigned int color);
	int insert(float fScore);
	void save();
	void load();
};

extern CHighscores highscores[];

#endif
