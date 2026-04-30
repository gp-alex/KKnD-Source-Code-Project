# CPLC — TODO: Renames, Issues, Suggestions


## Missing Investigations

### TODO: Trace CplcSpawnParams.task and CplcSpawnParams.task_kind

These fields store handler function pointer and fiber/callback kind. Are they redundant with `g_scripts[task_type]`? If so, they may be legacy or level-editor overrides. Need to check if `cplc_entity_spawn` ever uses `spawn_params.task` instead of `g_scripts[task_type]->handler`.

### TODO: Trace CplcSpawnParams.mobd_id

Same question — is `spawn_params.mobd_id` ever used, or does `cplc_entity_spawn` always use `g_scripts[task_type]->mobd_id`? If redundant, the spawn_params copy may be level-editor data that the runtime ignores.

### TODO: Investigate skip_spawning flag

`cplc_entity_spawn` checks `if (!cplc->spawn_params.skip_spawning)`. When is this flag set? Is it in the level file data, or set dynamically? Could be used for conditional entity placement (difficulty levels, multiplayer).

### TODO: Map CPLC_select layer IDs to menu screens

The layer indices passed to `CPLC_select` correspond to `MenuId` enum values. Full mapping:

| Layer ID | Used in | Likely Menu |
|---|---|---|
| 0 | Main menu, various fallbacks | Main/Title |
| 1 | Multiple places | Unknown menu |
| 2 | [kknd.c](kknd.c#L58722) | NewCampaign |
| 4 | [kknd.c](kknd.c#L59437) | Unknown |
| 5 | [kknd.c](kknd.c#L59517) | Unknown |
| 6 | [kknd.c](kknd.c#L59598) + many | Multiplayer? |
| 7 | [kknd.c](kknd.c#L61186) | Unknown |
| 8 | [kknd.c](kknd.c#L61092) | Unknown |
| 9 | [kknd.c](kknd.c#L60993) | Unknown |
| 10 | [kknd.c](kknd.c#L61253) | Unknown |
| 11 | [kknd.c](kknd.c#L58866) | PlayMission |
| 12 | [kknd.c](kknd.c#L58403) | Credits |
| 14 | [kknd.c](kknd.c#L58951) | NewMissions |
| 15 | [kknd.c](kknd.c#L59036) | Unknown |

**TODO:** Cross-reference with MenuId enum to complete this table.

### TODO: Verify `g_outpost_spawn_params` / `g_clanhall_spawn_params` fields

These BSS globals are zero-initialized sentinels. When `unit_create` reads `owner_side` from them, it gets 0 — is that correct? Does player 0 always own mobile-base-planted buildings? Or should these globals be initialized with the planting unit's player_num?

### TODO: Understand the HP reduction on Mute_09 / Surv_21

[kknd.c](kknd.c#L7237): CPLC buildings with `player_num == 0` on two specific levels get HP reduced to 1/5. Is player 0 neutral? Or is this a specific scenario mechanic (pre-damaged neutral buildings the player must capture)?

### TODO: Investigate `LevelCplcSurface.size` interpretation

The `size` field stores byte count of entity data EXCLUDING the size field itself. Code does `size + 4` to get total. But the sorted list pointers (next_x/prev_x/next_y/prev_y) are also in the struct — does `size` include those 16 bytes? If yes, actual entity data = `size - 16` bytes. If no, entities start right after `prev_y_sorted`.

---

## Naming Convention Suggestions

### CPLC function naming pattern
All CPLC core functions should follow: `cplc_<noun>_<verb>` or `CPLC_<Verb>`:
- `cplc_entity_spawn` (not `cplc_entity_init`)
- `cplc_viewport_update` (not `cplc_406320`)
- `cplc_viewport_remove` (not `cplc_4062E0`)
- `cplc_restore_from_backup` (not `cplc_4060F0`)
- `CPLC_select` — already good
- `LVL_cplc_init` — already good (follows LVL_ prefix convention)

### Global naming convention
All CPLC globals already follow `g_cplc_*` or `g_current_lvl_cplc_*`. The unnamed `dword_*` globals should follow `g_cplc_spawn_*` / `g_cplc_despawn_*` pattern.

### The "two rectangles" globals should be grouped
Consider a struct:
```c
struct CplcViewportBounds {
  int spawn_left, spawn_right, spawn_top, spawn_bottom;
  int despawn_left, despawn_right, despawn_top, despawn_bottom;
};
```
But since these are guessed-type globals at fixed addresses, renaming individual dwords is more practical.
