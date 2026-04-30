# LVL System — TODO / Proposals

## Rename Suggestions

### Types

| Current | Proposed | Reason |
|---------|----------|--------|
| `LevelHunk` | `LevelFile` or `LevelChunk` | "Hunk" is Amiga terminology; confusing on PC. A "LevelHunk" is really just the loaded file data |
| `LevelHunkSection` | `LevelSection` or `LevelSectionEntry` | Simpler, "Hunk" is redundant here |
| `stru2` | `RenderHandlerCacheEntry` or `BlitterCacheNode` | It's a cached render handler lookup result |
| `stru3` | `RenderHandlerDef` or `BlitterTableEntry` | Static table entry mapping hunk_id → init/draw/cleanup |
| `LevelDesc` | — (fine as-is) | |
| `LevelBoxdSurface` | `BoxdGridDef` | It's a collision grid definition, not a "surface" |
| `LevelCplcSurface` | `CplcLayer` or `CplcEntityList` | It's a layer of pre-sorted entity placement data |
| `LevelMapdSurface` | `MapdLayerDef` | Already proposed in mapd-todo.md |
| `LevelMobdSurface` | `MobdAnimSet` or `MobdFrameSet` | Contains animation frames for one MOBD entry |

### Functions

| Current | Proposed | Reason |
|---------|----------|--------|
| `LVL_ReadHunk` | `LVL_ReadIFFChunk` or `LVL_ReadChunk` | It reads an IFF chunk (tag + big-endian size + payload) |
| `HUNK_FixPointers` | `LVL_RelocatePointers` or `LVL_ApplyFixups` | Standard relocation terminology |
| `LVL_RunLevel` | `LVL_InitSubsystems` | More descriptive — it initializes all subsystems from loaded data |
| `LVL_Deinit` / `LVL_cleanup` | Unify to `LVL_Shutdown` | Two names for same function is confusing |
| `LVL_SubstHunk` | `LVL_InjectSection` or `LVL_ReplaceSection` | "Substitute" is OK but "inject" better describes shared sprite pattern |
| `LVL_FindSection` | — (fine as-is) | |
| `LVL_SysInit` | — (fine as-is) | |
| `LVL_SysCleanup` | — (fine as-is) | |
| `stru2_create` | `BlitterCache_Lookup` or `RenderHandler_Resolve` | Looks up/caches a render handler by hunk ID |
| `stru2_init` | `BlitterCache_Init` | |
| `stru2_cleanup` | `BlitterCache_Cleanup` | |
| `stru2_4104A0` | `BlitterCache_InitAll` | Calls mode_init on all cached entries |
| `stru2_4104E0` | `BlitterCache_CleanupAll` | Calls mode_cleanup on all cached entries |
| `sub_40E3E0` | `LVL_GetResolutionDir` or `GetLevelResDir` | Returns "320" or "640" based on screen width |
| `GAME_PrepareSuperLvl` | — (fine as-is, very descriptive) | |
| `sub_422F60` | `GAME_LoadMissionLevel` or `GAME_StartMission` | This is the main mission load entry point |
| `kknd_mfree` | `LVL_Free` or just inline `free()` | Trivial wrapper around free() |

### Globals

| Current | Proposed | Reason |
|---------|----------|--------|
| `g_level` | `g_loaded_level` | Distinguish from `g_current_lvl` (the active/running one) |
| `g_current_lvl` | `g_active_level` | Currently running level (set by LVL_RunLevel) |
| `g_current_lvl_sections` | `g_active_level_sections` | Follows from above |
| `g_wait_lvl` | `g_splash_level` or `g_loading_screen_lvl` | More descriptive |
| `g_supspr_lvl` | `g_shared_sprites_lvl` | What it is |
| `g_prev_level_id` | `g_last_loaded_level_id` | Clarifies it's for reload optimization |
| `g_level_loading_lock` | `g_level_transitioning` or `g_subsystems_loading` | "lock" implies mutex; it's really a flag |
| `g_render_handlers` | `g_blitter_table` or `g_render_handler_defs` | Static table of renderer definitions |
| `g_4795E8_tail` / `g_4795E8_pool` etc. | `g_blitter_cache_tail` / `g_blitter_cache_pool` | Named by address, unreadable |
| `byte_47A020[80]` | `g_level_base_path` or `g_lvl_install_path` | Used to construct level file paths for CD/HD install |
| `g_current_lvl_mapd_verified` | `g_mapd_active` or `g_has_mapd` | "verified" implies validation; it's just "section found" |
| `g_current_lvl_boxd_verified` | `g_boxd_active` or `g_has_boxd` | Same |
| `g_current_lvl_cplc_verified` | `g_cplc_active` or `g_has_cplc` | Same |
| `g_current_lvl_mobd_verified` | `g_mobd_active` or `g_has_mobd` | Same |
| `dword_478AA0` | `g_screen_resolution_width` or `g_display_width` | Used to determine 320 vs 640 path |

