# stru2 — Blitter Cache System

## Identity

`stru2` is a **runtime blitter/renderer cache**. During level loading, `HUNK_FixPointers` resolves hunk IDs (embedded pixel format identifiers in MOBD sprite data) into actual blitter function pointers. `stru2` caches these lookups so each hunk ID is resolved once, then reused for all sprites sharing the same format.

**stru3** is the **static definition table** — a compile-time array mapping hunk IDs to (init, draw, cleanup) function triples.

---

## Structs

### stru2 — CachedBlitter (24 bytes, 0x18)
```
Offset  Type                Field           Proposed Name
0x00    stru2*              next            next
0x04    stru2*              prev            prev
0x08    int                 hunk            blitter_id
0x0C    BOOL(*)(void)       mode_init       blitter_init
0x10    Blitter             mode_render     blitter_draw
0x14    int(*)(void)        mode_cleanup    blitter_cleanup
```

### stru3 — BlitterDefinition (16 bytes, 0x10)
```
Offset  Type                Field           Proposed Name
0x00    int                 hunk            blitter_id
0x04    BOOL(*)(void)       mode_init       blitter_init
0x08    Blitter             mode_draw       blitter_draw
0x0C    int(*)(void)        mode_cleanup    blitter_cleanup
```

`Blitter` typedef: `int (__fastcall *)(RenderCommand *cmd, int mode)` where mode: 0=render, 1=query width, 2=query height.

---

## Globals

| Current | Proposed | Purpose |
|---------|----------|---------|
| `g_4795E8_tail` | `g_blitter_cache_tail` | Active list tail (sentinel-based) |
| `g_4795E8_head` (implicit at tail-4) | `g_blitter_cache_head` | Active list head (sentinel-based) |
| `g_4795E8_pool` | `g_blitter_cache_pool` | Malloc'd block for pool allocator |
| `g_4795E8_free` | `g_blitter_cache_free` | Free list head |
| `g_render_handlers` | `g_blitter_definitions` | Static stru3 array — maps hunk IDs to blitter funcs |

**NOTE**: IDA shows `g_4795E8_tail` and `g_4795E8_pool` but NOT `g_4795E8_head` and `g_4795E8_free`. Head lives at `&g_4795E8_tail - 4` and free at `&g_4795E8_pool - 4` in memory. The sentinel trick: head/tail point to `&tail` when empty.

---

## Functions

| Current | Address | Proposed | Purpose |
|---------|---------|----------|---------|
| `stru2_init` | 0x4103C0 | `blitter_cache_init` | Alloc pool of 16 entries, init free/active lists |
| `stru2_create` | 0x410410 | `blitter_cache_get_or_create` | Lookup by hunk ID; cache hit → return existing; miss → alloc from free list, copy from g_blitter_definitions |
| `stru2_4104A0` | 0x4104A0 | `blitter_cache_init_all` | Walk active list, call `mode_init()` on each entry |
| `stru2_4104E0` | 0x4104E0 | `blitter_cache_cleanup_all` | Walk active list, call `mode_cleanup()` on each entry |
| `stru2_cleanup` | 0x410510 | `blitter_cache_free_pool` | Free pool memory |

---

## Flow

### Level Load Sequence
```
LVL_SysInit
  └─ stru2_init()           — Alloc pool (once per session)

LVL_LoadLevel(filename)
  └─ HUNK_FixPointers(data, rllc)
       └─ For each negative RLLC entry:
            read hunk_id from data
            stru2_create(hunk_id) — get/create cached blitter
            patch data[offset] = cached->mode_render   ← function pointer!

LVL_RunLevel(lvl)
  └─ stru2_4104A0()         — Call mode_init() on all cached blitters

LVL_Deinit
  └─ stru2_4104E0()         — Call mode_cleanup() on all cached blitters

LVL_SysCleanup
  └─ stru2_cleanup()        — Free pool memory
```

### stru2_create Logic (pseudocode)
```
blitter_cache_get_or_create(hunk_id):
    // 1. Cache hit — search active list
    for entry in active_list:
        if entry.blitter_id == hunk_id:
            return entry          // already cached

    // 2. Cache miss — search static definitions table
    for def in g_blitter_definitions:
        if def.blitter_id == hunk_id:
            // 3. Alloc from free list
            entry = free_list.pop()
            if !entry: return NULL  // pool exhausted
            active_list.push(entry)
            entry.blitter_id = hunk_id
            entry.blitter_init = def.blitter_init
            entry.blitter_draw = def.blitter_draw
            entry.blitter_cleanup = def.blitter_cleanup
            return entry

    return NULL  // unknown hunk ID
```

---

## Pool Allocator Pattern

Same pattern used throughout codebase (AI lists, sidebar, render nodes):
- Malloc single block for N entries
- Chain into free list via `next` pointer
- Active list is sentinel doubly-linked list (head/tail point to `&tail` when empty)
- 16 entries × 24 bytes = 0x180

The sentinel trick: `tail->next == &tail` means empty. Iterate `for (p = tail; p != &tail; p = p->next)`.

---

## HUNK_FixPointers Integration

In `HUNK_FixPointers` (0x437DA0), RLLC entries with **negative** offset (bit 31 set) are blitter fixups:
```c
if (name < 0) {
    ptr = data + (name & 0x7FFFFFFF);  // clear sign bit for offset
    hunk_id = *ptr;                     // read embedded hunk ID
    cached = stru2_create(hunk_id);     // resolve to blitter
    *ptr = cached->mode_render;         // patch with function pointer
}
```

Positive RLLC entries are regular pointer relocations. Entries with bit 30 set (0x40000000) are array relocations.

The cached `mode_render` value is stashed between iterations (`v15`/`v17`) as an optimization — if consecutive fixups reference the same hunk ID, skip the lookup.

---

## Design Insight

This is a classic **flyweight + cache** pattern for a fixed-function rendering pipeline:
- Level data embeds numeric renderer IDs in sprite structures
- At load time, IDs are resolved to function pointers and **patched in-place**
- After fixup, sprite data contains direct function pointers — zero runtime dispatch overhead
- The cache ensures each unique renderer mode is initialized exactly once per level

---

## Naming Convention

stru2/stru3 follow the **"system + cache"** pattern seen in other subsystems:
- Static definition table (stru3 → `BlitterDefinition`) — analogous to `UnitStats`
- Runtime cache node (stru2 → `CachedBlitter`) — analogous to `Unit` referencing `UnitStats`

Proposed struct renames:
- `KKND::stru2` → `KKND::CachedBlitter`
- `KKND::stru3` → `KKND::BlitterDefinition`

---

## Open Questions

1. **g_blitter_definitions table contents** — Static data, never initialized in code. Need IDA .data section or runtime dump to see what hunk IDs exist and which blitter functions they map to.
2. **What hunk IDs exist?** — These correspond to pixel formats/rendering modes in MOBD sprites. Likely values: 1=8bpp paletted, 2=RLE, etc. Need the actual table.
3. **Pool size (16)** — Is 16 always sufficient? No overflow handling — just returns NULL. If a level needs >16 unique blitter types, `HUNK_FixPointers` returns 0 and level load fails.
4. **mode_init/mode_cleanup semantics** — What do individual blitter init/cleanup do? Likely set up DirectDraw surface format, palette handling, etc.
