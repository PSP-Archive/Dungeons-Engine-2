// caAudio.cpp

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

#include "caAudio.h"
#include "../calog.h"
#include "../paths.h"
#include "../memory.h"
#include "../plattform_includes.h"

namespace caAudio
{
	#if __APPLE_CC__ | _WIN32

	#define MAXBUFFERS ANZAHL_SOUNDS
	#define MAXSOURCES ANZAHL_SOUNDS

	ALuint Buffer[MAXBUFFERS];
	ALuint Source[MAXSOURCES];

	ALfloat SourcePos[] = { 0.0, 0.0, 0.0 };
	ALfloat SourceVel[] = { 0.0, 0.0, 0.0 };
	ALfloat ListenerPos[] = { 0.0, 0.0, 0.0 };
	ALfloat ListenerVel[] = { 0.0, 0.0, 0.0 };
	ALfloat ListenerOri[] = { 0.0, 0.0, -1.0,  0.0, 1.0, 0.0 };

	ALboolean loadALData(char* szWavefile, bool looping, int iBuffer, int iSource, float fGain = 1.0f)
	{
		// Variables to load into.
		ALenum format;
		ALsizei size;
		ALvoid* data;
		ALsizei freq;

		// Load wav data into a buffer.
		if (alGetError() != AL_NO_ERROR)
		{
			return AL_FALSE;
		}
		
		#if _WIN32
			ALboolean loop;
			alutLoadWAVFile(szWavefile, &format, &data, &size, &freq, &loop);
		#else
			alutLoadWAVFile(szWavefile, &format, &data, &size, &freq);
		#endif
		
		alBufferData(Buffer[iBuffer], format, data, size, freq);
		alutUnloadWAV(format, data, size, freq);

		// Bind the buffer with the source.
		if (alGetError() != AL_NO_ERROR)
		{
			return AL_FALSE;
		}

		alSourcef(Source[iSource],AL_ROLLOFF_FACTOR,0);
		alSourcei(Source[iSource],AL_SOURCE_RELATIVE,AL_TRUE);

		alSourcei (Source[iSource], AL_BUFFER,   Buffer[iBuffer]);
		alSourcef (Source[iSource], AL_PITCH,    1.0      );
		alSourcef (Source[iSource], AL_GAIN,     fGain    );
		alSourcefv(Source[iSource], AL_POSITION, SourcePos);
		alSourcefv(Source[iSource], AL_VELOCITY, SourceVel);
		alSourcei (Source[iSource], AL_LOOPING,  looping?1:0);

		// Do another error check and return.
		if (alGetError() == AL_NO_ERROR)
		{
			return AL_TRUE;
		}

		return AL_FALSE;
	}

	void setListenerValues()
	{
		alListenerfv(AL_POSITION,    ListenerPos);
		alListenerfv(AL_VELOCITY,    ListenerVel);
		alListenerfv(AL_ORIENTATION, ListenerOri);
	}

	void playSound(soundeffects_e sound)
	{
		alSourcePlay(Source[sound]);
	}

	void stopAllSounds()
	{
		for (int i=0; i<MAXSOURCES; i++)
			alSourceStop(Source[i]);
	}

	void closeAudio()
	{
		alDeleteSources(MAXSOURCES, &Source[0]);
		alDeleteBuffers(MAXBUFFERS, &Buffer[0]);
		alutExit();
	}

	int initAudio()
	{
		// Initialize OpenAL and clear the error bit.
		alutInit(NULL, 0);
		alGetError();

		// Load the wav data.
		alGenBuffers(MAXBUFFERS, &Buffer[0]);
		alGenSources(MAXSOURCES, &Source[0]);

		setListenerValues();

		// Setup an exit procedure.
		atexit(closeAudio);

		return 0;
	}

	void loadWave(soundeffects_e sound, char *file, int iLoop)
	{
		char path_name[256];

		sprintf(path_name, "%s/%s", AUDIO_PATH, file);

		if (loadALData(path_name, (!iLoop?false:true), sound, sound, 1.0f) == AL_FALSE)
		{
			cout << "error loading audio" << endl;
			return;
		}
		else {
			cout << "loading audio ok: " << path_name << endl;
		}
	}

	void unloadWave(soundeffects_e sound)
	{
		alDeleteSources(1, &Source[sound]);
		alDeleteBuffers(1, &Buffer[sound]);
		alGenBuffers(1, &Buffer[sound]);
		alGenSources(1, &Source[sound]);
	}

	#elif _PSP

	typedef struct
	{
		unsigned long channels;
		unsigned long samplerate;
		unsigned long samplecount;
		unsigned long datalength;
		unsigned long rateratio;	// samplerate / 44100 * 0x10000
		unsigned long playptr;
		unsigned long playptr_frac;
		int           playloop;
		char          *wavdata;
	} wavout_wavinfo_t;

	enum soundslots_e
	{
		SLOT_1,
		SLOT_2,
		SLOT_3 // ,
//		SLOT_4
	};