---

## Struct Issues

### LevelHunk.sections Layout Is Misleading
```c
struct LevelHunk {
    LevelHunkSection *sections[1];  // flexible array
};
```
**Problem**: The decompiler shows `sections[1]` but this is really `sections[0]` — a flexible array member. After pointer fixup, `sections[0]` contains the actual `LevelHunkSection*` pointing into the DATA blob where the section directory lives. The LevelHunk itself is the DATA blob's base address, and `sections[0]` is the first DWORD in it (a pointer to the section table within the same allocation).

**Reality**: `LevelHunk` is not a proper struct — it's just the raw allocated memory. The first DWORD happens to be a pointer (after fixup) to an array of LevelHunkSection entries embedded further in the same buffer.

**Proposed clarification**:
```c
// The "LevelHunk" is the entire DATA blob. Its first field is a pointer
// (fixed up from offset) to the section directory embedded within itself.
struct LevelFile {
    LevelSection *section_dir;  // points into this same allocation
    // ... rest of DATA blob follows ...
};
```

### LevelHunkSection Pointer Awkwardness
In `LVL_SubstHunk`, the code does:
```c
src_hunk_name = *(LevelHunkSection **)src->sections[0].name;
```
This double-indirection suggests `sections[0]` is being treated as a raw DWORD containing the address, not as a proper struct. The `.name` field access on offset 0 is how the decompiler represents reading the first 4 bytes. In reality it's `*(_DWORD*)data_blob` = pointer to section table.

### stru3 Table — Unknown Size/Contents
`g_render_handlers` is a `stru3` but we never see its initialization in the decompiled code. It's likely initialized from static data. The `hunk` field values map to pixel format/rendering modes embedded in MOBD sprite data. Need to find the table initialization or .data section definition.

---

## Decompilation Issues

### LVL_ReadHunk Byte Swap Is Garbled
```c
LOBYTE(v6) = 0;
HIBYTE(v6) = BYTE2(ElementCount);
ElementCount = HIBYTE(ElementCount) | v6 | (((ElementCount << 16) | ElementCount & 0xFF00) << 8);
```
This is the decompiler's mangled representation of a big-endian to little-endian 32-bit swap:
```c
ElementCount = bswap32(ElementCount);
// or: ((x>>24)&0xFF) | ((x>>8)&0xFF00) | ((x<<8)&0xFF0000) | ((x<<24)&0xFF000000)
```

### HUNK_FixPointers Missing Iterator Type
The function loops using `(KKND::LevelHunk *)i = rllc + 1` which is really iterating over a DWORD array. The relocation entries are DWORDs, not `LevelHunk*`. The decompiler typed the pointer wrong.

