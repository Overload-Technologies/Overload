#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include "chipplayer.h"
#include "soloud_file.h"
#include "soloud_ay.h"

ChipPlayer::ChipPlayer(SoLoud::AyInstance *aInstance)
	: buffer(8192), chip(buffer), chip2(buffer), mInstance(aInstance)
{
	playtick = 0;
	playpos = 0;
	system_clock_rate = 0;
	ticks_per_buffer = 0;

	//set_timings(SNDR_DEFAULT_SYSTICK_RATE * 40 / 17, SNDR_DEFAULT_AY_RATE * 20 / 17, SNDR_DEFAULT_SAMPLE_RATE);
	set_timings(aInstance->mParent->mCpuspeed, aInstance->mParent->mChipspeed, SNDR_DEFAULT_SAMPLE_RATE);
	//set_volumes(SNDCHIP::CHIP_YM, 0x7FFF, SNDR_VOL_YM, SNDR_PAN_ABC);
	if (aInstance->mParent->mYm)
		set_volumes(SNDCHIP::CHIP_YM, 0x7FFF, SNDR_VOL_YM, SNDR_PAN_ABC);
	else
		set_volumes(SNDCHIP::CHIP_AY, 0x7FFF, SNDR_VOL_AY, SNDR_PAN_ABC);
}

ChipPlayer::~ChipPlayer()
{
}

unsigned ChipPlayer::get_time(unsigned tick)
{
	return (unsigned)((tick * 1000) / system_clock_rate);
}




void ChipPlayer::set_timings(unsigned aSystem_clock_rate, unsigned chip_clock_rate, unsigned sample_rate)
{
	ChipPlayer::system_clock_rate = aSystem_clock_rate;
	ticks_per_buffer = (unsigned)((long long)system_clock_rate * (buffer.size - 20) / sample_rate);
	ticks_per_buffer /= 2; // for safety
	chip.set_timings(system_clock_rate, chip_clock_rate, sample_rate);
	chip2.set_timings(system_clock_rate, chip_clock_rate, sample_rate);
	playtick = 0; playpos = 0;
	buffer.reset();
}

const int VOL_DIV = 2; // mixer just adds samples. to avoid overflow, we lower volume then revert back with saturation

void ChipPlayer::set_volumes(SNDCHIP::CHIP_TYPE t, unsigned global_vol, const SNDCHIP_VOLTAB *vt, const SNDCHIP_PANTAB *pt)
{
	chip.set_chip(t);
	chip.set_volumes(global_vol / VOL_DIV, vt, pt);
	chip2.set_chip(t);
	chip2.set_volumes(global_vol / VOL_DIV, vt, pt);
}

inline int _min(int a, int b) { return (a < b) ? a : b; }

unsigned ChipPlayer::play(float *dst, unsigned need_samples)
{
	unsigned stride = need_samples;
	while (need_samples)
	{
		// read from buffer
		buffer.count_start();
		buffer.count(chip);
		buffer.count(chip2);
		unsigned ready = buffer.count_end();
		unsigned from_buffer = _min(ready, need_samples);

		for (unsigned i = 0; i < from_buffer; i++)
		{
			// normalize (volume divided by 4)
			SNDSAMPLE& s = buffer.buffer[(buffer.read_position + i) & (buffer.size - 1)];
			//*dst++ = s;
			int l = VOL_DIV * (int)s.ch.left;
			//if (l > 0x7FFF) l = 0x7FFF;
			//if (l < -0x7FFF) l = -0x7FFF;
			int r = VOL_DIV * (int)s.ch.right;
			//if (r > 0x7FFF) r = 0x7FFF;
			//if (r < -0x7FFF) r = -0x7FFF;
			*dst = l / (float)0x7fff;
			*(dst + stride) = r / (float)0x7fff;
			dst++;
		}
		buffer.samples_read(from_buffer);

		need_samples -= from_buffer;
		if (!need_samples) return stride;

		if (mInstance->mPos >= mInstance->mParent->mLength)
			return stride - need_samples;

		// render to buffer
		chip.start_frame();
		chip2.start_frame();

		unsigned frame_tick = playtick;
		while ((unsigned)(playtick - frame_tick) < ticks_per_buffer)
		{

			unsigned short rd;
			do
			{
				rd = mInstance->mParent->mOps[mInstance->mPos/2];
				mInstance->mPos += 2;
				if (rd & 0x8000)
				{
					playtick += rd ^ 0x8000;
				}
			} while ((rd & 0x8000) && !(mInstance->mPos >= mInstance->mParent->mLength));
			
			if (mInstance->mPos >= mInstance->mParent->mLength)
				break;

			unsigned char reg = rd >> 8;
			unsigned char val = rd & 0xff;

			unsigned ts = (unsigned)(playtick - frame_tick);
			if (reg & 0x10) {
				chip2.select(reg & 0x0F);
				chip2.write(ts, val);
			}
			else {
				chip.select(reg);
				chip.write(ts, val);
			}
		}
		chip.end_frame((unsigned)(playtick - frame_tick));
		chip2.end_frame((unsigned)(playtick - frame_tick));
	}
	return stride;
}
