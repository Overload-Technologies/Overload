/*
   YM-2149F emulator for Unreal Speccy project
   created under public domain license by SMT, jan.2006
*/

#include <string.h>
#include "sndchip.h"

void SNDCHIP::render(AYOUT *src, unsigned srclen, unsigned clk_ticks)
{
        start_frame();
        for (unsigned index = 0; index < srclen; index++) {
                // if (src[index].timestamp > clk_ticks) continue; // wrong input data leads to crash
                select(src[index].reg_num);
                write(src[index].timestamp, src[index].reg_value);
        }
        end_frame(clk_ticks);
}


const unsigned MULT_C_1 = 14; // fixed point precision for 'system tick -> ay tick'
// b = 1+ln2(max_ay_tick/8) = 1+ln2(max_ay_fq/8 / min_intfq) = 1+ln2(10000000/(10*8)) = 17.9
// assert(b+MULT_C_1 <= 32)

void SNDCHIP::start_frame()
{
        r13_reloaded = 0;
        SNDRENDER::start_frame();
}

void SNDCHIP::end_frame(unsigned clk_ticks)
{
        // adjusting 't' with whole history will fix accumulation of rounding errors

        unsigned end_chip_tick = ((passed_clk_ticks + (long long)clk_ticks) * chip_clock_rate) / system_clock_rate;

        flush( (unsigned) (end_chip_tick - passed_chip_ticks) );
        SNDRENDER::end_frame(t);

        passed_clk_ticks += clk_ticks;
        passed_chip_ticks += t; t = 0;
}

void SNDCHIP::flush(unsigned chiptick)
{
   while (t < chiptick) {
      t++;
      if (++ta >= fa) ta = 0, bitA ^= -1;
      if (++tb >= fb) tb = 0, bitB ^= -1;
      if (++tc >= fc) tc = 0, bitC ^= -1;
      if (++tn >= fn)
         tn = 0,
         ns = (ns*2+1) ^ (((ns>>16)^(ns>>13)) & 1),
         bitN = 0 - ((ns >> 16) & 1);
      if (++te >= fe) {
         te = 0, env += denv;
         if (env & ~31) {
            unsigned mask = (1<<r.env);
            if (mask & ((1<<0)|(1<<1)|(1<<2)|(1<<3)|(1<<4)|(1<<5)|(1<<6)|(1<<7)|(1<<9)|(1<<15)))
               env = denv = 0;
            else if (mask & ((1<<8)|(1<<12)))
               env &= 31;
            else if (mask & ((1<<10)|(1<<14)))
               denv = -(int)denv, env = env + denv;
            else env = 31, denv = 0; //11,13
         }
      }

      unsigned en, l, r;

      en = ((ea & env) | va) & ((bitA | bit0) & (bitN | bit3));
      l  = vols[0][en]; r  = vols[1][en];

      en = ((eb & env) | vb) & ((bitB | bit1) & (bitN | bit4));
      l += vols[2][en]; r += vols[3][en];

      en = ((ec & env) | vc) & ((bitC | bit2) & (bitN | bit5));
      l += vols[4][en]; r += vols[5][en];

      if ((l ^ SNDRENDER::mix_l) | (r ^ SNDRENDER::mix_r)) // similar check inside update()
         update(t, l, r);
   }
}

void SNDCHIP::select(unsigned char nreg)
{
        if (chiptype == CHIP_AY) nreg &= 0x0F;
        activereg = nreg;
}

void SNDCHIP::write(unsigned timestamp, unsigned char val)
{
   if (activereg >= 0x10) return;

   if ((1 << activereg) & ((1<<1)|(1<<3)|(1<<5)|(1<<13))) val &= 0x0F;
   if ((1 << activereg) & ((1<<6)|(1<<8)|(1<<9)|(1<<10))) val &= 0x1F;

   if (activereg != 13 && reg[activereg] == val) return;

   reg[activereg] = val;


   if (timestamp) flush((timestamp * mult_const) >> MULT_C_1); // cputick * ( (chip_clock_rate/8) / system_clock_rate );

   switch (activereg) {
      case 0:
      case 1:
         fa = r.fA;
         break;
      case 2:
      case 3:
         fb = r.fB;
         break;
      case 4:
      case 5:
         fc = r.fC;
         break;
      case 6:
         fn = val*2;
         break;
      case 7:
         bit0 = 0 - ((val>>0) & 1);
         bit1 = 0 - ((val>>1) & 1);
         bit2 = 0 - ((val>>2) & 1);
         bit3 = 0 - ((val>>3) & 1);
         bit4 = 0 - ((val>>4) & 1);
         bit5 = 0 - ((val>>5) & 1);
         break;
      case 8:
         ea = (val & 0x10)? -1 : 0;
         va = ((val & 0x0F)*2+1) & ~ea;
         break;
      case 9:
         eb = (val & 0x10)? -1 : 0;
         vb = ((val & 0x0F)*2+1) & ~eb;
         break;
      case 10:
         ec = (val & 0x10)? -1 : 0;
         vc = ((val & 0x0F)*2+1) & ~ec;
         break;
      case 11:
      case 12:
         fe = r.envT;
         break;
      case 13:
         r13_reloaded = 1;
         te = 0;
         if (r.env & 4) env = 0, denv = 1; // attack
         else env = 31, denv = -1; // decay
         break;
   }
}

