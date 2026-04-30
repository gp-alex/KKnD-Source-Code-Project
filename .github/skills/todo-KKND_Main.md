# KKND_Main — Master Game Loop & State Machine

**Line**: 35884 in kknd.c
**Signature**: `int KKND_Main()`
**Called from**: `WinMain()` at line 73718 — this IS the game entry point after OS/DirectX validation.

## Rename Suggestion

**`KKND_Main` → `GAME_MasterLoop`** or **`GAME_Run`**

This is the top-level game orchestrator. It drives the entire application lifecycle: init → intro movies → menu → level load → gameplay → mission end → next level / retry / quit. It's not "main" in the C sense — it's the game-specific master loop that `WinMain` delegates to.

---

## High-Level Architecture

```
KKND_Main()
 │
 ├── timeBeginPeriod(1)                    // Request 1ms timer resolution
 ├── GAME_WaitScreen()                     // Init systems + animated loading screen
 ├── MOVIE_Play(0)                         // Play intro FMVs (mh_fmv.vbc + intro.vbc)
 │
 ├──► LABEL_5: MAIN MENU LOOP             // <── Returns here after ending movie
 │   ├── dword_4785C8(1)                   // Set display mode (function ptr)
 │   ├── Determine initial menu ID:
 │   │   ├── Single player → MenuId_Main
 │   │   ├── Multiplayer reconnect (dword_47A778) → Host/Join
 │   │   └── Multiplayer fresh → IpxSetup or Multiplayer
 │   ├── GAME_PrepareSuperLvl(menuId)      // Load super.lvl for menu UI
 │   ├── g_game_state = GameState_MainMenu
 │   └── ═══ MENU PUMP LOOP ═══           // Standard game loop
 │       (entities, input, sound, tasks, camera, movie, render, time)
 │       until: quit OR state != MainMenu
 │
 ├── Cleanup menu resources (MAPD nodes, LVL, level memory)
 ├── Handle save/load: restore level ID if loading
 │
 ├── if state == PreviousScreen → EXIT
 │
 ├──► MISSION CYCLE LOOP (while(1)):
 │   │
 │   ├── dword_4785C8(1)                   // Set display mode
 │   │
 │   ├── if NOT loading:
 │   │   ├── Normal levels (not 16-25 extreme):
 │   │   │   └── MOVIE_Play(1)             // Mission briefing FMV
 │   │   └── Extreme levels (16-25):
 │   │       ├── GAME_PrepareSuperLvl(MenuId_MissionComplete)
 │   │       └── ═══ MISSION COMPLETE PUMP ═══  // Same loop pattern
 │   │           until: quit OR state != MainMenu
 │   │           + cleanup nodes/levels
 │   │
 │   ├── sub_422F60()                      // LOAD LEVEL (sprites, terrain, units, game systems)
 │   ├── g_game_state = GameState_MainMenu
 │   │
 │   ├── ═══ GAMEPLAY PUMP LOOP ═══        // THE ACTUAL GAME
 │   │   ├── Network sync: g_netz_synchronized_lockstep_mode = dword_47A738
 │   │   ├── GAME_AdvanceEntityAnimations()
 │   │   ├── GAME_AdvanceEntityPositions()
 │   │   ├── INPUT_ProcessKeyboard()
 │   │   ├── INPUT_ProcessMouse()
 │   │   ├── SOUND_ProcessSounds()
 │   │   ├── FADE_Update()
 │   │   ├── sub_40EA20()                  // Increment opportunity timer
 │   │   ├── sub_44C4B0()                  // Update unit anchors & turret positions
 │   │   ├── TASK_ExecuteScheduledTasks()
 │   │   ├── GAME_UpdateCamera()
 │   │   ├── REND_DoFrame()
 │   │   ├── TIME_tick()
 │   │   ├── if sub_41CAD0():              // Save/load triggered?
 │   │   │   ├── sub_43A800(0)             // Mute music
 │   │   │   ├── sub_4211D0()              // Attempt save game
 │   │   │   │   └── if fail: sub_421D40() // Show save error message task
 │   │   │   └── sub_43A800(dword_4690AC)  // Restore music volume
 │   │   until: quit OR state != MainMenu
 │   │
 │   ├── sub_423320()                      // POST-MISSION: cleanup + determine next action
 │   │   Returns: 1 = continue to next level, 0 = campaign complete
 │   │
 │   ├── if returns 0 (campaign done):
 │   │   ├── dword_47CB0C = 0
 │   │   ├── MOVIE_Play(2)                 // Ending movie (survout/evolvout.vbc)
 │   │   └── goto LABEL_5 (back to menu)
 │   │
 │   └── if returns 1: loop continues → load next level
 │
 ├── SHUTDOWN:
 │   ├── sub_42F9A0()                      // Network cleanup
 │   ├── dword_468B60 = 1
 │   ├── kknd_mfree(g_wait_lvl)            // Free loading screen
 │   ├── sub_43A320()                      // Sound system cleanup
 │   ├── LVL_SysCleanup()                  // Level system cleanup
 │   └── timeEndPeriod(1)
 │       return 0 (success) or 1 (early failure)
```

