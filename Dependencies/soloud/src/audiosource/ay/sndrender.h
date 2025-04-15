/*
   sound resampling core for Unreal Speccy project
   created under public domain license by SMT, jan.2006
*/

#pragma once

#include "sndbuffer.h"

struct SNDOUT;

const unsigned SNDR_DEFAULT_SYSTICK_RATE = 3500000; // ZX-Spectrum Z80 clock
const unsigned SNDR_DEFAULT_SAMPLE_RATE = 44100;

class SNDRENDER
{
public:
        SNDRENDER(SNDBUFFER& sndbuffer);

        void set_timings(unsigned sys_clock_rate, unsigned sample_rate);

        // 'render' is a function that converts array of DAC inputs into PCM-buffer
        void render(SNDOUT *src, unsigned srclen, unsigned sys_ticks);

        // set of functions that fills buffer in emulation progress:

        // when called, we assume that sys_tick reset to 0
        void start_frame();
        // system writes to DAC at given sys_tick
        void update(unsigned sys_tick, unsigned l, unsigned r);
        // emulate non-changed DAC value until end of frame
        void end_frame(unsigned endframe_sys_tick);

protected:

        friend class SNDBUFFER; // access to dstpos in count()
        SNDBUFFER& sndbuffer;

        unsigned mix_l, mix_r; // current L/R volume values
        bufptr_t dstpos; // write position
        bufptr_t dst_start; // write position at start of frame. used to track count of samples in frame

private:
        // local copies of sndbuffer params (for speed)
        SNDSAMPLE* buffer;
        unsigned bufsize;

        unsigned tick, base_tick;
        unsigned s1_l, s1_r;
        unsigned s2_l, s2_r;

        unsigned sys_clock_rate, sample_rate;
        unsigned passed_sys_ticks, passed_snd_ticks;
        unsigned mult_const;

        void flush(unsigned endtick);
};

struct SNDOUT
{
        unsigned timestamp; // in 'system clock' ticks
        SNDSAMPLE newvalue;
};
