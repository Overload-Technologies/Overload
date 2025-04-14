/*
SoLoud audio engine
Copyright (c) 2013-2020 Jari Komppa

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

#ifndef SOLOUD_EQFILTER_H
#define SOLOUD_EQFILTER_H

#include "soloud.h"
#include "soloud_fftfilter.h"

namespace SoLoud
{
	class EqFilter;

	class EqFilterInstance : public FFTFilterInstance
	{
		enum FILTERATTRIBUTE
		{
			WET = 0,
			BAND1 = 1,
			BAND2 = 2,
			BAND3 = 3,
			BAND4 = 4,
			BAND5 = 5,
			BAND6 = 6,
			BAND7 = 7,
			BAND8 = 8
		};
		EqFilter *mParent;
	public:
		virtual void fftFilterChannel(float *aFFTBuffer, unsigned int aSamples, float aSamplerate, time aTime, unsigned int aChannel, unsigned int aChannels);
		EqFilterInstance(EqFilter *aParent);
	};

	class EqFilter : public FFTFilter
	{
	public:
		enum FILTERATTRIBUTE
		{
			WET = 0,
			BAND1 = 1,
			BAND2 = 2,
			BAND3 = 3,
			BAND4 = 4,
			BAND5 = 5,
			BAND6 = 6,
			BAND7 = 7,
			BAND8 = 8
		};
		virtual int getParamCount();
		virtual const char* getParamName(unsigned int aParamIndex);
		virtual unsigned int getParamType(unsigned int aParamIndex);
		virtual float getParamMax(unsigned int aParamIndex);
		virtual float getParamMin(unsigned int aParamIndex);
		float mVolume[8];
		result setParam(unsigned int aBand, float aVolume);
		virtual FilterInstance *createInstance();
		EqFilter();
	};
}

#endif