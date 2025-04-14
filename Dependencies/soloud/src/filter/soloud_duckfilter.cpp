/*
SoLoud audio engine
Copyright (c) 2013-2021 Jari Komppa

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

#include "soloud.h"
#include "soloud_duckfilter.h"

namespace SoLoud
{
	DuckFilterInstance::DuckFilterInstance(DuckFilter *aParent)
	{
		initParams(4);
		mParam[DuckFilter::ONRAMP] = aParent->mOnRamp;
		mParam[DuckFilter::OFFRAMP] = aParent->mOffRamp;
		mParam[DuckFilter::LEVEL] = aParent->mLevel;
		mListenTo = aParent->mListenTo;
		mSoloud = aParent->mSoloud;
		mCurrentLevel = 1;
	}

	void DuckFilterInstance::filter(float *aBuffer, unsigned int aSamples, unsigned int aBufferSize, unsigned int aChannels, float aSamplerate, double aTime)
	{
		updateParams(aTime);
		float onramp_step = 1;
		if (mParam[DuckFilter::ONRAMP] > 0.01)
			onramp_step = (1.0f - mParam[DuckFilter::LEVEL]) / (mParam[DuckFilter::ONRAMP] * aSamplerate);
		float offramp_step = 1;
		if (mParam[DuckFilter::OFFRAMP] > 0.01)
			offramp_step = (1.0f - mParam[DuckFilter::LEVEL]) / (mParam[DuckFilter::OFFRAMP] * aSamplerate);

		int soundOn = 0;
		if (mSoloud)
		{
			int voiceno = mSoloud->getVoiceFromHandle_internal(mListenTo);
			if (voiceno != -1)
			{
				BusInstance* bi = (BusInstance*)mSoloud->mVoice[voiceno];
				float v = 0;
				for (unsigned int i = 0; i < bi->mChannels; i++)
					v += bi->mVisualizationChannelVolume[i];
				if (v > 0.01f)
					soundOn = 1;
			}
		}
		float level = mCurrentLevel;
		for (unsigned int j = 0; j < aChannels; j++)
		{
			level = mCurrentLevel;
			int bchofs = j * aBufferSize;
			for (unsigned int i = 0; i < aSamples; i++)
			{
				if (soundOn && level > mParam[DuckFilter::LEVEL])
					level -= onramp_step;
				if (!soundOn && level < 1)
					level += offramp_step;
				if (level < mParam[DuckFilter::LEVEL]) level = mParam[DuckFilter::LEVEL];
				if (level > 1) level = 1;
				aBuffer[i + bchofs] += (-aBuffer[i + bchofs] + aBuffer[i + bchofs] * level) * mParam[DuckFilter::WET];
			}
		}
		mCurrentLevel = level;
	}

	DuckFilterInstance::~DuckFilterInstance()
	{
	}

	DuckFilter::DuckFilter()
	{
		mSoloud = 0;
		mOnRamp = 0.1f;
		mOffRamp = 0.5f;
		mLevel = 0.5f;
	}

	result DuckFilter::setParams(Soloud* aSoloud, handle aListenTo, float aOnRamp, float aOffRamp, float aLevel)
	{
		if (aOnRamp < 0.0f || aOffRamp < 0.0f || aLevel < 0.0f || aSoloud == 0 || !aSoloud->isValidVoiceHandle(aListenTo))
			return INVALID_PARAMETER;
		
		mListenTo = aListenTo;
		mOnRamp = aOnRamp;
		mOffRamp = aOffRamp;
		mLevel = aLevel;
		mSoloud = aSoloud;
		
		return 0;
	}

	int DuckFilter::getParamCount()
	{
		return 4;
	}

	const char* DuckFilter::getParamName(unsigned int aParamIndex)
	{
		if (aParamIndex > 3)
			return 0;
		const char *names[4] = {
			"Wet",
			"OnRamp",
			"OffRamp",
			"Level"
		};
		return names[aParamIndex];
	}

	unsigned int DuckFilter::getParamType(unsigned int aParamIndex)
	{
		return FLOAT_PARAM;
	}

	float DuckFilter::getParamMax(unsigned int aParamIndex)
	{
		return 1;
	}

	float DuckFilter::getParamMin(unsigned int aParamIndex)
	{
		return 0;
	}

	FilterInstance *DuckFilter::createInstance()
	{
		return new DuckFilterInstance(this);
	}
}
