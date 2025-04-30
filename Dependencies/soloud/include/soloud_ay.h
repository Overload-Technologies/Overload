/*
AY module for SoLoud audio engine
Copyright (c) 2020 Jari Komppa

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

   1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.

   2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.

   3. This notice may not be removed or altered from any source
   distribution.
*/

#ifndef AY_H
#define AY_H

#include "soloud.h"

class ChipPlayer;

namespace SoLoud
{
    class Ay;
	class File;
	class AyInstance : public AudioSourceInstance
	{
	public:
		Ay *mParent;
		ChipPlayer *mChip;
		int mPos;

		AyInstance(Ay *aParent);
		~AyInstance();
		virtual unsigned int getAudio(float *aBuffer, unsigned int aSamplesToRead, unsigned int aBufferSize);
		virtual bool hasEnded();
		virtual result rewind();
		virtual float getInfo(unsigned int aInfoKey);
	};

	class Ay : public AudioSource
	{
	public:
		bool mYm;
		int mChipspeed;
		int mCpuspeed;
		int mLooppos;
		int mLength;
		unsigned short* mOps;
	public:
		Ay();
		~Ay();
		result load(const char *aFilename);
		result loadFile(File *aFile);
		result loadMem(const unsigned char* aMem, unsigned int aLength, bool aCopy, bool aTakeOwnership);
		virtual AudioSourceInstance *createInstance();
	};
};

#endif