---

## Game State Machine

`g_game_state` is an `enum GameState` (kknd.h line 2216). Enum is incomplete — only 2 named values found:

| Value | Named | Meaning | Evidence |
|-------|-------|---------|----------|
| 0 | `GameState_MainMenu` | "Keep running current loop" | All pump loops check `== MainMenu` |
| 1 | *(missing)* | **Victory / Advance** | Set on mission win, `sub_423320` treats as "go to next level" |
| 2 | *(missing)* | **Defeat / Replay** | Set on mission loss, `sub_423320` returns `g_game_state == 2` for "continue but replay" |
| 3 | `GameState_PreviousScreen` | **Exit to previous screen / Quit** | Breaks all loops, leads to shutdown |

### Suggested Enum Rename
```c
enum GameState {
    GameState_Running = 0,           // was GameState_MainMenu — misleading name
    GameState_MissionVictory = 1,    // advance to next level
    GameState_MissionDefeat = 2,     // replay current level
    GameState_ExitToMenu = 3,        // was GameState_PreviousScreen
};
```

**Decompilation issue**: `GameState_MainMenu` is a bad name. It means "keep running" — used in ALL loops (menu, gameplay, briefing). It's really a "running" / "no-transition-requested" state.

---

## Three Distinct Pump Loops

KKND_Main has **3 nearly-identical event loops**, differentiated by what runs inside:

### Loop 1: Menu Loop (line 35919)
```
entities + input + sound + fade + tasks + camera + movie + render + time
```
Used for: Main menu, mission complete screen. Has MOVIE_do_frame() for background animations.

### Loop 2: Gameplay Loop (line 36014)
```
network_sync + entities + input + sound + fade +
sub_40EA20 + sub_44C4B0 + tasks + camera + render + time + save_check
```
Used for: Actual gameplay. Adds:
- Network lockstep sync toggle
- `sub_40EA20` — opportunity timer tick (units scan for targets every 64 frames)
- `sub_44C4B0` — unit animation anchor + turret position update
- Save/load check with `sub_41CAD0`

### Loop 3: Mission Complete Menu (line 35971)
Same as Loop 1. Runs `MenuId_MissionComplete` super level for extreme (bonus) missions.

---

## Function Reference Table

| Function | Line | Rename Suggestion | Purpose |
|----------|------|-------------------|---------|
| `KKND_Main` | 35884 | `GAME_Run` | Master game loop + state machine |
| `GAME_WaitScreen` | 35249 | keep | Init all systems + show 180-frame loading animation |
| `GAME_PrepareSuperLvl` | 35344 | keep | Load super.lvl + supspr.lvl for menu UI rendering |
| `sub_422F60` | 35657 | `GAME_LoadLevel` | Load level files, init all game systems (terrain, units, sidebar, etc.) |
| `sub_423320` | 35802 | `GAME_PostMission` | Cleanup mission + determine next action (advance/replay/end) |
| `sub_40EA20` | 17994 | `GAME_TickOpportunityTimer` | Increment `g_opportunity_timer` (unit target scan scheduling) |
| `sub_44C4B0` | 72688 | `GAME_UpdateUnitAnchors` | Update MOBD anchor points + turret positioning |
| `sub_41CAD0` | 29777 | `GAME_IsSaveRequested` | Check if save operation pending (`dword_479FC0`) |
| `sub_4211D0` | 34249 | `GAME_SaveToFile` | Full game save: camera, units, AI, map, oil → disk |
| `sub_421D40` | 34778 | `GAME_ShowSaveError` | Spawn fiber task showing save failure message for 180 ticks |
| `sub_43A800` | 55871 | `SOUND_SetMusicVolume` | Set DirectSound volume for music channel |
| `sub_42F9A0` | 47018 | `NETZ_Cleanup` | Reset network session, call cleanup twice, set `dword_468B68 = -1` |
| `sub_43A320` | 55561 | `SOUND_Cleanup` | Free all active sounds, reset sound system globals |
| `GAME_IsLoading` | 29771 | keep | `return g_is_game_loading` |
| `GAME_GetSaveLoadLevelId` | 29808 | keep | `return g_saveload_level_id` |
| `GAME_SetPlayerSideFromLevel` | 36486 | keep | Set `g_player_num` (1=surv, 2=mute) from current level range |

---

## Global Variable Reference

