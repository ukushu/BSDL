/*
  Simple DirectMedia Layer
  Copyright (C) 1997-2026 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
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
  3. This notice may not be removed or altered from any source distribution.
*/
#include "SDL_internal.h"

// Microsoft WAVE file loading routines

#include "SDL_wave.h"
#include "SDL_sysaudio.h"

#ifdef SDL_WAVE_DEBUG_LOG_FORMAT
static void WaveDebugLogFormat(WaveFile *file)
{
    WaveFormat *format = &file->format;
    const char *fmtstr = "WAVE file: %s, %u Hz, %s, %u bits, %u %s/s";
    const char *waveformat, *wavechannel, *wavebpsunit = "B";
    Uint32 wavebps = format->byterate;
    char channelstr[64];

    SDL_zeroa(channelstr);

    switch (format->encoding) {
    case PCM_CODE:
        waveformat = "PCM";
        break;
    case IEEE_FLOAT_CODE:
        waveformat = "IEEE Float";
        break;
    case ALAW_CODE:
        waveformat = "A-law";
        break;
    case MULAW_CODE:
        waveformat = "\xc2\xb5-law";
        break;
    case MS_ADPCM_CODE:
        waveformat = "MS ADPCM";
        break;
    case IMA_ADPCM_CODE:
        waveformat = "IMA ADPCM";
        break;
    default:
        waveformat = "Unknown";
        break;
    }

#define SDL_WAVE_DEBUG_CHANNELCFG(STR, CODE) \
    case CODE:                               \
        wavechannel = STR;                   \
        break;
#define SDL_WAVE_DEBUG_CHANNELSTR(STR, CODE)                                        \
    if (format->channelmask & CODE) {                                               \
        SDL_strlcat(channelstr, channelstr[0] ? "-" STR : STR, sizeof(channelstr)); \
    }

    if (format->formattag == EXTENSIBLE_CODE && format->channelmask > 0) {
        switch (format->channelmask) {
            SDL_WAVE_DEBUG_CHANNELCFG("1.0 Mono", 0x4)
            SDL_WAVE_DEBUG_CHANNELCFG("1.1 Mono", 0xc)
            SDL_WAVE_DEBUG_CHANNELCFG("2.0 Stereo", 0x3)
            SDL_WAVE_DEBUG_CHANNELCFG("2.1 Stereo", 0xb)
            SDL_WAVE_DEBUG_CHANNELCFG("3.0 Stereo", 0x7)
            SDL_WAVE_DEBUG_CHANNELCFG("3.1 Stereo", 0xf)
            SDL_WAVE_DEBUG_CHANNELCFG("3.0 Surround", 0x103)
            SDL_WAVE_DEBUG_CHANNELCFG("3.1 Surround", 0x10b)
            SDL_WAVE_DEBUG_CHANNELCFG("4.0 Quad", 0x33)
            SDL_WAVE_DEBUG_CHANNELCFG("4.1 Quad", 0x3b)
            SDL_WAVE_DEBUG_CHANNELCFG("4.0 Surround", 0x107)
            SDL_WAVE_DEBUG_CHANNELCFG("4.1 Surround", 0x10f)
            SDL_WAVE_DEBUG_CHANNELCFG("5.0", 0x37)
            SDL_WAVE_DEBUG_CHANNELCFG("5.1", 0x3f)
            SDL_WAVE_DEBUG_CHANNELCFG("5.0 Side", 0x607)
            SDL_WAVE_DEBUG_CHANNELCFG("5.1 Side", 0x60f)
            SDL_WAVE_DEBUG_CHANNELCFG("6.0", 0x137)
            SDL_WAVE_DEBUG_CHANNELCFG("6.1", 0x13f)
            SDL_WAVE_DEBUG_CHANNELCFG("6.0 Side", 0x707)
            SDL_WAVE_DEBUG_CHANNELCFG("6.1 Side", 0x70f)
            SDL_WAVE_DEBUG_CHANNELCFG("7.0", 0xf7)
            SDL_WAVE_DEBUG_CHANNELCFG("7.1", 0xff)
            SDL_WAVE_DEBUG_CHANNELCFG("7.0 Side", 0x6c7)
            SDL_WAVE_DEBUG_CHANNELCFG("7.1 Side", 0x6cf)
            SDL_WAVE_DEBUG_CHANNELCFG("7.0 Surround", 0x637)
            SDL_WAVE_DEBUG_CHANNELCFG("7.1 Surround", 0x63f)
            SDL_WAVE_DEBUG_CHANNELCFG("9.0 Surround", 0x5637)
            SDL_WAVE_DEBUG_CHANNELCFG("9.1 Surround", 0x563f)
            SDL_WAVE_DEBUG_CHANNELCFG("11.0 Surround", 0x56f7)
            SDL_WAVE_DEBUG_CHANNELCFG("11.1 Surround", 0x56ff)
        default:
            SDL_WAVE_DEBUG_CHANNELSTR("FL", 0x1)
            SDL_WAVE_DEBUG_CHANNELSTR("FR", 0x2)
            SDL_WAVE_DEBUG_CHANNELSTR("FC", 0x4)
            SDL_WAVE_DEBUG_CHANNELSTR("LF", 0x8)
            SDL_WAVE_DEBUG_CHANNELSTR("BL", 0x10)
            SDL_WAVE_DEBUG_CHANNELSTR("BR", 0x20)
            SDL_WAVE_DEBUG_CHANNELSTR("FLC", 0x40)
            SDL_WAVE_DEBUG_CHANNELSTR("FRC", 0x80)
            SDL_WAVE_DEBUG_CHANNELSTR("BC", 0x100)
            SDL_WAVE_DEBUG_CHANNELSTR("SL", 0x200)
            SDL_WAVE_DEBUG_CHANNELSTR("SR", 0x400)
            SDL_WAVE_DEBUG_CHANNELSTR("TC", 0x800)
            SDL_WAVE_DEBUG_CHANNELSTR("TFL", 0x1000)
            SDL_WAVE_DEBUG_CHANNELSTR("TFC", 0x2000)
            SDL_WAVE_DEBUG_CHANNELSTR("TFR", 0x4000)
            SDL_WAVE_DEBUG_CHANNELSTR("TBL", 0x8000)
            SDL_WAVE_DEBUG_CHANNELSTR("TBC", 0x10000)
            SDL_WAVE_DEBUG_CHANNELSTR("TBR", 0x20000)
            break;
        }
    } else {
        switch (format->channels) {
        default:
            if (SDL_snprintf(channelstr, sizeof(channelstr), "%u channels", format->channels) >= 0) {
                wavechannel = channelstr;
                break;
            }
        case 0:
            wavechannel = "Unknown";
            break;
        case 1:
            wavechannel = "Mono";
            break;
        case 2:
            wavechannel = "Setero";
            break;
        }
    }

#undef SDL_WAVE_DEBUG_CHANNELCFG
#undef SDL_WAVE_DEBUG_CHANNELSTR

    if (wavebps >= 1024) {
        wavebpsunit = "KiB";
        wavebps = wavebps / 1024 + (wavebps & 0x3ff ? 1 : 0);
    }

    SDL_LogDebug(SDL_LOG_CATEGORY_AUDIO, fmtstr, waveformat, format->frequency, wavechannel, format->bitspersample, wavebps, wavebpsunit);
}
#endif

