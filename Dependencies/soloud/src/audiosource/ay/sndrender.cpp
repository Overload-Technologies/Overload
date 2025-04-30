/*
   sound resampling core for Unreal Speccy project
   created under public domain license by SMT, jan.2006
*/

#include "sndrender.h"

void SNDRENDER::render(SNDOUT *src, unsigned srclen, unsigned sys_ticks)
{
        start_frame();
        for (unsigned index = 0; index < srclen; index++) {
                // if (src[index].timestamp > sys_ticks) continue; // wrong input data leads to crash
                update(src[index].timestamp, src[index].newvalue.ch.left, src[index].newvalue.ch.right);
        }
        end_frame(sys_ticks);
}

const unsigned TICK_FF = 6;            // oversampling ratio: 2^6 = 64
const unsigned TICK_F = (1<<TICK_FF);

const unsigned MULT_C = 12;   // fixed point precision for 'system tick -> sound tick'
// b = 1+ln2(max_sndtick) = 1+ln2((max_sndfq*TICK_F)/min_intfq) = 1+ln2(48000*64/10) ~= 19.2;
// assert(b+MULT_C <= 32)


void SNDRENDER::start_frame()
{
        dst_start = dstpos;
        base_tick = tick;
}

void SNDRENDER::update(unsigned sys_tick, unsigned l, unsigned r)
{
        if (!((l ^ mix_l) | (r ^ mix_r))) return;

        unsigned endtick = (sys_tick * mult_const) >> MULT_C; // = sys_tick * (sample_rate*TICK_F) / sys_clock_rate
        flush(base_tick + endtick);
        mix_l = l, mix_r = r;
}

void SNDRENDER::end_frame(unsigned endframe_sys_tick)
{
        // adjusting 'endframe_sys_tick' with whole history will fix accumulation of rounding errors
        //uint64_t endtick = ((passed_sys_ticks + endframe_sys_tick) * mult_const) >> MULT_C;
        unsigned endtick = ((passed_sys_ticks + endframe_sys_tick) * (long long)sample_rate * TICK_F) / sys_clock_rate;
        flush( (unsigned) (endtick - passed_snd_ticks) );

        // update global system_tick counter
        passed_sys_ticks += endframe_sys_tick;

        // shorten tick to avoid overflow
        unsigned ready_samples = dstpos - dst_start;
        if ((int)ready_samples < 0) ready_samples += bufsize;
        tick -= (ready_samples << TICK_FF);
        passed_snd_ticks += (ready_samples << TICK_FF);
}

void SNDRENDER::set_timings(unsigned aSys_clock_rate, unsigned aSample_rate)
{
        SNDRENDER::sys_clock_rate = aSys_clock_rate;
        SNDRENDER::sample_rate = aSample_rate;

        tick = 0; dstpos = dst_start = 0;
        passed_snd_ticks = passed_sys_ticks = 0;

        mult_const = (unsigned) (((long long)aSample_rate << (MULT_C+TICK_FF)) / aSys_clock_rate);
        // sndbuffer.reset(); // must be done globally - avoid many renders to do many resets
}

static unsigned filter_diff[TICK_F*2];
const double filter_sum_full = 1.0, filter_sum_half = 0.5;
const unsigned filter_sum_full_u = (unsigned)(filter_sum_full * 0x10000),
               filter_sum_half_u = (unsigned)(filter_sum_half * 0x10000);

// main rendering routine
// TICK_F snd_ticks is one output sample. FIR filter used to calculate output samples from snd_ticks
void SNDRENDER::flush(unsigned endtick)
{
        unsigned scale;
        if (!((endtick ^ tick) & ~(TICK_F-1))) {

                scale = filter_diff[(endtick & (TICK_F-1)) + TICK_F] - filter_diff[(tick & (TICK_F-1)) + TICK_F];
                s2_l += mix_l * scale;
                s2_r += mix_r * scale;

                scale = filter_diff[endtick & (TICK_F-1)] - filter_diff[tick & (TICK_F-1)];
                s1_l += mix_l * scale;
                s1_r += mix_r * scale;

                tick = endtick;

        } else {
                scale = filter_sum_full_u - filter_diff[(tick & (TICK_F-1)) + TICK_F];

                unsigned sample_value = ((mix_l*scale + s2_l) >> 16) +
                                          ((mix_r*scale + s2_r) & 0xFFFF0000);

                // don't bother of overflow here. render at low volume and normalize later
                buffer[dstpos].sample += sample_value;
                dstpos = (dstpos+1) & (bufsize-1);

                scale = filter_sum_half_u - filter_diff[tick & (TICK_F-1)];
                s2_l = s1_l + mix_l * scale;
                s2_r = s1_r + mix_r * scale;

                tick = (tick | (TICK_F-1))+1;

                if ((endtick ^ tick) & ~(TICK_F-1)) {
                        // assume filter_coeff is symmetric
                        unsigned val_l = mix_l * filter_sum_half_u;
                        unsigned val_r = mix_r * filter_sum_half_u;
                        do {
                                unsigned sample_value = ((s2_l + val_l) >> 16) +
                                                          ((s2_r + val_r) & 0xFFFF0000); // save s2+val

                                buffer[dstpos].sample += sample_value;
                                dstpos = (dstpos+1) & (bufsize-1);

                                tick += TICK_F;
                                s2_l = val_l, s2_r = val_r; // s2=s1, s1=0;

                        } while ((endtick ^ tick) & ~(TICK_F-1));
                }

                tick = endtick;

                scale = filter_diff[(endtick & (TICK_F-1)) + TICK_F] - filter_sum_half_u;
                s2_l += mix_l * scale;
                s2_r += mix_r * scale;

                scale = filter_diff[endtick & (TICK_F-1)];
                s1_l = mix_l * scale;
                s1_r = mix_r * scale;
        }
}