unsigned char SNDCHIP::read()
{
   if (activereg >= 0x10) return 0xFF;
   return reg[activereg & 0x0F];
}

void SNDCHIP::set_timings(unsigned system_clock_rate, unsigned chip_clock_rate, unsigned sample_rate)
{
        chip_clock_rate /= 8;

        SNDCHIP::system_clock_rate = system_clock_rate;
        SNDCHIP::chip_clock_rate = chip_clock_rate;

		mult_const = (unsigned)(((long long)chip_clock_rate << MULT_C_1) / system_clock_rate);
        SNDRENDER::set_timings(chip_clock_rate, sample_rate);
        passed_chip_ticks = passed_clk_ticks = 0;
        t = 0; ns = 0xFFFF;
        apply_regs();
}

void SNDCHIP::set_volumes(unsigned global_vol, const SNDCHIP_VOLTAB *voltab, const SNDCHIP_PANTAB *stereo)
{
        for (int j = 0; j < 6; j++)
                for (int i = 0; i < 32; i++)
                        vols[j][i] = (unsigned) (((long long)global_vol * voltab->v[i] * stereo->raw[j])/(65535*100*3));
        memcpy(&defvoltab, voltab, sizeof(SNDCHIP_VOLTAB));
        memcpy(&defpantab, stereo, sizeof(SNDCHIP_PANTAB));
}

void SNDCHIP::set_volume(unsigned global_vol)
{
        set_volumes(global_vol, &defvoltab, &defpantab);
}

void SNDCHIP::reset(unsigned timestamp)
{
        for (int i = 0; i < 14; i++) reg[i] = 0;
        apply_regs(timestamp);
}

void SNDCHIP::apply_regs(unsigned timestamp)
{
        for (unsigned char r = 0; r < 16; r++) {
                select(r); unsigned char p = reg[r];
                /* clr cached values */
                write(timestamp, p ^ 1); write(timestamp, p);
        }
}

SNDCHIP::SNDCHIP(SNDBUFFER& buffer)
                : SNDRENDER(buffer)
{
        bitA = bitB = bitC = 0;
        set_timings(SNDR_DEFAULT_SYSTICK_RATE, SNDR_DEFAULT_AY_RATE, SNDR_DEFAULT_SAMPLE_RATE);
        set_chip(CHIP_YM);
        set_volumes(0x7FFF, SNDR_VOL_YM, SNDR_PAN_ABC);
        reset();
}

// corresponds enum CHIP_TYPE
const char * const ay_chips[] = { "AY-3-8910", "YM2149F" };


const SNDCHIP_VOLTAB SNDR_VOL_AY_S =
{ { 0x0000,0x0000,0x0340,0x0340,0x04C0,0x04C0,0x06F2,0x06F2,0x0A44,0x0A44,0x0F13,0x0F13,0x1510,0x1510,0x227E,0x227E,
    0x289F,0x289F,0x414E,0x414E,0x5B21,0x5B21,0x7258,0x7258,0x905E,0x905E,0xB550,0xB550,0xD7A0,0xD7A0,0xFFFF,0xFFFF } };

const SNDCHIP_VOLTAB SNDR_VOL_YM_S =
{ { 0x0000,0x0000,0x00EF,0x01D0,0x0290,0x032A,0x03EE,0x04D2,0x0611,0x0782,0x0912,0x0A36,0x0C31,0x0EB6,0x1130,0x13A0,
    0x1751,0x1BF5,0x20E2,0x2594,0x2CA1,0x357F,0x3E45,0x475E,0x5502,0x6620,0x7730,0x8844,0xA1D2,0xC102,0xE0A2,0xFFFF } };

const SNDCHIP_PANTAB SNDR_PAN_MONO_S =
{ 100,100, 100,100, 100,100 };

const SNDCHIP_PANTAB SNDR_PAN_ABC_S =
{ 100,10,  66,66,   10,100 };

const SNDCHIP_PANTAB SNDR_PAN_ACB_S =
{ 100,10,  10,100,  66,66 };

const SNDCHIP_PANTAB SNDR_PAN_BAC_S =
{ 66,66,   100,10,  10,100 };

const SNDCHIP_PANTAB SNDR_PAN_BCA_S =
{ 10,100,  100,10,  66,66 };

const SNDCHIP_PANTAB SNDR_PAN_CAB_S =
{ 66,66,   10,100,  100,10 };

const SNDCHIP_PANTAB SNDR_PAN_CBA_S =
{ 10,100,  66,66,   100,10 };