#ifdef SDL_WAVE_DEBUG_DUMP_FORMAT
static void WaveDebugDumpFormat(WaveFile *file, Uint32 rifflen, Uint32 fmtlen, Uint32 datalen)
{
    WaveFormat *format = &file->format;
    const char *fmtstr1 = "WAVE chunk dump:\n"
                          "-------------------------------------------\n"
                          "RIFF                            %11u\n"
                          "-------------------------------------------\n"
                          "    fmt                         %11u\n"
                          "        wFormatTag                   0x%04x\n"
                          "        nChannels               %11u\n"
                          "        nSamplesPerSec          %11u\n"
                          "        nAvgBytesPerSec         %11u\n"
                          "        nBlockAlign             %11u\n";
    const char *fmtstr2 = "        wBitsPerSample          %11u\n";
    const char *fmtstr3 = "        cbSize                  %11u\n";
    const char *fmtstr4a = "        wValidBitsPerSample     %11u\n";
    const char *fmtstr4b = "        wSamplesPerBlock        %11u\n";
    const char *fmtstr5 = "        dwChannelMask            0x%08x\n"
                          "        SubFormat\n"
                          "        %08x-%04x-%04x-%02x%02x%02x%02x%02x%02x%02x%02x\n";
    const char *fmtstr6 = "-------------------------------------------\n"
                          " fact\n"
                          "  dwSampleLength                %11u\n";
    const char *fmtstr7 = "-------------------------------------------\n"
                          " data                           %11u\n"
                          "-------------------------------------------\n";
    char *dumpstr;
    size_t dumppos = 0;
    const size_t bufsize = 1024;
    int res;

    dumpstr = SDL_malloc(bufsize);
    if (!dumpstr) {
        return;
    }
    dumpstr[0] = 0;

    res = SDL_snprintf(dumpstr, bufsize, fmtstr1, rifflen, fmtlen, format->formattag, format->channels, format->frequency, format->byterate, format->blockalign);
    dumppos += res > 0 ? res : 0;
    if (fmtlen >= 16) {
        res = SDL_snprintf(dumpstr + dumppos, bufsize - dumppos, fmtstr2, format->bitspersample);
        dumppos += res > 0 ? res : 0;
    }
    if (fmtlen >= 18) {
        res = SDL_snprintf(dumpstr + dumppos, bufsize - dumppos, fmtstr3, format->extsize);
        dumppos += res > 0 ? res : 0;
    }
    if (format->formattag == EXTENSIBLE_CODE && fmtlen >= 40 && format->extsize >= 22) {
        const Uint8 *g = format->subformat;
        const Uint32 g1 = g[0] | ((Uint32)g[1] << 8) | ((Uint32)g[2] << 16) | ((Uint32)g[3] << 24);
        const Uint32 g2 = g[4] | ((Uint32)g[5] << 8);
        const Uint32 g3 = g[6] | ((Uint32)g[7] << 8);

        switch (format->encoding) {
        default:
            res = SDL_snprintf(dumpstr + dumppos, bufsize - dumppos, fmtstr4a, format->validsamplebits);
            dumppos += res > 0 ? res : 0;
            break;
        case MS_ADPCM_CODE:
        case IMA_ADPCM_CODE:
            res = SDL_snprintf(dumpstr + dumppos, bufsize - dumppos, fmtstr4b, format->samplesperblock);
            dumppos += res > 0 ? res : 0;
            break;
        }
        res = SDL_snprintf(dumpstr + dumppos, bufsize - dumppos, fmtstr5, format->channelmask, g1, g2, g3, g[8], g[9], g[10], g[11], g[12], g[13], g[14], g[15]);
        dumppos += res > 0 ? res : 0;
    } else {
        switch (format->encoding) {
        case MS_ADPCM_CODE:
        case IMA_ADPCM_CODE:
            if (fmtlen >= 20 && format->extsize >= 2) {
                res = SDL_snprintf(dumpstr + dumppos, bufsize - dumppos, fmtstr4b, format->samplesperblock);
                dumppos += res > 0 ? res : 0;
            }
            break;
        }
    }
    if (file->fact.status >= 1) {
        res = SDL_snprintf(dumpstr + dumppos, bufsize - dumppos, fmtstr6, file->fact.samplelength);
        dumppos += res > 0 ? res : 0;
    }
    res = SDL_snprintf(dumpstr + dumppos, bufsize - dumppos, fmtstr7, datalen);
    dumppos += res > 0 ? res : 0;

    SDL_LogDebug(SDL_LOG_CATEGORY_AUDIO, "%s", dumpstr);

    SDL_free(dumpstr);
}
#endif

