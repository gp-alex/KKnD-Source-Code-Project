# sub_423320 — Level Transition & Campaign Progression

**Rename**: `GAME_OnLevelEnd` or `GAME_ProcessLevelTransition`

**Location**: [kknd.c:35804](kknd.c#L35804)

**Signature**: `BOOL sub_423320()`

**Returns**: `TRUE` = continue to next level, `FALSE` = campaign ended (play outro movie, return to menu)

---

## Purpose

Called once after each mission's game loop exits in `KKND_Main`. Performs full level teardown (rendering, units, terrain, sound, network) then decides whether the campaign should advance to the next level or end.

### Call Site in KKND_Main (line ~36040)

```c
if ( !sub_423320() )         // "should campaign continue?"
{
    dword_47CB0C = 0;
    MOVIE_Play(2);           // play campaign outro movie
    if ( !g_os_quit_signal_received )
        goto LABEL_5;        // restart from main menu
    break;
}
// else: KKND_Main's while(1) loop continues → sub_422F60() loads next level
```

---

## Execution Flow

### Phase 1 — Render Cleanup

```c
// Remove all menu background render nodes (g_menu_background_nodes[3])
for each node in g_menu_background_nodes[0..2]:
    MAPD_RemoveRenderable(node) → unlink from render list, add to free pool
```

**Decompilation issue**: `node` is a terrible name for a file-scope global. It's `MapdRenderNode*[3]` at address 0x47A010.

**Rename**: `node` → `g_menu_background_nodes`
**Rename**: `dword_47A01C` → `g_menu_background_nodes_end` (sentinel — address of next global after the array)

### Phase 2 — Game Systems Cleanup

| Call | Line | Rename Suggestion | What It Frees |
|------|------|-------------------|---------------|
| `mode_null(v1)` | — | (keep) | No-op — null mode handler, does nothing. Decompiler artifact: IDA thinks `v1` is passed but `mode_null` ignores its argument |
| `sub_428080()` | 40544 | `GAME_FreeAllocatedBuffers` | Frees `dword_47A52C` and `dword_47A4F4` (two unknown buffers) |
| `sub_44C5C0()` | 72750 | `GAME_CleanupUnitsAndSystems` | **Big one.** Cleans up: selection system, minimap, oil patches, buildings tracker, AI, escort pool. Then walks `g_unit_list_head`, decrements unit counters, invalidates render nodes, moves all units to free pool. Frees `g_unit_pool`. Resets `g_escort_initialized` |
| `aircraft_list_free()` | 5752 | (keep) | Frees `g_aircraft_pool` |
| `sub_446E90()` | 67888 | `production_system_cleanup` | Removes production UI entity, frees `g_prod_pool` + `g_prod_options_pool`, nulls `g_sidebar_cash_string` |
| `sub_44C1D0()` | 72515 | `player_palettes_cleanup` | Frees per-player tint palettes from `g_tint_palettes_per_player` array |
| `BOXD_terrain_cleanup()` | 18001 | (keep) | Frees `g_terrain`, nulls pointer |
| `sidebar_cleanup()` | 19552 | (keep) | Frees `g_sidebar_button_pool` + `g_sidebar_pool` |

### Phase 3 — Level Data Cleanup (Branched)

**Branch A: Victory on main campaign level** (`g_game_state == 2` AND level outside Xtreme range)
```c
g_prev_level_id = g_current_lvl_id;    // remember for progression
cplc_4060F0();                          // restore CPLC backup (for replay/level select?)
LVL_cleanup();
sub_43A320();                           // sound_system_cleanup
```

**Branch B: Everything else** (defeat, Xtreme levels, load game)
```c
LVL_cleanup();
sub_43A320();                           // sound_system_cleanup
kknd_mfree(g_level);                    // free level hunk
kknd_mfree(g_supspr_lvl);              // free super-sprite level data
g_supspr_lvl = nullptr;
g_prev_level_id = LevelId_Invalid;     // no previous level
```

**Key difference**: Victory preserves `g_prev_level_id` and restores CPLC backup. Everything else does a full teardown.

**Rename**: `cplc_4060F0` → `cplc_restore_backup`

### Phase 4 — Network Cleanup (Multiplayer Only)

```c
if ( !g_is_single_player && !dword_47A778 )
{
    sub_42FB50();     // NETZ function (external stub, likely DirectPlay cleanup)
    sub_42F650();     // NETZ function (external stub)
    sub_42F8E0(0);    // NETZ_ToggleHostState — resets host flag if needed
}
```

**Rename**: `dword_47A778` → `g_netz_disconnect_flag` (set to 1 on disconnect/error, prevents cleanup of already-dead session)

**Rename suggestions**:
- `sub_42FB50` → `NETZ_CleanupSession` (extern, body not in kknd.c)
- `sub_42F650` → `NETZ_CleanupProvider` (extern, body not in kknd.c)
- `sub_42F8E0` → `NETZ_ToggleHostState`

### Phase 5 — Campaign Progression Decision

```c
if ( g_game_state != 1 )
    return g_game_state == 2;     // victory → TRUE, anything else → FALSE

// g_game_state == 1: "continue playing" state
if ( GAME_IsLoading() )
{
    // Loading saved game → override level, continue
    g_prev_level_id = LevelId_Invalid;
    g_current_lvl_id = GAME_GetSaveLoadLevelId();
    GAME_SetPlayerSideFromLevel();
    return 1;  // continue
}

// Normal campaign progression
if ( NOT in Xtreme range AND NOT final campaign level )
{
    ++g_current_lvl_id;                // advance to next level
    GAME_SetPlayerSideFromLevel();     // set player_num from level
    return 1;  // continue
}

// At final campaign level (Surv_15_Beseiged or Mute_15_CounterAttack)
dword_47A18C = 1;    // trigger campaign outro movie
return 0;            // campaign ended
```

---

## GameState Values (Incomplete Enum)

The decompiler only has 2 named values. From code analysis:

| Value | Name (suggested) | Meaning | Set By |
|-------|-----------------|---------|--------|
| 0 | `GameState_MainMenu` | Menu loop running | KKND_Main before each menu loop |
| 1 | `GameState_LevelComplete` | Level ended, progression pending | Mission outcome task on defeat, or when at final levels |
| 2 | `GameState_Victory` | Level won, advance campaign | Mission outcome task when `MissionOutcome == Victory` |
| 3 | `GameState_PreviousScreen` | Return to previous screen | Demo build end, Xtreme campaign levels |

**Rename**: `dword_47A18C` → `g_play_campaign_outro`

When `g_play_campaign_outro == 1`, `MOVIE_Play(2)` plays:
- `survout.vbc` for Survivor campaign ending (level 15 = Beseiged)
- `evolvout.vbc` for Mutant campaign ending (level 15 = CounterAttack)

---

## Level Range Summary

| Range | IDs | Campaign | Progression |
|-------|-----|----------|-------------|
| 0–14 | Surv_01..Surv_15_Beseiged | Survivor main | Sequential, ends at 14 |
| 15–29 | Mute_01..Mute_15_CounterAttack | Mutant main | Sequential, ends at 29 |
| 48–57 | Surv_16..Surv_25 | Survivor Xtreme | Uses `GameState_PreviousScreen` |
| 58–67 | Mute_16..Mute_25 | Mutant Xtreme | Uses `GameState_PreviousScreen` |

Levels 48–67 (Xtreme) get `GameState_PreviousScreen` on win → sub_423320 returns FALSE → plays outro movie → back to menu. They don't chain-advance.

---

## Global Variables Referenced

| Address | Current Name | Rename Suggestion | Type | Purpose |
|---------|-------------|-------------------|------|---------|
| 47A010 | `node[3]` | `g_menu_background_nodes` | `MapdRenderNode*[3]` | Menu screen background sprites |
| 47A01C | `dword_47A01C` | `g_menu_background_nodes_end` | sentinel | End of node array (loop bound) |
| 47A18C | `dword_47A18C` | `g_play_campaign_outro` | `BOOL` | Flag to play campaign ending movie |
| 47A778 | `dword_47A778` | `g_netz_disconnect_flag` | `BOOL` | Network disconnect/error flag |
| 47A52C | `dword_47A52C` | (unknown) | `void*` | Freed in sub_428080 |
| 47A4F4 | `dword_47A4F4` | (unknown) | `void*` | Freed in sub_428080 |
| 47CB0C | `dword_47CB0C` | (unknown) | `int` | Set to 0 by caller on campaign end |

---

## Cleanup Function Renames (Consolidated)

| Current | Address | Suggested Rename |
|---------|---------|-----------------|
| `sub_423320` | 423320 | `GAME_OnLevelEnd` |
| `sub_428080` | 428080 | `GAME_FreeAllocatedBuffers` |
| `sub_44C5C0` | 44C5C0 | `GAME_CleanupUnitsAndSystems` |
| `sub_446E90` | 446E90 | `production_system_cleanup` |
| `sub_44C1D0` | 44C1D0 | `player_palettes_cleanup` |
| `sub_43A320` | 43A320 | `sound_system_cleanup` |
| `cplc_4060F0` | 4060F0 | `cplc_restore_backup` |
| `sub_42FB50` | 42FB50 | `NETZ_CleanupSession` |
| `sub_42F650` | 42F650 | `NETZ_CleanupProvider` |
| `sub_42F8E0` | 42F8E0 | `NETZ_ToggleHostState` |

---

## Decompilation Issues

1. **`mode_null(v1)`** — IDA generated a phantom argument. `mode_null` is a no-op void function. The `v1` variable is marked "possibly undefined" at line 35804 comment. This entire call is dead code — likely a decompiler artifact from register reuse.

2. **`node` global name** — IDA picked `node` for a file-scope global at 0x47A010. Terrible name. Should be `g_menu_background_nodes`.

3. **`dword_47A01C` as loop sentinel** — Not actually a separate variable. It's the address immediately after `node[3]`, used as `(int)v0 < (int)&dword_47A01C` to iterate the array. Classic C `array + count` pattern that IDA decompiled as separate global.

4. **GameState enum incomplete** — Values 1 and 2 are missing from the enum definition. Should be `GameState_LevelComplete = 1` and `GameState_Victory = 2`.

5. **`sub_42FB50` / `sub_42F650` missing bodies** — These are likely DirectPlay COM interface calls that IDA couldn't decompile. Only forward declarations exist.

---

## Key Insight

`sub_423320` is the **single decision point** between "play next level" and "campaign is over". The KKND_Main outer `while(1)` loop repeatedly does: load level → run game loop → call sub_423320 → if TRUE, loop again. If FALSE, play outro movie and return to menu.

The campaign progression is purely sequential: `++g_current_lvl_id`. No branching paths. Xtreme levels don't chain — they return to menu after each.