	wavout_wavinfo_t *wavLoad(char *filename, int bLoop);

	void SetChannelCallback(unsigned int channel);
	void RemoveChannelCallback(unsigned int channel);

	#define SND_MAXSLOT 3 // 4 // 3 // 16

	wavout_wavinfo_t* wav_infos[ANZAHL_SOUNDS];

	wavout_wavinfo_t wavout_snd_wavinfo[SND_MAXSLOT];
	int wavout_snd_playing[SND_MAXSLOT];
	int snd_playing_prio[SND_MAXSLOT];

	short *samples;
	unsigned long req = 0;

	void wavout_snd_callback(void *_buf, unsigned int _reqn, void *pdata)
	{
		unsigned int i,slot;
		wavout_wavinfo_t *wi;
		unsigned long ptr,frac;
		short *buf = (short*)_buf;

		samples = (short*)_buf;
		req = _reqn;

	//	for (i=0; i<1; i++)
		for (i=0; i<_reqn; i++)
		{
			int outr = 0, outl = 0;

	//		for (slot = 0; slot < 1; slot++)
			for (slot = 0; slot < SND_MAXSLOT; slot++)
			{
				if (!wavout_snd_playing[slot]) continue;

				wi = &wavout_snd_wavinfo[slot];

	//			frac = wi->playptr_frac + wi->rateratio*1024;
				frac = wi->playptr_frac + wi->rateratio;

	//			wi->playptr=ptr=wi->playptr + (frac>>16);
				wi->playptr=ptr = wi->playptr + (frac>>16);
				wi->playptr_frac = (frac & 0xffff);

				if (ptr >= wi->samplecount)
				{
					if (wi->playloop)
					{ // ca
						wi->playptr = 0;
						wi->playptr_frac = 0;
					}
					else
					{
						wavout_snd_playing[slot] = 0;
					}
					break;
				}
				
				short *src = (short *)wi->wavdata;
				
				if (wi->channels==1)
				{
					outl += src[ptr];
					outr += src[ptr];
				}
				else
				{
					outl += src[ptr*2];
					outr += src[ptr*2+1];
				}
			}

			if		(outl < -32768)	outl= -32768;
			else if (outl >  32767)	outl=  32767;

			if		(outr < -32768)	outr= -32768;
			else if (outr >  32767)	outr=  32767;

			*(buf++) = outl;
			*(buf++) = outr;
		}
	}

	void wavHalt(char *string, char *name)
	{
		char text[200];

		sprintf(text, "Problems with wav: %s", name);

		writeLog(text);
	}

	void SetChannelCallback(unsigned int channel)
	{
		pspAudioSetChannelCallback(0, wavout_snd_callback, NULL);
	}

	void RemoveChannelCallback(unsigned int channel)
	{
		pspAudioSetChannelCallback(0, 0, NULL);
	}

	int wavInit()
	{
		int i;

		pspAudioInit();
		pspAudioSetChannelCallback(0, wavout_snd_callback, NULL);

		for (i=0; i<SND_MAXSLOT; i++)
		{
			wavout_snd_playing[i]=0;
			snd_playing_prio[i]=0;
		}

		for (i=0; i<ANZAHL_SOUNDS; i++)
			wav_infos[i] = 0;

		return 0;
	}

	//stop all
	void wavoutStopPlay()
	{
		int i;

		for (i=0; i<SND_MAXSLOT; i++)
		{
			wavout_snd_playing[i]=0;
			snd_playing_prio[i]=0;
		}
	}

	//return 0 if success
	int wavPlay(wavout_wavinfo_t *wi, int iSlot, int prio)
	{
		int i = iSlot;

		wavout_wavinfo_t *wid;

		//for (i=0; i<SND_MAXSLOT; i++) if (wavout_snd_playing[i]==0) break;
		
		if (wavout_snd_playing[i] != 0 && snd_playing_prio[i] > prio) return 0;

		wavout_snd_playing[i] = 0;

		if (i==SND_MAXSLOT) return -1;
		
		wid=&wavout_snd_wavinfo[i];
		wid->channels=wi->channels;
		wid->samplerate=wi->samplerate;
		wid->samplecount=wi->samplecount;
		wid->datalength=wi->datalength;
		wid->wavdata=wi->wavdata;
		wid->rateratio=wi->rateratio;
		wid->playptr=0;
		wid->playptr_frac=0;
		wid->playloop=wi->playloop;
		
		wavout_snd_playing[i]=1;
		
		snd_playing_prio[i]=prio;
		
		return 0;
	}

