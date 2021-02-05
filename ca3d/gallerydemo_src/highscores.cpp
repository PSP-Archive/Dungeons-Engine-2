// highscores.cpp

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

#include "highscores.h"
#include <iostream>
#include <fstream>
using namespace std;

CHighscores highscores[1];

void CHighscores::fill()
{
	scores.clear();
	
	time_t t = getCurrentTime();

	for (unsigned int i=0; i<10; i++)
	{
		score_t score;

		score.fScore = 999.99f;
		score.time   = t;

		scores.push_back(score);
	}
}

void CHighscores::show(CFont *pFont, unsigned int color)
{
	int i=0;

	vector<score_t>::iterator it;

	pFont->draw( "Top times:", 20, 20, color, TEXT_LEFT);

	for (it = scores.begin(); it != scores.end(); it++)
	{
		char *szTime = ctime(&(*it).time);
		char szTimeOhne[30] = {0};
		char szScore[30] = {0};
		char szPosition[5] = {0};
		
		for (unsigned int source=0, target=0; source<strlen(szTime); source++)
		{
			// szTimeOhne enthält nicht den Zeilenvorschub...
			if (szTime[source] != '\n') szTimeOhne[target++] = szTime[source];
		}

		sprintf(szScore, "%.2f", (*it).fScore);
		sprintf(szPosition, "%i:", i+1);

		pFont->draw( szPosition,	 50,  50+(18*i), color, TEXT_RIGHT);
		pFont->draw( szScore,		120,  50+(18*i), color, TEXT_RIGHT);
		pFont->draw( szTimeOhne,	140,  50+(18*i), color, TEXT_LEFT);

		i++;
	}
}

int CHighscores::insert(float fScore)
{
	int iTopTenPosition = 0;

	vector<score_t>::iterator it;

	for (it = scores.begin(); it != scores.end(); it++)
	{
		if (fScore <= (*it).fScore)
		{
			score_t score;

			score.fScore = fScore;
			score.time = getCurrentTime();

			vector<score_t>::iterator it2 = scores.begin() + iTopTenPosition;
			
			scores.insert(it2, score);

			scores.pop_back();

			break;
		}

		iTopTenPosition++;
	}

	return (iTopTenPosition+1);
}

float CHighscores::makeChksum()
{
	float chksum = 4.21f;

	vector<score_t>::iterator it;

	for (it = scores.begin(); it != scores.end(); it++)
	{
		score_t temp = *it;
		
		chksum += temp.fScore;
	}

	chksum = sqrtfCa(chksum) * 0.27f + sqrtfCa(chksum);

	return chksum;
}

void CHighscores::save()
{
	char szFilename[200];

	sprintf(szFilename, "%s/%s", SYSTEM_PATH, "highscores.dat");

	ofstream outfile(szFilename, ios::out|ios::trunc|ios::binary);

	vector<score_t>::iterator it;

	for (it = scores.begin(); it != scores.end(); it++)
	{
		score_t temp = *it;

		outfile.write( (char*)&temp, sizeof(score_t) );
	}

	float chksum = makeChksum();

	outfile.write( (char*)&chksum, sizeof(float) );

	outfile.clear();
	outfile.close();
}

void CHighscores::load()
{
	char szFilename[200];

	sprintf(szFilename, "%s/%s", SYSTEM_PATH, "highscores.dat");

	scores.clear();

	ifstream infile(szFilename, ios::in|ios::binary);

	if (infile)
	{
		score_t temp;
		
		for (int i=0; i<10; i++)
		{
			infile.read( (char*)&temp, sizeof(score_t) );

			scores.push_back(temp);
		}
		
		float chksum1, chksum2;

		chksum1 = makeChksum();

		infile.read( (char*)&chksum2, sizeof(float) );

		infile.clear();
		infile.close();

		if (chksum1 != chksum2)
		{
			fill();
			save();
		}
	}
	else
	{
		fill();
		save();
	}
}