static WaveRiffSizeHint WaveGetRiffSizeHint(void)
{
    const char *hint = SDL_GetHint(SDL_HINT_WAVE_RIFF_CHUNK_SIZE);

    if (hint) {
        if (SDL_strcmp(hint, "force") == 0) {
            return RiffSizeForce;
        } else if (SDL_strcmp(hint, "ignore") == 0) {
            return RiffSizeIgnore;
        } else if (SDL_strcmp(hint, "ignorezero") == 0) {
            return RiffSizeIgnoreZero;
        } else if (SDL_strcmp(hint, "maximum") == 0) {
            return RiffSizeMaximum;
        }
    }

    return RiffSizeNoHint;
}

static WaveTruncationHint WaveGetTruncationHint(void)
{
    const char *hint = SDL_GetHint(SDL_HINT_WAVE_TRUNCATION);

    if (hint) {
        if (SDL_strcmp(hint, "verystrict") == 0) {
            return TruncVeryStrict;
        } else if (SDL_strcmp(hint, "strict") == 0) {
            return TruncStrict;
        } else if (SDL_strcmp(hint, "dropframe") == 0) {
            return TruncDropFrame;
        } else if (SDL_strcmp(hint, "dropblock") == 0) {
            return TruncDropBlock;
        }
    }

    return TruncNoHint;
}

