#pragma once

union SNDSAMPLE
{
        unsigned sample; // left/right channels in low/high WORDs
        struct { unsigned short left, right; } ch; // or left/right separately
};

typedef unsigned bufptr_t;
class SNDRENDER;

class SNDBUFFER
{
public:
        SNDBUFFER(unsigned size);
        ~SNDBUFFER();

        void reset();

        SNDSAMPLE* buffer;
        bufptr_t size;

        void count_start();
        void count(const SNDRENDER& render);
        unsigned count_end();

        unsigned count_single(const SNDRENDER& render);
        void get_buffers(bufptr_t& pos1, unsigned& size1, bufptr_t& pos2, unsigned& size2, unsigned count);

        // can read some samples starting from here, use count_start(), count(), count_end() to detect count of samples ready
        bufptr_t read_position;

        // called when application read `counts` samples from `read_position`
        // SNDBUFFER needs to zero buffer under read samples to continue mixing
        void samples_read(unsigned count);

private:
        unsigned samples_ready;
};




#if 0 // USAGE EXAMPLE

  #include "sndrender/*.h"

  SNDBUFFER buf(4096);
  SNDCHIP ay1(buf), ay2(buf);
  SNDRENDER beeper(buf);

  // global emulation loop
  for (;;) {
     ay1.start_frame();
     ay2.start_frame();
     beeper.start_frame();

     // Z80 emulation before INT
     for (int t = 0; t < 71680; t++) {
        ay1.select(0);
        ay1.write(t, t % 100);
        ay2.select(3);
        ay2.write(t, t % 100);
        beeper.update(t, t % 4000, t % 400);
     }
     ay1.end_frame(t);
     ay2.end_frame(t);
     beeper.end_frame(t);

     buf.count_start();
     buf.count(ay1);
     buf.count(ay2);
     buf.count(beeper);
     unsigned n_samples = buf.count_end();

     bufptr_t pos1, pos2;
     unsigned size1, size2;
     buf.get_buffers(pos1, size1, pos2, size2, n_samples);

     wav_play(buf.buffer+pos1, size1);
     wav_play(buf.buffer+pos2, size2);
     buf.samples_read(n_samples);
  }

#endif