**Proposed rewrite for clarity**:
```c
BOOL HUNK_FixPointers(void *data, void *rllc_section) {
    uint32_t num_entries = *(uint32_t*)rllc_section;  // first DWORD = count
    uint32_t *entries = (uint32_t*)rllc_section + 1;  // fixup array follows

    for (uint32_t i = 0; i < num_entries; ) {
        uint32_t entry = entries[i++];
        if (entry & 0x80000000) {
            // Mode 3: renderer fixup
            uint32_t offset = entry & 0x7FFFFFFF;
            int hunk_id = *(int*)((char*)data + offset);
            stru2 *cached = BlitterCache_Lookup(hunk_id);
            if (!cached) return FALSE;
            *(Blitter*)((char*)data + offset) = cached->mode_render;
        } else if (entry & 0x40000000) {
            // Mode 2: batch pointer fixup
            uint32_t offset = entry & 0xBFFFFFFF;
            uint32_t count = entries[i++];  // next entry is count
            uint32_t *ptrs = (uint32_t*)((char*)data + offset);
            for (uint32_t j = 0; j < count; j++)
                ptrs[j] += (uint32_t)data;
        } else {
            // Mode 1: simple pointer fixup
            uint32_t offset = entry;
            *(uint32_t*)((char*)data + offset) += (uint32_t)data;
        }
    }
    return TRUE;
}
```

### LVL_SubstHunk Decompilation Is Severely Mangled
The double-indirection through `.name` field and `__shifted` pointer types makes the logic nearly unreadable. The actual algorithm is simple (find section by name in both src and dst, swap pointer), but decompiler output obscures it with type casts.

---

## Enum Proposals

### HunkFixupType (for RLLC entries)
```c
enum HunkFixupType {
    Fixup_SimplePointer = 0x00000000,   // bit31=0, bit30=0: single pointer relocation
    Fixup_BatchPointer  = 0x40000000,   // bit31=0, bit30=1: N consecutive pointers
    Fixup_Renderer      = 0x80000000,   // bit31=1: replace hunk_id with blitter fn ptr
};
#define FIXUP_TYPE_MASK    0xC0000000
#define FIXUP_OFFSET_MASK  0x3FFFFFFF
```

### LevelFileType (for documentation, not in code)
```c
enum LevelFileType {
    LevelFile_Wait,        // wait.lvl — splash/loading screen
    LevelFile_SuperSprite, // supspr.lvl / sprites.lvl — shared sprite bank
    LevelFile_Super,       // super.lvl — menu system
    LevelFile_Mission,     // surv_XX.lvl / mute_XX.lvl — campaign missions
    LevelFile_Multiplayer, // mlti_XX.lvl — multiplayer maps
    LevelFile_FMV,         // fmv.lvl — mission briefing
};
```

### LevelSection FourCC Tags
```c
// Known section tags (used as 4-char identifiers)
#define SECTION_MAPD  "MAPD"   // Map/background tiles
#define SECTION_MOBD  "MOBD"   // Mobile object (sprite) data
#define SECTION_BOXD  "BOXD"   // Box/collision grid
#define SECTION_CPLC  "CPLC"   // Entity placement
// Chunk tags in .lvl file (IFF headers)
#define CHUNK_DATA    "DATA"   // Level data payload
#define CHUNK_RLLC    "RLLC"   // Relocation/fixup table
```

### LevelInitFlag (for the various dword_479Dxx booleans)
```c
enum LevelInitFlag {
    LevelInit_Tasks         = 0x001,  // g_tasks_initialized
    LevelInit_Timer         = 0x002,  // g_timer_initialized
    LevelInit_Sound         = 0x004,  // dword_479DBC
    LevelInit_RenderCache   = 0x008,  // dword_479DEC (stru2_4104A0)
    LevelInit_SoundStreams  = 0x010,  // dword_479DC4 (sub_43B6B0)
    LevelInit_MAPD          = 0x020,  // g_mapd_initialized
    LevelInit_MOBD          = 0x040,  // g_mobd_initialized
    LevelInit_BOXD          = 0x080,  // g_boxd_collisions_initialized
    LevelInit_CPLC          = 0x100,  // g_cplc_initialized
    LevelInit_Unknown445    = 0x200,  // dword_479DC0 (sub_445690)
    LevelInit_Fade          = 0x400,  // g_fade_initialized
    LevelInit_Window        = 0x800,  // dword_479DD8 (sub_428310)
};
```
**Note**: Could replace 13 separate boolean globals with one bitfield. But matching original code may be preferred.

---

## Architecture Observations

### Binary Dump Is Platform-Locked
The DATA hunk is a raw 32-bit x86 memory image:
- Pointers are 32-bit
- Struct layout assumes x86 alignment
- IFF headers are big-endian (Amiga heritage) but payload is little-endian (x86)