static WaveFactChunkHint WaveGetFactChunkHint(void)
{
    const char *hint = SDL_GetHint(SDL_HINT_WAVE_FACT_CHUNK);

    if (hint) {
        if (SDL_strcmp(hint, "truncate") == 0) {
            return FactTruncate;
        } else if (SDL_strcmp(hint, "strict") == 0) {
            return FactStrict;
        } else if (SDL_strcmp(hint, "ignorezero") == 0) {
            return FactIgnoreZero;
        } else if (SDL_strcmp(hint, "ignore") == 0) {
            return FactIgnore;
        }
    }

    return FactNoHint;
}

static void WaveFreeChunkData(WaveChunk *chunk)
{
    if (chunk->data) {
        SDL_free(chunk->data);
        chunk->data = NULL;
    }
    chunk->size = 0;
}

static int WaveNextChunk(SDL_IOStream *src, WaveChunk *chunk)
{
    Uint32 chunkheader[2];
    Sint64 nextposition = chunk->position + chunk->length;

    // Data is no longer valid after this function returns.
    WaveFreeChunkData(chunk);

    // Error on overflows.
    if (SDL_MAX_SINT64 - chunk->length < chunk->position || SDL_MAX_SINT64 - 8 < nextposition) {
        return -1;
    }

    // RIFF chunks have a 2-byte alignment. Skip padding byte.
    if (chunk->length & 1) {
        nextposition++;
    }

    if (SDL_SeekIO(src, nextposition, SDL_IO_SEEK_SET) != nextposition) {
        // Not sure how we ended up here. Just abort.
        return -2;
    } else if (SDL_ReadIO(src, chunkheader, sizeof(Uint32) * 2) != (sizeof(Uint32) * 2)) {
        return -1;
    }

    chunk->fourcc = SDL_Swap32LE(chunkheader[0]);
    chunk->length = SDL_Swap32LE(chunkheader[1]);
    chunk->position = nextposition + 8;

    return 0;
}

static int WaveReadPartialChunkData(SDL_IOStream *src, WaveChunk *chunk, size_t length)
{
    WaveFreeChunkData(chunk);

    if (length > chunk->length) {
        length = chunk->length;
    }

    if (length > 0) {
        chunk->data = (Uint8 *)SDL_malloc(length);
        if (!chunk->data) {
            return -1;
        }

        if (SDL_SeekIO(src, chunk->position, SDL_IO_SEEK_SET) != chunk->position) {
            // Not sure how we ended up here. Just abort.
            return -2;
        }

        chunk->size = SDL_ReadIO(src, chunk->data, length);
        if (chunk->size != length) {
            // Expected to be handled by the caller.
        }
    }

    return 0;
}

static int WaveReadChunkData(SDL_IOStream *src, WaveChunk *chunk)
{
    return WaveReadPartialChunkData(src, chunk, chunk->length);
}

typedef struct WaveExtensibleGUID
{
    Uint16 encoding;
    Uint8 guid[16];
} WaveExtensibleGUID;

// Some of the GUIDs that are used by WAVEFORMATEXTENSIBLE.
#define WAVE_FORMATTAG_GUID(tag)                                                     \
    {                                                                                \
        (tag) & 0xff, (tag) >> 8, 0, 0, 0, 0, 16, 0, 128, 0, 0, 170, 0, 56, 155, 113 \
    }
