# KKND Codebase Naming Conventions

## Stats

- **623** named functions (human-assigned names during RE)
- **793** unnamed `sub_XXXXXX` functions (IDA default)
- ~1416 total functions in kknd.c

---

## Existing Original Binary Prefixes (ALL_CAPS)

These come from debug strings, export names, or obvious API patterns in the original binary. They strongly suggest the original devs used `MODULE_FunctionName` convention:

| Prefix | Count | Examples | Likely Module |
|--------|-------|---------|---------------|
| `BOXD_` | 33 | `BOXD_terrain_cleanup`, `BOXD_collide_solid` | Collision/pathing (box grid) |
| `GAME_` | 21 | `GAME_IsLoading`, `GAME_ReadConfig` | Top-level game logic |
| `REND_` | 12 | `REND_DoFrame`, `REND_DrawList` | Renderer (DirectDraw) |
| `LVL_`  | 12 | `LVL_RunLevel`, `LVL_FindSection` | Level loading/management |
| `MOVIE_` | 10 | `MOVIE_Play`, `MOVIE_load` | VBC video playback |
| `FILE_` | 6 | `FILE_init`, `FILE_read` | File I/O abstraction |
| `FADE_` | 6 | `FADE_Start`, `FADE_Update` | Screen fade effects |
| `SOUND_` | 5 | `SOUND_play`, `SOUND_init` | Sound system (DirectSound) |
| `INPUT_` | 5 | `INPUT_ProcessKeyboard`, `INPUT_ProcessMouse` | Input handling |
| `MATH_` | 3 | `MATH_atan`, `MATH_isqrt` | Math utilities |
| `TASK_` | 8+ | `TASK_yield`, `TASK_send_message`, `TASK_Alloc` | Task scheduler kernel |
| `UNIT_` | 6 | `UNIT_Handler_Scout`, `UNIT_tasks_init` | Unit handler dispatch |
| `NETZ_` | 2 | `NETZ_Init`, `NETZ_ErrorString` | Network (DirectPlay) |
| `MOBD_` | 2 | `MOBD_Deinit` | MOBD sprite resources |
| `TIME_` | 2 | `TIME_init`, `TIME_tick` | Frame timing |
| `HUNK_` | 1 | `HUNK_FixPointers` | Memory hunk management |
| `CPLC_` | 1 | `CPLC_select` | Compiled placement (but most cplc_ are lowercase) |
| `OS_` | 1 | `OS_MessagePump` | Win32 OS layer |

**Pattern**: `MODULE_PascalCaseVerb` or `MODULE_snake_case_verb`. Mixed — original devs weren't 100% consistent either.

---

## Existing RE-Assigned Prefixes (lowercase)

These were assigned during reverse engineering. They follow `module_descriptive_name` snake_case:

| Prefix | Count | Examples | Domain |
|--------|-------|---------|--------|
| `unit_mode_` | 140+ | `unit_mode_idle`, `unit_mode_attack` | Unit FSM state functions |
| `unit_` | 87 | `unit_destroy`, `unit_on_damage` | Unit operations |
| `task_` | 32 | `task_game_update_loop`, `task_camera_shake` | Task entry points |
| `MessageHandler_` | 37 | `MessageHandler_Scout`, `MessageHandler_BuildingDefault` | Message dispatch |
| `sidebar_mode_` | 22 | `sidebar_mode_infantry_open` | Sidebar FSM states |
| `sidebar_` | 7 | `sidebar_cleanup`, `sidebar_init` | Sidebar infrastructure |
| `render_state_handler_` | 15 | `render_state_handler_infantry` | Render state update callbacks |
| `ai_` | 15 | `ai_task_409770`, `ai_update_base_area` | AI controllers |
| `entity_` | 11 | `entity_remove`, `entity_anim_advance` | Entity/sprite system |
| `cplc_` | 9 | `cplc_entity_init`, `cplc_init_all` | CPLC spawning |
| `cursor_` | 6 | `cursor_group_orders`, `cursor_load_mobd` | Cursor/selection |
| `proj_mode_` | 9 | `proj_mode_rocket`, `proj_mode_grenade` | Projectile FSM states |
| `turret_mode_` | 12 | `turret_mode_idle`, `turret_mode_attack_target` | Turret FSM states |
| `buildings_tracker_` | 6 | `buildings_tracker_init`, `buildings_tracker_increment` | Building limit tracking |
| `nuke_mode_` | 4 | `nuke_mode_init`, `nuke_mode_finalize` | Nuke ability FSM |
| `minimap_` | 4 | `minimap_init`, `minimap_open` | Minimap UI |
| `palette_` | 3 | `palette_408410_dim` | Palette manipulation |
| `upgrade_mode_` | 3 | `upgrade_mode_init` | Upgrade process FSM |
| `ui_string_` | 4 | `ui_string_free` | UI text display |
| `render_mode_` | 4 | `render_mode_sprt_draw` | Render mode setup/draw |
| `explosion_` / `explosions_` | 2 | `explosions_init`, `explosion_finished` | Explosion system |

---