This mixed endianness (big-endian chunk headers, little-endian data) suggests the original level tools were ported from Amiga/68k (where IFF originated) but the game data targets x86.

### supspr.lvl vs sprites.lvl — Probable Same File
Both paths exist:
- `supspr.lvl` — used by `GAME_PrepareSuperLvl` (menu path)
- `sprites.lvl` — used by `MOVIE_Play` (FMV path) and `sub_422F60` (mission path)

They serve the same purpose (shared MOBD injection). Likely the same physical file with two names, or two references to the same concept from different development periods.

### g_level vs g_current_lvl — Confusing Dual State
- `g_level` = the loaded LevelHunk* (returned from LVL_LoadLevel)
- `g_current_lvl` = the active running level (set by LVL_RunLevel)

These are usually the same pointer, but the distinction matters:
- `g_level` owns the memory (freed with `kknd_mfree`)
- `g_current_lvl` is the "active" one (subsystems reference it)
- After `LVL_Deinit`, `g_current_lvl = NULL` but `g_level` may still hold the pointer until freed

### Multiplayer Level ID Gap
Campaign levels = 0–29. Multiplayer = 48–67. The gap (30–47) is unused. This suggests:
- Original design may have planned more campaign levels (30 per side = 60 total?)
- Or multiplayer was added later with a fixed offset

### Level Reload Optimization
When transitioning between missions, if same faction continues:
```c
if (g_current_lvl_id != g_prev_level_id) {
    // load new level file
} else {
    // skip load, just re-run
}
```
But for menu transitions or multiplayer, level is always freed and sprites are freed too.

### Victory Condition Bits
`victory_condition_bits` = bitmask stored in `dword_47A3D0`. The mission outcome task checks these bits. Common values in the table:
- 771 = 0x303 (bits 0,1,8,9)  — most common, standard "destroy all enemies"
- 787 = 0x313 (bits 0,1,4,8,9)
- 768 = 0x300 (bits 8,9)      — escort/rescue missions?
- 1795 = 0x703 (bits 0,1,8,9,10)
- 775 = 0x307 (bits 0,1,2,8,9)

Need to reverse the bit meanings from the mission outcome task handler.

### Disabled Units Mask
Bitmask in `LevelDesc.disabled_units_mask`. Each bit disables one unit type for the mission:
- 0x0010 = disable specific unit (used in surv_05)
- 0x4000, 0x8000 = later tech units
- 0x200000, 0x400000, 0x800000 = advanced units

Need to map bit positions to UnitType enum values.

---

## Investigation Needed

1. **g_render_handlers table contents** — Where is it initialized? What hunk IDs exist? Maps rendering modes to sprite pixel formats.
2. **supspr.lvl vs sprites.lvl** — Are these the same file? Check actual game directory.
3. **RLLC first DWORD** — Is it truly `num_entries` or `sections[0]`? The decompiled code uses `rllc->sections[0]` as the count, which is suspicious given it's typed as a pointer.
4. **Victory condition bit meanings** — Reverse from mission_check_on_death_victory_conditions and the mission outcome task.
5. **Disabled units bit mapping** — Match bit positions to UnitType enum.
6. **superlvl_ai_cost_reduction** — Values 256/280/296/304/328 appear. Is 256 = 1.0x (no reduction)? Formula: `cost * reduction / 256`? Or `cost - cost * reduction / 256`?
7. **_level_desc_field_16** — Always 0 in the table. Padding? Unused difficulty field?
8. **LVL_InitTasks parameter** — `LVL_InitTasks(0)` — what does stack_size=0 mean? Default?
9. **dword_479DC0 (sub_445690)** — What subsystem is this? Between CPLC and FADE in init chain.
10. **dword_479DD8 (sub_428310)** — What subsystem? Last in init chain.
11. **Level transition memory management** — When does `g_level` get freed? Confirm it's `kknd_mfree(g_level)` only on full exit or faction change.
12. **CPLC layer 12** — `CPLC_select(12)` called in some paths. What menu/screen uses layer 12?
