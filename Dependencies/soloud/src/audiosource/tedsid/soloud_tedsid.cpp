/*
TED/SID module for SoLoud audio engine
Copyright (c) 2015-2020 Jari Komppa

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
#include "sid.h"
#include "ted.h"
#include "soloud_tedsid.h"
#include "soloud_file.h"
#include "zx7decompress.h"

namespace SoLoud
{

	TedSidInstance::TedSidInstance(TedSid *aParent)
	{
		mParent = aParent;
		mSampleCount = 0;
		mSID = new SIDsound(mParent->mModel, 0);
		mSID->setFrequency(0);
		mSID->setSampleRate(TED_SOUND_CLOCK);		
		mSID->setFrequency(1);

		mTED = new TED();
		mTED->oscillatorInit();

		int i;
		for (i = 0; i < 128; i++)
			mRegValues[i] = 0;

		mPos = 0;
	}

	unsigned int TedSidInstance::getAudio(float *aBuffer, unsigned int aSamplesToRead, unsigned int /*aBufferSize*/)
	{
		unsigned int i;
		for (i = 0; i < aSamplesToRead; i++)
		{
		    tick();
			short sample;
			mSID->calcSamples(&sample, 1);
			short tedsample = 0;
			mTED->renderSound(1, &tedsample);
			aBuffer[i] = (sample + tedsample) / 8192.0f;
			mSampleCount--;
		}
		return aSamplesToRead;
	}
	
	void TedSidInstance::tick()
	{
	    if (mParent->mOps == 0)
	        return;

		while (mSampleCount == 0)
		{
			unsigned short op = mParent->mOps[mPos / 2];
			mPos += 2;
			if (mPos >= mParent->mLength) mPos = mParent->mLooppos;
			if (op & 0x8000)
			{
				mSampleCount = op & 0x7fff;
			}
			else
			{
				int reg = (op >> 8) & 0xff;
				int val = op & 0xff;
				mRegValues[reg] = val;
				if (reg < 64)
				{
					mSID->write(reg, val);
				}
				else
				if (reg < 64 + 5)
				{
					mTED->writeSoundReg(reg - 64, val);
				}
			}
		}
	}

	float TedSidInstance::getInfo(unsigned int aInfoKey)
	{
		return (float)mRegValues[aInfoKey & 127];
	}

	bool TedSidInstance::hasEnded()
	{
		return 0;
	}

	TedSidInstance::~TedSidInstance()
	{
		delete mSID;
		delete mTED;
	}

	TedSid::TedSid()
	{
		mBaseSamplerate = TED_SOUND_CLOCK;
		mChannels = 1;
		mOps = 0;
		mModel = 0;
		mLength = 0;
		mLooppos = 0;
	}

	TedSid::~TedSid()
	{
		stop();
		delete[] mOps;
		mOps = 0;
	}

	result TedSid::loadMem(const unsigned char *aMem, unsigned int aLength, bool aCopy, bool aTakeOwnership)
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

	result TedSid::load(const char *aFilename)
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

	result TedSid::loadFile(File *aFile)
	{
		if (aFile == NULL)
			return INVALID_PARAMETER;
		delete[] mOps;
		mOps = 0;
		// Expect a file wih header and at least one reg write
		if (aFile->length() < 34) return FILE_LOAD_FAILED;

		aFile->seek(0);
		if (aFile->read32() != 'PIHC') return FILE_LOAD_FAILED; // CHIP
		if (aFile->read32() != 'ENUT') return FILE_LOAD_FAILED; // TUNE
		int dataofs = aFile->read16();
		int chiptype = aFile->read8();
		// check if this file is for sid, ted, or combination of several
		if (!(chiptype == 0 || chiptype == 4 || chiptype == 5 || chiptype == 6)) return FILE_LOAD_FAILED;
		int flags = aFile->read8();
		int kchunks = aFile->read16();
		int lastchunk = aFile->read16();
		mLength = (kchunks - 1) * 1024 + lastchunk;
		mLooppos = aFile->read16() * 1024 + aFile->read16();
		aFile->read32(); // cpuspeed
		aFile->read32(); // chipspeed
		if ((mFlags & (16 | 32)) ==  0) mModel = SID6581;
		if ((mFlags & (16 | 32)) == 16) mModel = SID8580;
		if ((mFlags & (16 | 32)) == 32) mModel = SID8580DB;
		if ((mFlags & (16 | 32)) == 48) mModel = SID6581R1;
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


	AudioSourceInstance * TedSid::createInstance() 
	{
		return new TedSidInstance(this);
	}

};