## The Question: One Uniform Convention?

### Option A: Full `MODULE_PascalCase` (match original binary)

Convert everything to the original devs' style: `BOXD_CollideFloor`, `UNIT_ModeIdle`, `TASK_Yield`.

**Pros**:
- Matches the handful of original symbols we know
- Module prefix instantly tells you which subsystem
- grep-friendly — `^UNIT_Mode` finds all unit modes

**Cons**:
- **Huge scope** — 623 named + hundreds of `sub_` to rename
- PascalCase after prefix loses readability for multi-word names: `UNIT_ModeAttackMove` vs `unit_mode_attack_move`
- Current `unit_mode_xxx` naming is already very readable and searchable
- MessageHandler names would get verbose: `MSG_HandlerBuildingDefault`
- Mode functions (140+) would all become `UNIT_ModeXxx` — loses the visual distinction between modes, handlers, and utilities

### Option B: `MODULE_snake_case` (current RE style, systematized)

Keep snake_case, standardize the module prefixes:

```
BOXD_collide_floor          (was: BOXD_collide_floor — no change)
UNIT_mode_idle              (was: unit_mode_idle — capitalize prefix)
TASK_yield                  (was: TASK_yield — no change)
GAME_on_level_end           (new: sub_423320)
REND_do_frame               (was: REND_DoFrame — lowercase after prefix)
MSG_handler_scout           (was: MessageHandler_Scout)
UNIT_handler_scout          (was: UNIT_Handler_Scout)
```

**Pros**:
- Minimal churn — most functions only need prefix capitalized
- snake_case is highly readable for long names
- Module boundary is clear: caps prefix, underscore, lowercase rest
- Already the dominant pattern (500+ functions)

**Cons**:
- Diverges from original binary's PascalCase fragments
- `REND_do_frame` vs original `REND_DoFrame` — we'd be "correcting" the originals
- `MSG_handler_scout` is less punchy than `MessageHandler_Scout`

### Option C: Hybrid — Capitalize Prefix, Keep Rest As-Is

Capitalize only the module prefix. Don't touch the rest:

```
BOXD_collide_floor          (no change)
UNIT_mode_idle              (capitalize: unit → UNIT)
TASK_yield                  (no change)
GAME_OnLevelEnd             (new names: PascalCase after prefix)
REND_DoFrame                (no change — already correct)
MSG_Scout                   (was: MessageHandler_Scout)
UNIT_Handler_Scout          (capitalize prefix only)
```

**Pros**:
- Least disruptive — only capitalize existing prefixes
- New names follow `MODULE_PascalCase` like originals
- Existing snake_case stays untouched

**Cons**:
- Two styles coexist permanently: `UNIT_mode_idle` and `UNIT_Handler_Scout`
- "Hybrid" = no true consistency

### Option D: Keep Current Conventions, Standardize Prefixes Only

Don't change case at all. Just ensure every function has a proper prefix:

```
unit_mode_idle              (no change)
task_yield                  (lowercase TASK → task — matches majority)
game_on_level_end           (new)
rend_do_frame               (lowercase REND → rend)
msg_handler_scout           (was: MessageHandler_Scout)
```

**Pros**:
- Minimal diff — only rename orphan functions
- 100% snake_case consistency

**Cons**:
- Throws away original binary naming evidence
- `task_yield` vs `TASK_yield` — loses the "kernel API" feel
- `rend_do_frame` is less distinctive than `REND_DoFrame`

---

## Recommendation: Option B with Pragmatic Exceptions

**Option B** (`MODULE_snake_case`) gives best balance of readability, searchability, and fidelity to original intent.

### The Standard

```
PREFIX_descriptive_name
│       │
│       └── snake_case, all lowercase
└── ALL CAPS, 2-5 chars, identifies subsystem
```

### Module Prefix Table

| Prefix | Domain | Scope |
|--------|--------|-------|
| `GAME_` | Top-level game loop, config, save/load | ~30 functions |
| `LVL_` | Level loading, hunk management | ~15 functions |
| `TASK_` | Scheduler kernel, yield, messaging | ~20 functions |
| `UNIT_` | Unit lifecycle, handlers, modes, combat | ~250 functions |
| `UNIT_mode_` | Unit FSM state functions specifically | ~140 functions |
| `UNIT_handler_` | Unit task entry points | ~25 functions |
| `MSG_` | MessageHandler dispatch functions | ~37 functions |
| `BOXD_` | Collision grid, pathing, terrain | ~35 functions |
| `REND_` | Renderer, draw lists, DirectDraw | ~20 functions |
| `MAPD_` | Map display, render nodes, camera | ~10 functions |
| `MOBD_` | MOBD sprite resource management | ~5 functions |
| `CPLC_` | Compiled placement, level spawning | ~10 functions |
| `ENT_` | Entity system (create, remove, animate) | ~15 functions |
| `SIDE_` | Sidebar UI | ~30 functions |
| `PROD_` | Production/factory system | ~5 functions |
| `PROJ_` | Projectile modes and tasks | ~15 functions |
| `TURR_` | Turret modes and init | ~15 functions |
| `NUKE_` | Nuke/airstrike ability | ~5 functions |
| `AI_` | AI controllers, decision making | ~20 functions |
| `MOVIE_` | VBC video playback | ~10 functions |
| `SOUND_` | Sound system | ~5 functions |
| `INPUT_` | Keyboard/mouse | ~5 functions |
| `FADE_` | Screen transitions | ~6 functions |
| `FILE_` | File I/O | ~6 functions |
| `MATH_` | Math utilities | ~5 functions |
| `TIME_` | Frame timing | ~2 functions |
| `NETZ_` | Network/DirectPlay | ~10 functions |
| `OS_` | Win32 layer | ~3 functions |
| `PAL_` | Palette manipulation | ~5 functions |
| `UI_` | UI strings, messages | ~10 functions |
| `EXPL_` | Explosions, bloodsplats | ~10 functions |
| `MINI_` | Minimap | ~5 functions |
| `CURSOR_` | Cursor, selection rectangle | ~10 functions |

