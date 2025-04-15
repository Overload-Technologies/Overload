/*
 * zx7 decompress by Jari Komppa, under public domain / unlicense
 *
 * Heavily based on zx7 decompressor by Einar Saukas. 
 * Einar Saukas requests that you mention the use of dx7, but
 * this is not enforced by any way.
 *
 * Note that compressor has a different license!
 */

#ifndef ZX7DECOMPRESS_H
#define ZX7DECOMPRESS_H

#ifdef __cplusplus
extern "C" {
#endif

struct zx7_io
{
    unsigned char* input_data;
    unsigned char* output_data;
    size_t input_index;
    size_t output_index;
    int bit_mask;
    int bit_value;
};

static int zx7_read_byte(struct zx7_io *io) {
    return io->input_data[io->input_index++];
}

static int zx7_read_bit(struct zx7_io *io) {
    io->bit_mask >>= 1;
    if (io->bit_mask == 0) {
        io->bit_mask = 128;
        io->bit_value = zx7_read_byte(io);
    }
    return io->bit_value & io->bit_mask ? 1 : 0;
}

static int zx7_read_elias_gamma(struct zx7_io *io) {
    int i = 0;
    int value = 0;

    while (!zx7_read_bit(io)) {
        i++;
    }
    if (i > 15) {
        return -1;
    }
    value = 1;
    while (i--) {
        value = value << 1 | zx7_read_bit(io);
    }
    return value;
}

static int zx7_read_offset(struct zx7_io *io) {
    int value = 0;
    int i = 0;

    value = zx7_read_byte(io);
    if (value < 128) {
        return value;
    } else {
        i = zx7_read_bit(io);
        i = i << 1 | zx7_read_bit(io);
        i = i << 1 | zx7_read_bit(io);
        i = i << 1 | zx7_read_bit(io);
        return (value & 127 | i << 7) + 128;
    }
}

static void zx7_write_byte(struct zx7_io *io, int value) {
    io->output_data[io->output_index++] = value;
}

static void zx7_write_bytes(struct zx7_io *io, int offset, int length) {
    int i;
    while (length-- > 0) {
        i = io->output_index - offset;
        zx7_write_byte(io, io->output_data[i]);
    }
}

static int zx7_decompress(unsigned char *input_data, unsigned char *output_data) {
    struct zx7_io io;
    int length;

    io.input_data = input_data;
    io.output_data = output_data;
    io.input_index = 0;
    io.output_index = 0;
    io.bit_mask = 0;
    io.bit_value = 0;

    zx7_write_byte(&io, zx7_read_byte(&io));
    while (1) {
        if (!zx7_read_bit(&io)) {
            zx7_write_byte(&io, zx7_read_byte(&io));
        } else {
            length = zx7_read_elias_gamma(&io) + 1;
            if (length == 0) {
                return io.input_index;
            }
            zx7_write_bytes(&io, zx7_read_offset(&io) + 1, length);
        }
    }
}

#ifdef __cplusplus
}
#endif

#endif // ZX7DECOMPRESS_H