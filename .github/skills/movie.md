# Movie System — VBC Video Playback

## Overview

KKND uses custom `.vbc` video format for FMV cutscenes and mission briefings. System handles file I/O, block-based video decoding, palette animation, DirectSound audio, and double-buffered rendering integrated into the render pipeline.

Files stored in `FMV\` directory. Level-specific briefings referenced via `g_lvl_desc[lvl_id].vbc_filename`.

---

## File Format (.vbc)

### Header (60 bytes = 0x3C)
Read as raw `MovieFrame` struct, then fields are cherry-picked into `MovieHeader`:
```
Offset  Size  MovieFrame field → MovieHeader field
0x14    2     _movie_frame_14  → _movie_header_0 (bpp, low 4 bits)
0x16    2     width            → width
0x18    2     height           → height
0x1A    2     _movie_frame_1A  → _movie_header_6
0x1C    2     _movie_frame_1C  → _movie_header_8
0x1E    2     num_frames       → num_frames
0x20    2     _movie_frame_20  → sound_flags
0x22    4     _movie_frame_22  → sound_samples_per_sec
```

### Frame Data
- Each frame prefixed by 4-byte `size` field
- Frames are chained: next frame's size lives at `&frame.size + current_size`
- Last frame's size excludes 4 bytes (no next-frame pointer)
- Frame body read into `frame._movie_frame_4` onward

### Frame Flags (`_movie_frame_4` = `_movie_330`)
Bit field controlling which data sections present in frame:
```
Bit 0 (0x01) — Position offset: _movie_frame_6 = x_offset, _movie_frame_8 = y_offset
                Converted: offset = x + y * width
Bit 2 (0x04) — Sound data: 4-byte size prefix, then (size-4) bytes of PCM audio
Bit 3 (0x08) — Video data: block-decoded pixel data (see Decoding below)
Bit 4 (0x10) — Palette data: palette entries for indexed color
Bit 5 (0x20) — Timing parameter: 2-byte value → _movie_header_E (frame duration ms)
Bit 6 (0x40) — Subtitle text: 4-byte size prefix, then text string
```

Each section is length-prefixed with 4-byte size at its start. Parser walks through sections sequentially using `p_movie_frame_6` pointer that advances past each consumed section.

---

## Video Decoding

### Block-Based Decompression
Two decoders depending on bits-per-pixel:
- **BPP=2** (16-bit): `sub_45D3B8` — processes 8x4 pixel blocks (32 bytes stride)
- **BPP=1** (8-bit): `sub_45A48E` — processes 4x4 pixel blocks (8 bytes stride)

Both work on `width/4 × height/4` block grid. Each byte in compressed stream encodes 4 blocks via 2-bit codes packed in pairs:
```
byte & 0xC0 >> 4  → block 0 dispatch
byte & 0x30 >> 2  → block 1 dispatch
byte & 0x0C       → block 2 dispatch
byte & 0x03       → block 3 dispatch
```

Dispatch goes through function pointer tables (`funcs_45A537[4]` / `funcs_45D4B5[4]`):
- Code 0: Copy block from delta buffer (previous frame)
- Code 1: Read codebook index, copy block from codebook + delta
- Code 2: Read raw block data
- Code 3: (varies by palette mode)

### Palette Modes (BPP=2 only, `sub_45D3B8`)
`a9` parameter selects sub-mode:
- **0**: Default codebook decode (`sub_45D147`, `sub_45D25B`, `sub_45D2C1`)
- **1**: New palette applied (`sub_45D1A4` variant, `sub_45D2A4`, `sub_45D35B`)
- **2**: Reuse existing palette transform (`sub_45D19B` variant, `sub_45D288`, `sub_45D2FE`)

### Lookup Table (`_movie_344[256]`)
Built during `MOVIE_read_file`. Maps 16×16 block grid to byte offsets:
```c
for (v10 = -8; v10 < 8; v10++)
  for (v11 = -8; v11 < 8; v11++) {
    idx = (16 * (v10 & 0xF)) | (v11 & 0xF);
    _movie_344[idx] = bpp * (v11 + width * v10);
  }