### Pragmatic Exceptions

1. **Keep `TASK_` uppercase API feel** for the scheduler kernel:
   `TASK_yield`, `TASK_send_message`, `TASK_Alloc`, `TASK_Terminate` — these are *the* kernel API. The uppercase after prefix (`TASK_Alloc` not `TASK_alloc`) matches original binary evidence and signals "system call."

2. **Mode functions keep `_mode_` infix** for instant recognition:
   `UNIT_mode_idle`, `TURR_mode_attack`, `PROJ_mode_rocket`, `SIDE_mode_infantry_open`

3. **Handler functions keep `_handler_` infix**:
   `UNIT_handler_scout`, `UNIT_handler_outpost`

4. **MessageHandlers shorten to `MSG_`**:
   `MSG_scout`, `MSG_building_default`, `MSG_ai_general`
   (37 functions — `MessageHandler_` is 15 chars of prefix, too verbose)

5. **Render state handlers → `REND_state_`**:
   `REND_state_infantry`, `REND_state_vehicles`

### Example Renames

| Current | Proposed |
|---------|----------|
| `sub_423320` | `GAME_on_level_end` |
| `sub_44C5C0` | `GAME_cleanup_units_and_systems` |
| `sub_43A320` | `SOUND_cleanup_all` |
| `sub_446E90` | `PROD_cleanup` |
| `sub_44C1D0` | `PAL_cleanup_player_tints` |
| `sub_428080` | `GAME_free_buffers` |
| `MessageHandler_Scout` | `MSG_scout` |
| `MessageHandler_BuildingDefault` | `MSG_building_default` |
| `UNIT_Handler_Scout` | `UNIT_handler_scout` |
| `unit_mode_idle` | `UNIT_mode_idle` |
| `render_state_handler_infantry` | `REND_state_infantry` |
| `sidebar_mode_infantry_open` | `SIDE_mode_infantry_open` |
| `EventHandler_Passive` | `MSG_passive` |
| `buildings_tracker_init` | `GAME_buildings_tracker_init` |
| `entity_remove` | `ENT_remove` |
| `entity_anim_advance` | `ENT_anim_advance` |
| `cursor_group_orders` | `CURSOR_group_orders` |
| `task_game_update_loop` | `TASK_game_update_loop` |
| `task_camera_shake` | `GAME_camera_shake_task` |
| `proj_mode_rocket` | `PROJ_mode_rocket` |
| `turret_mode_idle` | `TURR_mode_idle` |
| `cplc_entity_init` | `CPLC_entity_init` |
| `explosions_init` | `EXPL_init` |

---

## Globals Naming Convention

Globals already use `g_` prefix inconsistently. Proposed:

```
g_MODULE_descriptive_name
│ │       │
│ │       └── snake_case
│ └── lowercase module tag (optional for obvious ones)
└── always starts with g_
```

Examples:
- `g_task_active_head` → keep (already good)
- `dword_47A18C` → `g_game_play_campaign_outro`
- `dword_47A778` → `g_netz_disconnect_flag`
- `node[3]` → `g_mapd_menu_background_nodes`
- `g_game_state` → keep (already good)

---

## Enums & Structs

Already follow `KKND::PascalCase` from IDA namespace. Keep that. Fill in missing values:

```c
enum KKND::GameState {
    GameState_MainMenu = 0,
    GameState_LevelComplete = 1,    // MISSING — add
    GameState_Victory = 2,          // MISSING — add
    GameState_PreviousScreen = 3,
};
```

---

## Priority Order for Renaming

1. **`sub_` functions** — 793 unnamed. Highest impact. As each is investigated, assign `MODULE_snake_case` name.
2. **Lowercase prefixes → CAPS** — `unit_mode_X` → `UNIT_mode_X`, `task_X` → `TASK_X`, etc. Mechanical, high volume (~400).
3. **`MessageHandler_X` → `MSG_X`** — 37 functions, saves 12 chars each.
4. **Globals** — `dword_XXXXX` → `g_module_name`. As investigated.
5. **Fill enum gaps** — GameState, TaskYieldFlags, etc.
