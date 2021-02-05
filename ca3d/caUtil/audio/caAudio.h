// caAudio.h

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

#ifndef CA_AUDIO_H
#define CA_AUDIO_H

namespace caAudio
{
	enum soundeffects_e
	{
		SOUND_STEP,
		SOUND_SHOOT0,
		SOUND_SHOOT1,
		SOUND_NO_AMMO,
		SOUND_WEAPON_CHANGE,
		SOUND_BUTTON,
		SOUND_GET_AMMO,
		SOUND_ENEMY_SHOOT0,
		SOUND_ENEMY_SHOOT1,
		SOUND_ENEMY_WARNS,
		SOUND_ENEMY_HIT,
		SOUND_ENEMY_SPLAT,
	//	SOUND_MUSIC,
		SOUND_LEVEL_MUSIC,
		ANZAHL_SOUNDS
	};

	void playSound(soundeffects_e sound);
	void stopAllSounds();
	int initAudio();
	void closeAudio();
	void loadWave(soundeffects_e sound, char *file, int iLoop);
	void unloadWave(soundeffects_e sound);
};

#endif