```
Encodes relative pixel offsets within a 16×16 neighborhood for block addressing.

### Double Buffering
- `_movie_332` toggles 0/1 each frame via XOR
- Current decode target: `*((&frame_front_buffer) + _movie_332)` = decode into active buffer
- Previous frame source: `*((&frame_front_buffer) + (_movie_332 ^ 1))` = delta reference
- After decode: `_movie_header_10 = decoded buffer pointer` (exposed to renderer)

---

## Palette System

### In-Frame Palette Updates (BPP=1)
When frame has bit 4 set and BPP=1:
- `_movie_2C[0..1]` (__int16) = palette entry count (0 means 256)
- `_movie_2C[2..3]` (__int16) = palette start index
- `_movie_2C[4+]` = RGB triplets (3 bytes each)

### Palette Application (in `MOVIE_do_frame`)
After frame decoded, if `_movie_2C` count nonzero, iterates entries:
```c
// reads RGB from _movie_2C, writes to stru_477980.cbSize area (reused as palette buffer)
// then calls palette_40E400() to upload to hardware
```
Palette stored at `&stru_477980.cbSize` — this is a WAVEFORMATEX repurposed region (global overlap with sound format struct). The palette lives at `g_pal_4785DC`.

### 16-bit Palette Transform (`word_476DE0`)
For BPP=2 with palette flag: 512-byte (0x200) lookup table loaded from frame data. If `dword_476FE0` (base palette pointer) set, each entry transformed through it: `word_476DE0[i] = base_pal[original & 0x7FFF]`.

---

## Sound System

### Audio Format
Derived from `sound_flags`:
- Bit check `!= 8` → 16-bit PCM, else 8-bit PCM
- Bit `0x100` → stereo (2 channels), else mono
- `bytes_per_sample = ((flags != 8) + 1) * ((flags & 0x100) ? 2 : 1)`

### DirectSound Streaming
- 64KB (0x10000) circular DirectSound buffer created on first sound frame
- `MOVIE_play_sound()` initializes buffer, copies initial audio data
- Subsequent frames: Lock buffer at `dword_477944` write cursor, copy `sound_bytes_num` bytes
- On movie end: remaining buffer filled with 0x80 (silence for 8-bit unsigned PCM)
- Position tracking: `dword_477DC4` = last DS position, `dword_477DC8` = wrap accumulator

### Sync Timing
Two modes:
1. **Sound-based** (when `g_movie_sound_initialized`): DS buffer position → samples → milliseconds
   - `ms = (ds_position / bytes_per_sample) * 1000.0 / samples_per_sec`
2. **Timer-based** (no sound): `timeGetTime()` wall clock

Frame display gated: only show when `calculated_time >= dword_477940` (next frame deadline).
`dword_477940 += _movie_header_E` after each displayed frame (frame duration in ms).

---

## Render Integration

### MovieFrameImage
Implements `Blitter` interface — plugs into render pipeline via `mode_render = render_movie_frame`.

`render_movie_frame(cmd, mode)`:
- mode 0: render (clip test → blit via function pointers)
- mode 1: return width
- mode 2: return height

Actual blitting through:
- `dword_4785D8` = fullscreen blit (when `_movie_frame_image_14 = 1`) — assigned `sub_434EA0`
- `dword_478A04` = partial/viewport blit (when `_movie_frame_image_14 = 0`) — assigned `sub_434D00`
- `dword_4789F0` = clip rect setter — assigned `sub_434710`

### Dual Viewport (Mission Briefings)
`MOVIE_load_mission_briefing` sets up two render nodes:
- **Main** (`g_main_movie_img`): 320×240 viewport, top portion of frame
- **Secondary** (`g_secondary_movie_img`): 160×128 viewport, bottom portion
  - Pixel data offset: `_movie_header_10 + main_width * main_height`
- `_movie_frame_image_14 = 0` for briefings (uses viewport blit, not fullscreen)

---

## Key Functions

| Function | Line | Purpose |
|----------|------|---------|
| `MOVIE_read_file` | 74677 | Parse .vbc header, alloc buffers, build LUT |
| `MOVIE_free_buffers` | 74759 | Close file, free front/back buffers + struct |
| `MOVIE_45A070` | 74775 | Advance frame, read from file, call decode |
| `MOVIE_45A110` | 74799 | Decode frame: parse flags, dispatch sound/video/palette/text |
| `MOVIE_load` | 16121 | High-level: read file + create fullscreen render node |
| `MOVIE_do_frame` | 16160 | Frame pump: advance, stream audio, apply palette, sync timing |
| `MOVIE_play_sound` | 16405 | Init DirectSound buffer, copy first audio chunk |
| `MOVIE_cleanup` | 16450 | Stop sound, free movie, remove render nodes |
| `MOVIE_is_null` | 16475 | `return g_movie == nullptr` |
| `MOVIE_load_mission_briefing` | 16481 | Load with dual viewport (main + subtitles area) |
| `MOVIE_Play` | 35467 | Top-level: intro/briefing/ending dispatch with input loop |
| `render_movie_frame` | 21704 | Blitter callback: clip + blit frame to screen |
| `sub_45A48E` | 80065 | 8-bit block decoder (4×4 blocks) |
| `sub_45D3B8` | 99061 | 16-bit block decoder (8×4 blocks, palette modes) |
| `sub_40D450` | 16570~ | Subtitle text processor (word wrap, scroll buffer) |

---

## Global Variables

| Variable | Type | Purpose |
|----------|------|---------|
| `g_movie` | `Movie*` | Active movie instance (singleton) |
| `g_main_movie_img` | `MovieFrameImage` | Primary render image |
| `g_secondary_movie_img` | `MovieFrameImage` | Briefing secondary viewport image |
| `g_main_movie_rn` | `RenderNode*` | Primary render node |
| `g_secondary_movie_rn` | `RenderNode*` | Secondary render node |
| `g_movie_sound_initialized` | `BOOL` | DirectSound buffer active |
| `g_dsb_477DE4` | `IDirectSoundBuffer*` | DS buffer handle |
| `stru_477980` | `WAVEFORMATEX` | Audio format (also palette buffer overlap!) |
| `stru_477DD0` | `DSBUFFERDESC1` | DS buffer descriptor |
| `dword_477940` | `int` | Next frame display time (ms) |
| `dword_477944` | `int` | DS write cursor position |
| `dword_477DC4` | `int` | DS last read position |
| `dword_477DC8` | `int` | DS wrap counter (adds 0x10000 per wrap) |
| `dword_477DF0` | `int` | Frame ready flag |
| `dword_477DF4` | `int` | Frame decoded-but-not-displayed flag |
| `g_movie_frame` | `MovieFrame*` | Temp pointer used during read_file |
| `byte_477DF8[756]` | `char[]` | Subtitle text scroll buffer |
| `g_4780F8_movie_str` | `UiString*` | Subtitle UI string widget |
| `word_476DE0` | `__int16[256]` | 16-bit palette LUT |
| `dword_476FE0` | `int` | Base palette pointer for 16-bit transform |

---

## MOVIE_Play Types

| Type | Files | Behavior |
|------|-------|----------|
| 0 | `mh_fmv.vbc` → `intro.vbc` | Intro sequence, fullscreen, skip on any key |
| 1 | `g_lvl_desc[id].vbc_filename` | Mission briefing: dual viewport (320×240 + 160×128), subtitle text, level sprites loaded |
| 2 | `survout.vbc` / `evolvout.vbc` | Ending movies (Survivor final = survout, else evolvout) |

Briefing viewports: `REND_viewport_create(0, 38, 31, 320, 240)` and `REND_viewport_create(0, 240, 313, 160, 128)`.

---

## Naming Suggestions

### Struct Fields
| Current | Suggested | Evidence |
|---------|-----------|----------|
| `_movie_header_0_low_4_bits_for_bpp` | `bpp` | Low 4 bits = bits per pixel. Simple. |
| `_movie_header_6` | `block_width` or `frame_x_scale` | Copied from `_movie_frame_1A`, passed to `_movie_frame_image_28` |
| `_movie_header_8` | `block_height` or `frame_y_scale` | Copied from `_movie_frame_1C`, passed to `_movie_frame_image_2C` |
| `_movie_header_E` | `frame_duration_ms` | Added to `dword_477940` (frame display time) each frame |
| `_movie_header_10` | `current_buffer_ptr` | Set to decoded buffer address after frame decode |
| `_movie_header_28_str_len` | `subtitle_len` | Frame text/subtitle length |
| `_movie_header_28_str` | `subtitle_str` | Frame text/subtitle pointer |
| `_movie_2C[372]` | `palette_buffer` | Stores palette RGB data + count + start index |
| `_movie_1A0[80]` | unknown | Not accessed in movie code — possibly padding or unused |
| `_movie_330` | `frame_flags` | Copy of `_movie_frame_4` for current frame |
| `_movie_332` | `buffer_toggle` | XOR'd 0↔1 each frame for double buffer swap |
| `_movie_344[256]` | `block_offset_lut` | Block pixel offset lookup table |
| `_movie_780[131016]` | `frame_data_buffer` | Large buffer for raw frame bytes from file |
| `_movie_frame_4` | `flags` | Bit field for section presence |
| `_movie_frame_6` | `x_offset` | Position X when flag bit 0 set |
| `_movie_frame_8` | `y_offset` | Position Y when flag bit 0 set |
| `_movie_frame_1A` | `block_width` | Copied to `_movie_header_6` |
| `_movie_frame_1C` | `block_height` | Copied to `_movie_header_8` |
| `_movie_frame_20` | `sound_flags` | Copied to header sound_flags |
| `_movie_frame_22` | `sound_samples_per_sec` | Copied to header |
| `_movie_frame_image_14` | `is_fullscreen` | 1=fullscreen blit, 0=viewport blit |
| `_movie_frame_image_18` | `pixel_data` | Points to decoded frame buffer |
| `_movie_frame_image_28` | `scale_x` or `block_width` | From header._movie_header_6 |
| `_movie_frame_image_2C` | `scale_y` or `block_height` | From header._movie_header_8 |

### Functions
| Current | Suggested |
|---------|-----------|
| `MOVIE_45A070` | `MOVIE_advance_frame` |
| `MOVIE_45A110` | `MOVIE_decode_frame` |
| `sub_45A48E` | `MOVIE_decode_blocks_8bit` |
| `sub_45D3B8` | `MOVIE_decode_blocks_16bit` |
| `sub_40D450` | `MOVIE_process_subtitle` |
| `sub_40D430` | `MOVIE_init_subtitle` |

### Globals
| Current | Suggested |
|---------|-----------|
| `dword_477940` | `g_movie_next_frame_time` |
| `dword_477944` | `g_movie_ds_write_cursor` |
| `dword_477DC4` | `g_movie_ds_last_pos` |
| `dword_477DC8` | `g_movie_ds_wrap_accum` |
| `dword_477DF0` | `g_movie_frame_ready` |
| `dword_477DF4` | `g_movie_frame_decoded` |
| `word_476DE0` | `g_movie_palette_lut_16` |
| `dword_476FE0` | `g_movie_base_palette_ptr` |

---

## Decompilation Issues

1. **`stru_477980` dual-use**: WAVEFORMATEX struct reused as palette buffer (`&stru_477980.cbSize` = palette entry array). This is an actual code trick — the sound format struct is only 18 bytes, but palette_40E400 treats the memory after it as 1024-byte palette (256 × 4 bytes RGBX). The globals overlap.

2. **MovieFrame read as header**: `MOVIE_read_file` reads 0x3C bytes into `movie->frame` (MovieFrame), then cherry-picks fields into `movie->header` (MovieHeader). The file format header IS a MovieFrame struct — they share the same binary layout at different offsets. MovieHeader is a runtime-only summary.

3. **Frame size chaining**: In `MOVIE_45A070`, `p_frame->size = *(int *)((char *)&movie->frame.size + p_frame->size)` — reads next frame's size from current frame data at offset equal to current size. This is correct but confusing: each frame's data contains the next frame's size at its end.

4. **`_movie_1A0[80]`, `_movie_2E0[36]`, `_movie_304[9]`, `_movie_328[8]`**: Not accessed in any movie function found. Likely padding or decompiler sizing artifacts from the 0x748 memset region. Total Movie struct = 0x20748 bytes (alloc size), but only header + palette + toggle + file + buffers + LUT + frame + data[] are actively used.

5. **`g_movie_frame` global**: Used only as temp in `MOVIE_read_file` — set to `&movie->frame`, used to copy fields, never used elsewhere. Could be local. Decompiler may have promoted it to global because of aliasing.

6. **Block decoder function tables**: `funcs_45A537[4]` and `funcs_45D4B5[4]` are dispatch tables indexed by 2-bit codes. Each entry is one of: copy-from-delta, codebook-lookup, read-raw, or skip. The `__usercall` helpers (`sub_45A3C4`, `sub_45A3E7`, etc.) use non-standard calling conventions — registers carry block pointers and strides.

---

## Architecture Summary

```
MOVIE_Play(type)
 ├── MOVIE_load(filename, x, y, z)          [fullscreen]
 │   └── MOVIE_read_file(filename)          [parse header, alloc]
 │
 ├── MOVIE_load_mission_briefing(...)       [dual viewport]
 │   └── MOVIE_read_file(filename)
 │
 └── Game Loop:
     ├── INPUT_ProcessKeyboard()
     ├── MOVIE_do_frame()
     │   ├── MOVIE_45A070(movie)            [read + advance frame]
     │   │   └── MOVIE_45A110(movie, frame) [decode]
     │   │       ├── sub_45D3B8(...)        [16-bit blocks]
     │   │       └── sub_45A48E(...)        [8-bit blocks]
     │   ├── DirectSound streaming          [Lock/copy/Unlock]
     │   ├── Palette update                 [palette_40E400]
     │   └── Timing sync                    [DS position or timeGetTime]
     ├── REND_DoFrame()
     │   └── render_movie_frame(cmd, mode)  [Blitter callback]
     │       ├── dword_4785D8(y,w,h)        [fullscreen blit]
     │       └── dword_478A04(y,w,h)        [viewport blit]
     └── TIME_tick()
```