static WaveExtensibleGUID extensible_guids[] = {
    { PCM_CODE, WAVE_FORMATTAG_GUID(PCM_CODE) },
    { MS_ADPCM_CODE, WAVE_FORMATTAG_GUID(MS_ADPCM_CODE) },
    { IEEE_FLOAT_CODE, WAVE_FORMATTAG_GUID(IEEE_FLOAT_CODE) },
    { ALAW_CODE, WAVE_FORMATTAG_GUID(ALAW_CODE) },
    { MULAW_CODE, WAVE_FORMATTAG_GUID(MULAW_CODE) },
    { IMA_ADPCM_CODE, WAVE_FORMATTAG_GUID(IMA_ADPCM_CODE) }
};

static Uint16 WaveGetFormatGUIDEncoding(WaveFormat *format)
{
    size_t i;
    for (i = 0; i < SDL_arraysize(extensible_guids); i++) {
        if (SDL_memcmp(format->subformat, extensible_guids[i].guid, 16) == 0) {
            return extensible_guids[i].encoding;
        }
    }
    return UNKNOWN_CODE;
}

static bool WaveReadFormat(WaveFile *file)
{
    WaveChunk *chunk = &file->chunk;
    WaveFormat *format = &file->format;
    SDL_IOStream *fmtsrc;
    size_t fmtlen = chunk->size;

    if (fmtlen > SDL_MAX_SINT32) {
        // Limit given by SDL_IOFromConstMem.
        return SDL_SetError("Data of WAVE fmt chunk too big");
    }
    fmtsrc = SDL_IOFromConstMem(chunk->data, (int)chunk->size);
    if (!fmtsrc) {
        return false;
    }

    if (!SDL_ReadU16LE(fmtsrc, &format->formattag) ||
        !SDL_ReadU16LE(fmtsrc, &format->channels) ||
        !SDL_ReadU32LE(fmtsrc, &format->frequency) ||
        !SDL_ReadU32LE(fmtsrc, &format->byterate) ||
        !SDL_ReadU16LE(fmtsrc, &format->blockalign)) {
        return false;
    }
    format->encoding = format->formattag;

    // This is PCM specific in the first version of the specification.
    if (fmtlen >= 16) {
        if (!SDL_ReadU16LE(fmtsrc, &format->bitspersample)) {
            return false;
        }
    } else if (format->encoding == PCM_CODE) {
        SDL_CloseIO(fmtsrc);
        return SDL_SetError("Missing wBitsPerSample field in WAVE fmt chunk");
    }

    // The earlier versions also don't have this field.
    if (fmtlen >= 18) {
        if (!SDL_ReadU16LE(fmtsrc, &format->extsize)) {
            return false;
        }
    }

    if (format->formattag == EXTENSIBLE_CODE) {
        /* note that this ignores channel masks, smaller valid bit counts
         * inside a larger container, and most subtypes. This is just enough
         * to get things that didn't really _need_ WAVE_FORMAT_EXTENSIBLE
         * to be useful working when they use this format flag.
         */

        // Extensible header must be at least 22 bytes.
        if (fmtlen < 40 || format->extsize < 22) {
            SDL_CloseIO(fmtsrc);
            return SDL_SetError("Extensible WAVE header too small");
        }

        if (!SDL_ReadU16LE(fmtsrc, &format->validsamplebits) ||
            !SDL_ReadU32LE(fmtsrc, &format->channelmask) ||
            SDL_ReadIO(fmtsrc, format->subformat, 16) != 16) {
        }
        format->samplesperblock = format->validsamplebits;
        format->encoding = WaveGetFormatGUIDEncoding(format);
    }

    SDL_CloseIO(fmtsrc);

    return true;
}

