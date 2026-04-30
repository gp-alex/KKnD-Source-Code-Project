# XMark — Mission Objective Marker

## Overview

XMark is a **visual "go here" indicator** for mission objectives. Displays an animated X cursor on the game map (world-space) plus a temporary directional arrow on the minimap/HUD (screen-space). Used only in 3 specific single-player missions.

## Struct: XMark (0x0C bytes)

```
Offset  Type                             Field      Suggested Name
------  ----                             -----      --------------
0x00    void (__fastcall*)(XMark*)       mode       → tick_fn (always mode_null — no-op)
0x04    Task*                            task       → owning task
0x08    Entity*                          entity     → animated X sprite
```

**Note:** `mode` is set to `mode_null` on init and **never reassigned** anywhere in the codebase. All `x_mark->mode(x_mark)` calls are dead code (no-op). The mode/state-machine pattern is vestigial — likely the original code had states that were simplified away, or it follows the project convention of "every ctx struct starts with a mode function pointer" even when unused.

## Functions

### `task_draw_x_mark` (0x426710) — World-Space Marker

Entry point / coroutine for the map X marker.

```
init:
    TASK_Alloc(task, 0x0C)  → XMark
    set mode = mode_null, entity = task->entity, task = task
    entity_anim_load(entity, 820)  → animated X cursor frame (MobdId_Cursors)
    entity->z = 0 (ground level)
    entity->collider = g_null_collision (no collision)
    spawn task_draw_x_mark_2 (HUD arrow)

    switch (g_current_lvl_id):
        Surv_04_RescueTheScout:
            position = (47616, 390400)    ← scout rescue zone
        Surv_06_ExterminateTheVillage:
            position = (38400, 628736)    ← village location
            call mode(x_mark); return
        Mute_04_RaidTheFort:
            position = (59392, 465408)    ← fort location
            call mode(x_mark); return

    call mode(x_mark)  ← no-op
```

**Key:** Entity can be spawned with or without a parent entity:
- With parent (`unit->entity`): marker follows the unit (scout missions) — entity_create_ex passes parent
- Without parent (`nullptr`): marker is static at fixed coordinates (exterminate/load-game)

### `task_draw_x_mark_2` (0x426680) — HUD Arrow Overlay

Screen-space indicator, spawned by `task_draw_x_mark`:

```
entity->rend->render_state_handler = render_state_handler_ui  ← screen coords
entity->z = 5  ← above game world
position = (g_screen_width << 7, g_screen_height << 7)  ← bottom-right corner
entity->collider = g_null_collision
entity_anim_load(entity, Mute_04 ? 868 : 852)  ← arrow/pointer frame
TASK_yield(task, Task_Sleep, 600)  ← visible for 600 frames (~10 sec at 60fps)
entity_remove + TASK_Terminate  ← self-destructs
```

Two different animation frames:
- **852**: standard "look here" arrow (Survivors missions + Mute_05/06)
- **868**: alternate arrow (Mute_04_RaidTheFort only)

## Usage Sites

| Location | Level | Context | Parent Entity |
|----------|-------|---------|---------------|
| Line 34133 | Surv_04, Mute_04 | Save game load — restoring `g_scout` marker | `nullptr` (static) |
| Line 38099 | Surv_04 | Scout rescue event — attaches to scout unit | `unit->entity` |
| Line 38105 | Mute_04 | Raid the Fort — attaches to first player unit | `unit->entity` |
| Line 38250 | Surv_06 | Exterminate Village — standalone objective marker | `nullptr` (static) |

All spawned via: `entity_create_ex(MobdId_Cursors, parent, task_draw_x_mark, TaskKind_Coroutine, nullptr)`

## Mission Objective Coordinates

| Level | X | Y | What's There |
|-------|---|---|-------------|
| `Surv_04_RescueTheScout` | 47616 | 390400 | Scout rescue zone |
| `Surv_06_ExterminateTheVillage` | 38400 | 628736 | Evolved village |
| `Mute_04_RaidTheFort` | 59392 | 465408 | Survivor fort |

## Script Handler Table

`task_draw_x_mark` is registered in `g_script_handlers[]` (line ~993), so CPLC mission scripts can reference it by index. This is how mission-specific spawning works — the level's CPLC data triggers the X mark creation.

## Decompilation Issues

1. **mode is dead code**: `x_mark->mode(x_mark)` called 3 times in `task_draw_x_mark` but mode is always `mode_null`. Decompiler faithfully preserved the calls but they're no-ops. Could remove entirely.
2. **Calling convention mismatch**: mode declared `__fastcall` in struct but called as `__thiscall` in decompiled code (`((void (__thiscall *)(XMark *))x_mark->mode)(x_mark)`). Both pass first arg in ECX on x86 — functionally identical, decompiler artifact.
3. **`is_on_collision_grid = 1` repeated**: Appears twice for same entity in `task_draw_x_mark_2`. Decompiler artifact — likely compiler set flag, then immediately used entity pointer, and decompiler couldn't merge.

## Naming Suggestions

| Current | Suggested |
|---------|-----------|
| `XMark` | `MissionObjectiveMarker` or keep `XMark` (apt, it's literally an X) |
| `task_draw_x_mark` | `task_xmark_world` |
| `task_draw_x_mark_2` | `task_xmark_hud_arrow` |
| `XMark::mode` | **remove** or rename to `tick_fn` (vestigial) |
| `g_scout` | keep — it IS the scout unit |

## Summary

XMark = simplest possible system. 12-byte struct with dead mode pointer. Spawns animated X cursor on map + temporary HUD arrow. Only used in 3 campaign missions as "go here" indicator. World marker persists until mission ends; HUD arrow self-destructs after 10 seconds.
