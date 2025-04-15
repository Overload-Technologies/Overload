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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "sndbuffer.h"
#include "sndrender.h"
#include "sndchip.h"
#include "chipplayer.h"
#include "soloud_ay.h"
#include "soloud_file.h"
#include "zx7decompress.h"


namespace SoLoud
{

	AyInstance::AyInstance(Ay *aParent)
	{
		mPos = 0;
		mParent = aParent;
		mChip = new ChipPlayer(this);
	}

	unsigned int AyInstance::getAudio(float *aBuffer, unsigned int aSamplesToRead, unsigned int /*aBufferSize*/)
	{
		int samples = mChip->play(aBuffer, aSamplesToRead);
		return samples;
	}
	
	bool AyInstance::hasEnded()
	{
		return mParent->mLength <= mPos;
	}

	result AyInstance::rewind()
	{
		mPos = 0;
		return SO_NO_ERROR;
	}

	float AyInstance::getInfo(unsigned int aInfoKey)
	{
		if ((aInfoKey & 0xf) <= 13)
		{
			if (aInfoKey < 0x10)
				return mChip->chip.get_reg(aInfoKey);
			if (aInfoKey & 0x10)
				return mChip->chip2.get_reg(aInfoKey & 0xf);
		}
		return 0;
	}

	AyInstance::~AyInstance()
	{
		delete mChip;
	}

	Ay::Ay()
	{
		mBaseSamplerate = 44100;
		mChannels = 2;
		mOps = 0;
		mYm = false;
		mChipspeed = 1774400;
		mCpuspeed = 50;
		mLooppos = 0;
		mLength = 0;
	}

	Ay::~Ay()
	{
		stop();
		delete[] mOps;
	}

	result Ay::loadMem(const unsigned char *aMem, unsigned int aLength, bool aCopy, bool aTakeOwnership)
	{
		if (!aMem || aLength == 0)
			return INVALID_PARAMETER;
		MemoryFile mf;
		int res = mf.openMem(aMem, aLength, aCopy, aTakeOwnership);
		if (res != SO_NO_ERROR)
		{
			return res;
		}
		res = loadFile(&mf);
		return res;
	}

	result Ay::load(const char *aFilename)
	{
		if (!aFilename)
			return INVALID_PARAMETER;
		DiskFile df;
		int res = df.open(aFilename);
		if (res != SO_NO_ERROR)
		{
			return res;
		}
		res = loadFile(&df);

		return res;
	}

	result Ay::loadFile(File *aFile)
	{
		if (aFile == NULL)
			return INVALID_PARAMETER;
		// Expect a file wih header and at least one reg write
		if (aFile->length() < 34) return FILE_LOAD_FAILED;

		aFile->seek(0);
		if (aFile->read32() != 'PIHC') return FILE_LOAD_FAILED; // CHIP
		if (aFile->read32() != 'ENUT') return FILE_LOAD_FAILED; // TUNE
		int dataofs = aFile->read16();
		int chiptype = aFile->read8();
		// check if this file is for AY / YM, turbosound or turbosound next
		if (!(chiptype == 1 || chiptype == 2 || chiptype == 3)) return FILE_LOAD_FAILED;
		int flags = aFile->read8();
		int kchunks = aFile->read16();
		int lastchunk = aFile->read16();
		mLength = (kchunks - 1) * 1024 + lastchunk;
		mLooppos = aFile->read16() * 1024 + aFile->read16();
		mCpuspeed = aFile->read32();
		mChipspeed = aFile->read32();
		mYm = false;
		if (flags & 64) mYm = true;
		mOps = new unsigned short[mLength];
		aFile->seek(dataofs);
		if (flags & 1)
		{
			// uncompressed
			aFile->read((unsigned char*)mOps, mLength);
		}
		else
		{
			// compressed
			int len = aFile->length() - dataofs;
			unsigned char* buf = new unsigned char[len];
			aFile->read(buf, len);
			int bufofs = 0;
			for (int i = 0; i < kchunks; i++)
			{
				bufofs += zx7_decompress(buf + bufofs, ((unsigned char*)mOps) + i * 1024);
			}
			delete[] buf;
		}
		return SO_NO_ERROR;
	}

	AudioSourceInstance * Ay::createInstance() 
	{
		return new AyInstance(this);
	}

};