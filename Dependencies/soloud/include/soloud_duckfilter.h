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

#ifndef SOLOUD_DUCKFILTER_H
#define SOLOUD_DUCKFILTER_H

#include "soloud.h"

namespace SoLoud
{
	class DuckFilter;

	class DuckFilterInstance : public FilterInstance
	{
		handle mListenTo;
		Soloud* mSoloud;
		float mCurrentLevel;
	public:
		virtual void filter(float *aBuffer, unsigned int aSamples, unsigned int aBufferSize, unsigned int aChannels, float aSamplerate, time aTime);
		virtual ~DuckFilterInstance();
		DuckFilterInstance(DuckFilter *aParent);
	};

	class DuckFilter : public Filter
	{
	public:
		enum FILTERATTRIBUTE
		{
			WET = 0,
			ONRAMP,
			OFFRAMP,
			LEVEL
		};
		Soloud* mSoloud;
		float mOnRamp;
		float mOffRamp;
		float mLevel;
		handle mListenTo;
		virtual int getParamCount();
		virtual const char* getParamName(unsigned int aParamIndex);
		virtual unsigned int getParamType(unsigned int aParamIndex);
		virtual float getParamMax(unsigned int aParamIndex);
		virtual float getParamMin(unsigned int aParamIndex);
		virtual FilterInstance *createInstance();
		DuckFilter();
		result setParams(Soloud* aSoloud, handle aListenTo, float aOnRamp = 0.1f, float aOffRamp = 0.5f, float aLevel = 0.1f);
	};
}

#endif