| Variable | Type | Rename Suggestion | Purpose |
|----------|------|-------------------|---------|
| `g_game_state` | `GameState` | keep | Master state machine driver |
| `g_os_quit_signal_received` | `BOOL` | keep | OS quit signal (WM_QUIT) |
| `g_mission_outcome` | `MissionOutcome` | keep | Mission result enum |
| `g_is_single_player` | `BOOL` | keep | SP vs MP mode flag (default 1) |
| `dword_4785C8` | `fn ptr` | `g_set_display_mode` | Graphics mode setter, assigned `sub_434790` |
| `dword_47A778` | `int` | `g_netz_mission_completed` | Set to 1 on multiplayer victory, controls reconnect flow |
| `dword_468B68` | `int` | `g_netz_session_id` | Network session ID (-1 = no session) |
| `dword_468B60` | `int` | `g_systems_shutting_down` | 1 during init/shutdown, 0 during normal operation |
| `dword_47A738` | `int` | `g_netz_lockstep_requested` | Drives `g_netz_synchronized_lockstep_mode` |
| `dword_47CB0C` | `int` | `g_campaign_ending_played` | Reset to 0 before ending movie |
| `dword_4690AC` | `int=16` | `g_music_volume` | Default music volume level (16) |
| `dword_47A01C` | `int` | `g_mapd_node_count` | Loop terminator for node[] cleanup (probably 3 but could vary) |
| `dword_479FC0` | `int` | `g_save_requested` | Checked by `sub_41CAD0`, triggers save flow |
| `dword_47A18C` | `int` | `g_campaign_finale_pending` | Set 1 on final mission victory, triggers ending movie |
| `node[3]` | `MapdRenderNode*[]` | `g_menu_mapd_nodes` | MAPD render nodes for menu/UI background |
| `g_level` | `LevelHunk*` | keep | Current loaded level data |
| `g_wait_lvl` | `LevelHunk*` | keep | Loading screen level data |
| `g_supspr_lvl` | `LevelHunk*` | keep | Super sprite level (shared across menus) |

---

## Level Flow & Campaign Progression

### sub_423320 (GAME_PostMission) Decision Tree
```
1. Cleanup: remove MAPD nodes, free unit lists, terrain, sidebar, sounds
2. If g_game_state == 2 (defeat) AND not extreme levels:
   → Keep level loaded (g_prev_level_id = current), return 1 (replay)
3. Else: full cleanup, free level + sprites
4. If multiplayer: network disconnect if not victory-reconnect
5. If g_game_state == 1 (victory):
   a. If loading save: restore saved level, return 1
   b. If not final mission: increment g_current_lvl_id, return 1
   c. If IS final mission (Surv_15 or Mute_15): set dword_47A18C = 1, return 0
6. If g_game_state == 2 (defeat): return 1 (try again)
7. Otherwise: return 0 (exit)
```

### Campaign Structure
- **Survivors**: levels 0-14, extreme 48-57 → `g_player_num = 1`
- **Mutants**: levels 15-29, extreme 58-67 → `g_player_num = 2`
- **Extreme/Bonus**: levels 16-25 (special range, skip briefing FMV, show MissionComplete menu)
- Final missions: `LevelId_Surv_15_Beseiged` and `LevelId_Mute_15_CounterAttack`

---

## Decompilation Issues Found

1. **`GameState_MainMenu` misnomer**: Value 0 means "keep running" not "main menu". All pump loops (gameplay included) check this value. Should be `GameState_Running` or `GameState_Active`.

2. **Missing enum values**: `g_game_state = 1` and `= 2` are raw integers. Should be:
   - `1` = `GameState_MissionVictory` (triggers level advance in `sub_423320`)
   - `2` = `GameState_MissionDefeat` (triggers replay in `sub_423320`)

3. **`GameState_PreviousScreen` misnomer**: Value 3 means "exit current flow" — could be back to menu from gameplay, or quit from menu. Better: `GameState_ExitToMenu` or `GameState_Break`.

4. **3 nearly-identical pump loops**: Classic decompiler artifact — original code likely had a shared `GAME_RunLoop(flags)` function that got inlined, or a macro. The gameplay loop adds 4 extra calls vs menu loop.

5. **`node[3]` + `dword_47A01C` loop pattern**: Cleanup iterates `node` array using `(int)v < (int)&dword_47A01C` as sentinel — `dword_47A01C` is the int RIGHT AFTER `node[3]` in memory. This is the classic "address of next global as array bound" pattern (seen before in UI system). `dword_47A01C` is NOT a count — it's just the memory address that happens to follow `node[2]`.

6. **`dword_4785C8(1)` mystery**: Function pointer called with 1 before every loop. Assigned to `sub_434790` (line 17510). Likely a display/page-flip mode setter. Name suggestion: `g_fn_set_display_mode`.

7. **Early exit path**: If `MOVIE_Play(0)` fails (returns 0), KKND_Main takes a different cleanup path that skips `sub_42F9A0()` (network cleanup). This is probably fine — network hasn't been fully initialized yet at that point.

---

## Lifecycle Summary

```
WinMain → KKND_Main:
  1. INIT: timer, systems, loading screen, intro movies
  2. MENU: load super.lvl, run menu pump until user picks level
  3. BRIEFING: play mission FMV (or show MissionComplete for extreme)
  4. LOAD: load level files + init all game systems
  5. PLAY: run gameplay pump until victory/defeat/quit
  6. POST-MISSION: cleanup, decide next action
     → Victory (not final): advance level → goto 3
     → Victory (final): play ending movie → goto 2
     → Defeat: replay level → goto 3
     → Quit: break
  7. SHUTDOWN: network, sound, level system, timer
```

This is the heartbeat of the entire game.