static bool WaveReadRIFFHeader(SDL_IOStream *src, WaveFile *file, WaveChunk *RIFFchunk, Sint64 *RIFFend, bool *RIFFlengthknown)
{
    Sint64 RIFFstart;

    RIFFstart = SDL_TellIO(src);
    if (RIFFstart < 0) {
        return SDL_SetError("Could not seek in file");
    }

    RIFFchunk->position = RIFFstart;
    if (WaveNextChunk(src, RIFFchunk) < 0) {
        return SDL_SetError("Could not read RIFF header");
    }

    // Check main WAVE file identifiers.
    if (RIFFchunk->fourcc == RIFF) {
        Uint32 formtype;
        // Read the form type. "WAVE" expected.
        if (!SDL_ReadU32LE(src, &formtype)) {
            return SDL_SetError("Could not read RIFF form type");
        } else if (formtype != WAVE) {
            return SDL_SetError("RIFF form type is not WAVE (not a Waveform file)");
        }
    } else if (RIFFchunk->fourcc == WAVE) {
        // RIFF chunk missing or skipped. Length unknown.
        RIFFchunk->position = 0;
        RIFFchunk->length = 0;
    } else {
        return SDL_SetError("Could not find RIFF or WAVE identifiers (not a Waveform file)");
    }

    // The 4-byte form type is immediately followed by the first chunk.
    file->chunk.position = RIFFchunk->position + 4;

    /* Use the RIFF chunk size to limit the search for the chunks. This is not
     * always reliable and the hint can be used to tune the behavior. By
     * default, it will never search past 4 GiB.
     */
    switch (file->riffhint) {
    case RiffSizeIgnore:
        *RIFFend = RIFFchunk->position + SDL_MAX_UINT32;
        break;
    default:
    case RiffSizeIgnoreZero:
        if (RIFFchunk->length == 0) {
            *RIFFend = RIFFchunk->position + SDL_MAX_UINT32;
            break;
        }
        SDL_FALLTHROUGH;
    case RiffSizeForce:
        *RIFFend = RIFFchunk->position + RIFFchunk->length;
        *RIFFlengthknown = true;
        break;
    case RiffSizeMaximum:
        *RIFFend = SDL_MAX_SINT64;
        break;
    }

    return true;
}

static bool WaveRememberChunk(SDL_IOStream *src, WaveFile *file, WaveChunk *fmtchunk, WaveChunk *datachunk, Sint64 flen)
{
    WaveChunk *chunk = &file->chunk;

    if (chunk->fourcc == FMT) {
        if (fmtchunk->fourcc == FMT) {
            // Multiple fmt chunks. Ignore or error?
        } else {
            // The fmt chunk must occur before the data chunk.
            if (datachunk->fourcc == DATA) {
                return SDL_SetError("fmt chunk after data chunk in WAVE file");
            }
            *fmtchunk = *chunk;
        }
    } else if (chunk->fourcc == DATA) {
        /* If the data chunk is bigger than the file, it might be corrupt
           or the file is truncated. Try to recover by clamping the file
           size. This also means a malicious file can't allocate 4 gigabytes
           for the chunks without actually supplying a 4 gigabyte file. */
        if ((flen > 0) && ((chunk->position + chunk->length) > flen)) {
            chunk->length = (Uint32) (flen - chunk->position);
        }

        /* Only use the first data chunk. Handling the wavl list madness
         * may require a different approach.
         */
        if (datachunk->fourcc != DATA) {
            *datachunk = *chunk;
        }
    } else if (chunk->fourcc == FACT) {
        /* The fact chunk data must be at least 4 bytes for the
         * dwSampleLength field. Ignore all fact chunks after the first one.
         */
        if (file->fact.status == 0) {
            if (chunk->length < 4) {
                file->fact.status = -1;
            } else {
                // Let's use src directly, it's just too convenient.
                Sint64 position = SDL_SeekIO(src, chunk->position, SDL_IO_SEEK_SET);
                if (position == chunk->position && SDL_ReadU32LE(src, &file->fact.samplelength)) {
                    file->fact.status = 1;
                } else {
                    file->fact.status = -1;
                }
            }
        }
    }
    return true;
}