const double filter_coeff[TICK_F*2] =
{
   // filter designed with Matlab's DSP toolbox
   0.000797243121022152, 0.000815206499600866, 0.000844792477531490, 0.000886460636664257,
   0.000940630171246217, 0.001007677515787512, 0.001087934129054332, 0.001181684445143001,
   0.001289164001921830, 0.001410557756409498, 0.001545998595893740, 0.001695566052785407,
   0.001859285230354019, 0.002037125945605404, 0.002229002094643918, 0.002434771244914945,
   0.002654234457752337, 0.002887136343664226, 0.003133165351783907, 0.003391954293894633,
   0.003663081102412781, 0.003946069820687711, 0.004240391822953223, 0.004545467260249598,
   0.004860666727631453, 0.005185313146989532, 0.005518683858848785, 0.005860012915564928,
   0.006208493567431684, 0.006563280932335042, 0.006923494838753613, 0.007288222831108771,
   0.007656523325719262, 0.008027428904915214, 0.008399949736219575, 0.008773077102914008,
   0.009145787031773989, 0.009517044003286715, 0.009885804729257883, 0.010251021982371376,
   0.010611648461991030, 0.010966640680287394, 0.011314962852635887, 0.011655590776166550,
   0.011987515680350414, 0.012309748033583185, 0.012621321289873522, 0.012921295559959939,
   0.013208761191466523, 0.013482842243062109, 0.013742699838008606, 0.013987535382970279,
   0.014216593638504731, 0.014429165628265581, 0.014624591374614174, 0.014802262449059521,
   0.014961624326719471, 0.015102178534818147, 0.015223484586101132, 0.015325161688957322,
   0.015406890226980602, 0.015468413001680802, 0.015509536233058410, 0.015530130313785910,
   0.015530130313785910, 0.015509536233058410, 0.015468413001680802, 0.015406890226980602,
   0.015325161688957322, 0.015223484586101132, 0.015102178534818147, 0.014961624326719471,
   0.014802262449059521, 0.014624591374614174, 0.014429165628265581, 0.014216593638504731,
   0.013987535382970279, 0.013742699838008606, 0.013482842243062109, 0.013208761191466523,
   0.012921295559959939, 0.012621321289873522, 0.012309748033583185, 0.011987515680350414,
   0.011655590776166550, 0.011314962852635887, 0.010966640680287394, 0.010611648461991030,
   0.010251021982371376, 0.009885804729257883, 0.009517044003286715, 0.009145787031773989,
   0.008773077102914008, 0.008399949736219575, 0.008027428904915214, 0.007656523325719262,
   0.007288222831108771, 0.006923494838753613, 0.006563280932335042, 0.006208493567431684,
   0.005860012915564928, 0.005518683858848785, 0.005185313146989532, 0.004860666727631453,
   0.004545467260249598, 0.004240391822953223, 0.003946069820687711, 0.003663081102412781,
   0.003391954293894633, 0.003133165351783907, 0.002887136343664226, 0.002654234457752337,
   0.002434771244914945, 0.002229002094643918, 0.002037125945605404, 0.001859285230354019,
   0.001695566052785407, 0.001545998595893740, 0.001410557756409498, 0.001289164001921830,
   0.001181684445143001, 0.001087934129054332, 0.001007677515787512, 0.000940630171246217,
   0.000886460636664257, 0.000844792477531490, 0.000815206499600866, 0.000797243121022152
};

void init_diff()
{
        double sum = 0;
        for (int i = 0; i < TICK_F*2; i++) {
                filter_diff[i] = (int)(sum * 0x10000);
                sum += filter_coeff[i];
        }
}

SNDRENDER::SNDRENDER(SNDBUFFER& _sndbuffer)
                : sndbuffer(_sndbuffer), buffer(_sndbuffer.buffer), bufsize(_sndbuffer.size)
{
        tick = 0;
        base_tick = 0;
        s1_l = 0;
        s1_r = 0;
        s2_l = 0;
        s2_r = 0;
        mix_l = 0;
        mix_r = 0;

        sys_clock_rate = 0;
        sample_rate = 0;
        passed_sys_ticks = 0; 
        passed_snd_ticks = 0;
        mult_const = 0;

        set_timings(SNDR_DEFAULT_SYSTICK_RATE, SNDR_DEFAULT_SAMPLE_RATE);

        static char diff_ready = 0;
        if (!diff_ready) {
                init_diff();
                diff_ready = 1;
        }
}