	//return waveinfo * if success
	wavout_wavinfo_t *wavLoad(char *filename, int bLoop)
	{
		unsigned int filelen;
		int fd;
		unsigned long channels;
		unsigned long samplerate;
		unsigned long blocksize;
		unsigned long bitpersample;
		unsigned long datalength;
		unsigned long samplecount;

		char *wavfile;
		char path_name[256];

		wavout_wavinfo_t *wi;

		SceIoStat stat;

		sprintf(path_name, "%s/%s", AUDIO_PATH, filename);

		fd = sceIoOpen(path_name, PSP_O_RDONLY, 0777);

		if (fd < 0)
		{
			wavHalt("file failed",path_name);
			return NULL;
		}

		sceIoGetstat(path_name, &stat);

		wi = (wavout_wavinfo_t*) memoryAllocate((stat.st_size*2) + sizeof(wavout_wavinfo_t));

		wavfile = (char*)(wi + sizeof(wavout_wavinfo_t));
		filelen = sceIoRead(fd, wavfile, stat.st_size);
		sceIoClose(fd);

		if (memcmp(wavfile, "RIFF", 4)!=0)
		{
			wavHalt("format error not RIFF", path_name);
			memoryFree(wi);
			return NULL;
		}

		if (memcmp(wavfile+8, "WAVEfmt \x10\x00\x00\x00\x01\x00", 14) != 0)
		{
			wavHalt("format error no WAVEfmt string", path_name);
			memoryFree(wi);
			return NULL;
		}

		channels		= *(short *)(wavfile+0x16);
		samplerate		= *(long *)(wavfile+0x18);
		blocksize		= *(short *)(wavfile+0x20);
		bitpersample	= *(short *)(wavfile+0x22);

		if (memcmp(wavfile+0x24, "data", 4)!=0)
		{
			wavHalt("no data chunk found", path_name);
			memoryFree(wi);
			return NULL;
		}

		datalength=*(unsigned long *)(wavfile+0x28);

		if (datalength+0x2c>filelen)
		{
			wavHalt("buffer is supposed to be bigger than this", path_name);
			memoryFree(wi);
			return NULL;
		}

		if (channels!=2 && channels!=1)
		{
			wavHalt("not Mono or Stereo sample", path_name);
			memoryFree(wi);
			return NULL;
		}

		if (samplerate>100000 || samplerate<2000)
		{
			wavHalt("sample rate is wrong", path_name);
			memoryFree(wi);
			return NULL;
		}

		if (blocksize!=channels*2)
		{
			wavHalt("BLOCKSIZE MISMATCH", path_name);
			memoryFree(wi);
			return NULL;
		}

		if (bitpersample!=16)
		{
			wavHalt("Bits Per Sample Error", path_name);
			memoryFree(wi);
			return NULL;
		}

		if (channels==2)
		{
			samplecount=datalength/4;
		}
		else
		{
			samplecount=datalength/2;
		}

		if (samplecount<=0)
		{
			wavHalt("no samples", path_name);
			memoryFree(wi);
			return NULL;
		}

		wi->channels=channels;
		wi->samplerate=samplerate;
		wi->samplecount=samplecount;
		wi->datalength=datalength;
		wi->wavdata=wavfile+0x2c;
		wi->rateratio=(samplerate*0x4000)/11025;
		wi->playptr=0;
		wi->playptr_frac=0;
		wi->playloop=bLoop;

		return wi;
	}

	void playSound(soundeffects_e sound)
	{
		soundslots_e slot;
		int prio;

		switch (sound)
		{	
			// SLOT 1

			case SOUND_ENEMY_WARNS:
				slot = SLOT_1; prio = 10;
				break;

			case SOUND_ENEMY_SHOOT0:
			case SOUND_ENEMY_SHOOT1:
				slot = SLOT_1; prio = 5;
				break;
			
	//		case SOUND_MUSIC:
	//			slot = SLOT_1; prio = 100;
	//			break;

			// SLOT 2

			case SOUND_STEP:
				slot = SLOT_2; prio = 0;
				break;
			
			case SOUND_GET_AMMO:
				slot = SLOT_2; prio = 2;
				break;

			case SOUND_ENEMY_HIT:
				slot = SLOT_2; prio = 4;
				break;

			case SOUND_ENEMY_SPLAT:
				slot = SLOT_2; prio = 6;
				break;
			
			case SOUND_BUTTON:
				slot = SLOT_2; prio = 10;
				break;

			// SLOT 3

			case SOUND_SHOOT0:
			case SOUND_SHOOT1:
			case SOUND_NO_AMMO:
			case SOUND_WEAPON_CHANGE:
				slot = SLOT_3; prio = 0;
				break;

			case ANZAHL_SOUNDS:
				break;

			// SLOT 4

			case SOUND_LEVEL_MUSIC:
//				slot = SLOT_4; prio = 0;
				break;

		}

		wavPlay(wav_infos[sound], slot, prio);
	}

	void stopAllSounds()
	{
		wavoutStopPlay();
	}

	void closeAudio()
	{
	}

	int initAudio()
	{
		return wavInit();
	}

	void loadWave(soundeffects_e sound, char *file, int iLoop)
	{
		wav_infos[sound] = wavLoad(file, iLoop);
	}

	void unloadWave(soundeffects_e sound)
	{
		if (wav_infos[sound])
		{
			memoryFree(wav_infos[sound]);
			wav_infos[sound] = 0;
		}
	}

	#endif
};