static bool WaveLoad(SDL_IOStream *src, WaveFile *file, SDL_AudioSpec *spec, Uint8 **audio_buf, Uint32 *audio_len)
{
    int result;
    Uint32 chunkcount = 0;
    Uint32 chunkcountlimit = 10000;
    const Sint64 flen = SDL_GetIOSize(src);   // this might be -1 if the IOStream can't determine the total size.
    const char *hint;
    Sint64 RIFFend = 0, lastchunkpos;
    bool RIFFlengthknown = false;
    WaveChunk *chunk = &file->chunk;
    WaveChunk RIFFchunk;
    WaveChunk fmtchunk;
    WaveChunk datachunk;

    SDL_zero(RIFFchunk);
    SDL_zero(fmtchunk);
    SDL_zero(datachunk);

    hint = SDL_GetHint(SDL_HINT_WAVE_CHUNK_LIMIT);
    if (hint) {
        unsigned int count;
        if (SDL_sscanf(hint, "%u", &count) == 1) {
            chunkcountlimit = count <= SDL_MAX_UINT32 ? count : SDL_MAX_UINT32;
        }
    }

    if (!WaveReadRIFFHeader(src, file, &RIFFchunk, &RIFFend, &RIFFlengthknown)) {
        return false;
    }

    /* Step through all chunks and save information on the fmt, data, and fact
     * chunks. Ignore the chunks we don't know as per specification. This
     * currently also ignores cue, list, and inst chunks.
     */
    while ((Uint64)RIFFend > (Uint64)chunk->position + chunk->length + (chunk->length & 1)) {
        // Abort after too many chunks or else corrupt files may waste time.
        if (chunkcount++ >= chunkcountlimit) {
            return SDL_SetError("Chunk count in WAVE file exceeds limit of %" SDL_PRIu32, chunkcountlimit);
        }

        result = WaveNextChunk(src, chunk);
        if (result < 0) {
            // Unexpected EOF. Corrupt file or I/O issues.
            if (file->trunchint == TruncVeryStrict) {
                return SDL_SetError("Unexpected end of WAVE file");
            }
            // Let the checks after this loop sort this issue out.
            break;
        } else if (result == -2) {
            return SDL_SetError("Could not seek to WAVE chunk header");
        }

        if (!WaveRememberChunk(src, file, &fmtchunk, &datachunk, flen)) {
            return false;
        }

        /* Go through all chunks in verystrict mode or stop the search early if
         * all required chunks were found.
         */
        if (file->trunchint == TruncVeryStrict) {
            if ((Uint64)RIFFend < (Uint64)chunk->position + chunk->length) {
                return SDL_SetError("RIFF size truncates chunk");
            }
        } else if (fmtchunk.fourcc == FMT && datachunk.fourcc == DATA) {
            if (file->fact.status == 1 || file->facthint == FactIgnore || file->facthint == FactNoHint) {
                break;
            }
        }
    }

    /* Save the position after the last chunk. This position will be used if the
     * RIFF length is unknown.
     */
    lastchunkpos = chunk->position + chunk->length;

    // The fmt chunk is mandatory.
    if (fmtchunk.fourcc != FMT) {
        return SDL_SetError("Missing fmt chunk in WAVE file");
    }
    // A data chunk must be present.
    if (datachunk.fourcc != DATA) {
        return SDL_SetError("Missing data chunk in WAVE file");
    }
    // Check if the last chunk has all of its data in verystrict mode.
    if (file->trunchint == TruncVeryStrict) {
        // data chunk is handled later.
        if (chunk->fourcc != DATA && chunk->length > 0) {
            Uint8 tmp;
            Uint64 position = (Uint64)chunk->position + chunk->length - 1;
            if (position > SDL_MAX_SINT64 || SDL_SeekIO(src, (Sint64)position, SDL_IO_SEEK_SET) != (Sint64)position) {
                return SDL_SetError("Could not seek to WAVE chunk data");
            } else if (!SDL_ReadU8(src, &tmp)) {
                return SDL_SetError("RIFF size truncates chunk");
            }
        }
    }

    // Process fmt chunk.
    *chunk = fmtchunk;

    /* No need to read more than 1046 bytes of the fmt chunk data with the
     * formats that are currently supported. (1046 because of MS ADPCM coefficients)
     */
    if (WaveReadPartialChunkData(src, chunk, 1046) < 0) {
        return SDL_SetError("Could not read data of WAVE fmt chunk");
    }

    /* The fmt chunk data must be at least 14 bytes to include all common fields.
     * It usually is 16 and larger depending on the header and encoding.
     */
    if (chunk->length < 14) {
        return SDL_SetError("Invalid WAVE fmt chunk length (too small)");
    } else if (chunk->size < 14) {
        return SDL_SetError("Could not read data of WAVE fmt chunk");
    } else if (!WaveReadFormat(file)) {
        return false;
    } else if (!SDL_WaveCheckFormat(file, (size_t)datachunk.length)) {
        return false;
    }

#ifdef SDL_WAVE_DEBUG_LOG_FORMAT
    WaveDebugLogFormat(file);
#endif
#ifdef SDL_WAVE_DEBUG_DUMP_FORMAT
    WaveDebugDumpFormat(file, RIFFchunk.length, fmtchunk.length, datachunk.length);
#endif

    WaveFreeChunkData(chunk);

    // Process data chunk.
    *chunk = datachunk;

    if (chunk->length > 0) {
        result = WaveReadChunkData(src, chunk);
        if (result < 0) {
            return false;
        } else if (result == -2) {
            return SDL_SetError("Could not seek data of WAVE data chunk");
        }
    }

    if (chunk->length != chunk->size) {
        // I/O issues or corrupt file.
        if (file->trunchint == TruncVeryStrict || file->trunchint == TruncStrict) {
            return SDL_SetError("Could not read data of WAVE data chunk");
        }
        // The decoders handle this truncation.
    }

    if (!SDL_WaveDecodeSamples(file, audio_buf, audio_len)) {
        return false;
    }
    if (!SDL_WaveSetAudioSpec(file, spec)) {
        return false;
    }

    // Report the end position back to the cleanup code.
    if (RIFFlengthknown) {
        chunk->position = RIFFend;
    } else {
        chunk->position = lastchunkpos;
    }

    return true;
}

bool SDL_LoadWAV_IO(SDL_IOStream *src, bool closeio, SDL_AudioSpec *spec, Uint8 **audio_buf, Uint32 *audio_len)
{
    bool result = false;
    WaveFile file;

    if (spec) {
        SDL_zerop(spec);
    }
    if (audio_buf) {
        *audio_buf = NULL;
    }
    if (audio_len) {
        *audio_len = 0;
    }

    // Make sure we are passed a valid data source
    CHECK_PARAM(!src) {
        SDL_InvalidParamError("src");
        goto done;
    }
    CHECK_PARAM(!spec) {
        SDL_InvalidParamError("spec");
        goto done;
    }
    CHECK_PARAM(!audio_buf) {
        SDL_InvalidParamError("audio_buf");
        goto done;
    }
    CHECK_PARAM(!audio_len) {
        SDL_InvalidParamError("audio_len");
        goto done;
    }

    SDL_zero(file);
    file.riffhint = WaveGetRiffSizeHint();
    file.trunchint = WaveGetTruncationHint();
    file.facthint = WaveGetFactChunkHint();

    result = WaveLoad(src, &file, spec, audio_buf, audio_len);
    if (!result) {
        SDL_free(*audio_buf);
        *audio_buf = NULL;
        *audio_len = 0;
    }

    // Cleanup
    if (!closeio) {
        SDL_SeekIO(src, file.chunk.position, SDL_IO_SEEK_SET);
    }
    WaveFreeChunkData(&file.chunk);
    SDL_free(file.decoderdata);
done:
    if (closeio && src) {
        SDL_CloseIO(src);
    }
    return result;
}

bool SDL_LoadWAV(const char *path, SDL_AudioSpec *spec, Uint8 **audio_buf, Uint32 *audio_len)
{
    SDL_IOStream *stream = SDL_IOFromFile(path, "rb");
    if (!stream) {
        if (spec) {
            SDL_zerop(spec);
        }
        if (audio_buf) {
            *audio_buf = NULL;
        }
        if (audio_len) {
            *audio_len = 0;
        }
        return false;
    }
    return SDL_LoadWAV_IO(stream, true, spec, audio_buf, audio_len);
}
