# KKND UI System — Sidebar, Input, Production

## Overview

UI runs inside `task_game_update_loop` (line 41014), which owns the `CursorState` on stack and processes input every frame. Sidebar is a vertical button strip on right edge. Production flows: sidebar click → `enqueue_infantry_ex` → per-frame tick → `TaskMessage_UnitReady` → unit spawn. All events go through `GameEvent` queue for network sync.

## Architecture Layers

```
INPUT_ProcessKeyboard/Mouse → g_game_keyboard/mouse_state
       ↓
task_game_update_loop → game_process_events(cursor, 1)
       ↓                    ↓
  cursor_group_orders   Camera scroll / UI hover / Event queue
       ↓
  GameEvent enqueued → network sync → event dispatch handler (line 69620)
       ↓
  TaskMessage sent to unit/factory/etc
```

---

## CursorState (0x7C bytes, on stack)

```
Offset  Type                     Field                           Suggested Name
------  ----                     -----                           --------------
0x00    CursorUnitSelection*     selection_head                  → selected units list
0x04    CursorUnitSelection*     selection_tail
0x08    Task*                    _cursor_state_task_8            → hovered unit task (hover target)
0x0C    CursorUnitSelection*     selection_pool                  → pool base (100 entries)
0x10    CursorUnitSelection*     selection_free_pool             → free list
0x14    Task*                    cursor_task                     → g_game_update_loop_task
0x18    Task*                    hovered_ui_task                 → sidebar button being hovered
0x1C    Task*                    hovered_unit_task               → game unit under cursor
0x20    int                      cursor_mobd_offset              → current cursor animation frame
0x24    BOOL                     is_help_mode                    → "?" help cursor active
0x28    BOOL                     is_airstrike_targeting          → airstrike targeting mode
0x2C    BOOL                     is_placing_building             → building placement mode
0x30    BOOL                     is_selling_building             → sell building mode
0x34    BOOL                     is_cursor_over_impassable_terrain
0x38    BOOL                     are_own_units_selected
0x3C    BOOL                     is_single_unit_selected
0x40    BOOL                     are_movable_units_selected
0x44    BOOL                     are_attacker_units_selected
0x48    UnitType                 unit_type_to_voice_response     → highest-priority type for audio
0x4C    BOOL                     is_rmb_scrolling                → right-click scroll mode
0x50    int                      rmb_scrolling_initial_x
0x54    int                      rmb_scrolling_initial_y
0x58    int                      rmb_scrolling_max_dx
0x5C    int                      rmb_scrolling_max_dy
0x60    int                      rmb_scrolling_dx
0x64    int                      rmb_scrolling_dy
0x68    Unit*                    selection_executing_representative → primary selected unit
0x6C    UnitCommandArchetype     selection_executing_archetype   → command capability class
0x70    Entity*                  cursor_hitbox_tester            → world-space collision probe
0x74    Entity*                  cursor_entity                   → screen-space cursor sprite
0x78    BuildingPlannerPayload*  planner                         → active building placement
```

### CursorUnitSelection (0x0C bytes, pool of 100)
```
0x00  next/prev  — linked list in cursor selection
0x08  unit_task  — Task* of selected unit
```

---

## Input Processing

### INPUT_ProcessKeyboard (line 27958)
Polls `GetAsyncKeyState()` for all tracked keys. Computes held/new/released masks. Maps virtual keys to `KeyboardActions` flags. Sets `g_game_keyboard_state`.

### INPUT_ProcessMouse (line 40804)
Uses `GetCursorPos()` + `ScreenToClient()`. Computes deltas, button transitions. Fixed-point coords (<<8). Sets `g_game_mouse_state`.

### Global State
```c
KeyboardState g_input_last_keyboard_state;  // line 5375 — prev frame
KeyboardState g_game_keyboard_state;        // line 5449 — current frame
MouseState g_input_last_mouse_state;        // line 5430 — prev frame
MouseState g_game_mouse_state;              // line 5440 — current frame
CursorState *g_cursor;                      // line 5445 — points to stack var in game loop
int g_key_modifier;                         // Scancode_None / Scancode_Ctrl / Scancode_Alt
```

---

## Main Game Loop: task_game_update_loop (line 41014)

Init:
- Allocates 8 `GameEventNode` pool (0xA0 bytes)
- Allocates 100 `CursorUnitSelection` pool (0x4B0 bytes)
- Creates cursor entities: `cursor_hitbox_tester` (world-space, z=1) + `cursor_entity` (screen-space, z=1001, initially hidden)
- Sets `g_game_update_loop_task = task`, `g_cursor = &cursor`

Main loop (every frame):
```
1. game_process_events(&cursor, 1)     ← camera, input, event queue
2. Check UI hover transitions          ← deselect hint on mouse-off
3. Check keyboard shortcuts:
   - Ctrl+P/Q/T/2 → debug/netz commands
   - Ctrl+0-9 → assign control group (GameEvent_AssignedControlGroup)
   - 2-11 → recall control group (cursor_select_control_group)
4. Dispatch by cursor state:
   - hovered_ui_task → sub_42B600 (sidebar button interaction)
   - is_placing_building → building placement logic
   - is_airstrike_targeting → airstrike targeting
   - is_selling_building → sell building
   - is_help_mode → help cursor
   - else → cursor_group_orders (normal game interaction)
5. Update cursor sprite based on context
```

---

## game_process_events (line 43259)

Big function. Handles:

### Camera Control
- **Edge scroll**: mouse near screen edges moves camera
- **RMB scroll**: right-click drag panning
- **Keyboard scroll**: arrow keys (with optional speed boost)
- **Camera bounds**: constrained to map dimensions
- **Camera shake**: applies `g_camera_shake_x/y` offsets

### Cursor World Position
Updates `cursor_hitbox_tester` entity to world coordinates:
```c
hitbox.x = camera.x + mouse.x
hitbox.y = camera.y + mouse.y
```
Checks terrain passability → sets `is_cursor_over_impassable_terrain`.

### Event Queue Processing
Dequeues `GameEventNode` from `g_game_event_queue`, dispatches to handler.

---

## cursor_group_orders (line 42159)

Main order-processing function. Checks mouse clicks + unit interaction.

### Control Group Assignment
```
Ctrl + 2-0 → assign current selection to group (GameEvent_AssignedControlGroup)
```

### Order Dispatch by Archetype

| Mouse Action | Target | Archetype | Event/Message | Cursor Frame |
|---|---|---|---|---|
| LMB on empty | — | Any movable | `GameEvent_MoveCommand` | 384 (green arrow) |
| LMB on enemy | — | Any attacker | `GameEvent_AttackCommand` | 304 (red target) |
| LMB on building | own | Infiltrator | `GameEvent_InfiltratorAssigned` | 244 |
| LMB on building | own | Technician | `GameEvent_TechnicianAssigned` | — |
| LMB on repair bay | own | CombatVehicle | `GameEvent_RepairBayAssigned` | 144 (wrench) |
| LMB on building | own | MobileBase | `GameEvent_MobileBaseDeployed` | 188 |
| LMB on lab | own | — | `TaskMessage_UpgradeStarted` | 216 |
| LMB on unit | — | — | `GameEvent_SelectedUnit` | 48 (default) |
| LMB drag | — | — | Box select → `GameEvent_SelectedBox` | — |

### Box Selection (cursor_42C9E0, line 44226)
Creates 4 corner entities. Drag defines rectangle. On release: iterates units in box, builds selection list. Generates `GameEvent_SelectionCleared` then `GameEvent_SelectedBox`.

### cursor_429D40 (line 41981) — Classify Selected Unit
Maps unit type to `UnitCommandArchetype`:
- Infantry with weapon → `CombatInfantry`
- Vehicle with weapon → `CombatVehicle`
- Saboteur/Vandal → `Infiltrator`
- Engineer → `Technician`
- Outpost/Clanhall → `MobileBase`
- Derrick → `MobileDerrick`
- Tanker → `Tanker`
- Lab → `Lab`

---

## Game Event System

### GameEventNode (0x14 bytes, pool of 8)
```
0x00  GameEventNode*  next
0x04  GameEventType   type
0x08  char[12]        payload    ← varies by event type
```

### Event Flow
```
cursor_group_orders / sidebar / etc
  → g_game_event_queue.evt.type = GameEvent_X
  → g_game_event_queue.evt.payload = data
  → queue_game_event()
       ↓
  network sync (multiplayer) or immediate (singleplayer)
       ↓
  event_dispatch_handler (line 69620)  ← big switch on GameEventType
       ↓
  TASK_send_message(sender, TaskMessage_X, payload, target_task)
```

### Event Dispatch Table (line 69620)

| GameEvent | Action |
|-----------|--------|
| `SelectedBox` | Iterates units in box, builds selection list |
| `SelectedUnit` | Clears selection, adds single unit |
| `SelectionCleared` | Returns all selection nodes to pool |
| `AssignedControlGroup` | Sets `control_groups[player]` on all selected units |
| `RecalledControlGroup` | Clears selection, selects all units in group |
| `MoveCommand` | Sends `TaskMessage_MoveOrder` to all selected |
| `AttackCommand` | Sends `TaskMessage_AttackOrder` to all selected |
| `UnitProduced` | Sends `TaskMessage_UnitReady` to factory task |
| `BuildingPlaced` | Calls `entity_create_by_unit_type` |
| `MobileBaseDeployed` | Sends `TaskMessage_BuildingPlacementModeBegin` |
| `BuildingConstructionProgressed` | Sends `TaskMessage_AdvanceConstructionStage` |
| `UpgradeCompleted` | Sends `TaskMessage_UpgradeComplete` |
| `RepairBayHealTick` | Adjusts unit hitpoints directly |
| `GamePaused/Resumed` | Sets pause flags (multiplayer only) |
| `TankerAssignedToDrillrig` | Sends `TaskMessage_TankerAssignedNewDrillrig` |
| `TankerAssignedToPowerPlant` | Sends `TaskMessage_TankerAssignedNewPowerPlant` |
| `RepairBayAssigned` | Sends `TaskMessage_RepairBayAssigned` |
| `TechnicianAssigned` | Sends `TaskMessage_TechnicianAssigned` |
| `InfiltratorAssigned` | Sends `TaskMessage_Infiltrate` |
| `AirstrikeCalled` | Spawns Wasp/Buzzard unit at coordinates |
| `BuildingSold` | Sends `TaskMessage_Destroy` |
| `SwearAllegiance` | Sets diplomacy alliance, shows message |

---

## Sidebar System

### Sidebar (0x4C bytes)
```
0x00  next/prev       — linked list (pool-managed)
0x08  Task*           task → task_sidebar coroutine
0x0C  int             num_buttons
0x10  fixed           x → (screen_width - 320 + margin_right) << 8
0x14  fixed           y
0x18  int             h_spacing → 0 for vertical
0x1C  int             v_spacing → g_sidebar_default_v_spacing for vertical
0x20  Entity*         entity → MobdId_Sidebar
0x24  SidebarButton*  button_list_head
0x28  SidebarButton*  button_list_tail
0x2C-0x48             unused fields
```

### sidebar_create (line 18635)
Allocates from `g_sidebar_free_list_head`. Position: `x = (margin_right + screen_width - 320) << 8`. Entity uses `render_state_handler_ui`. Default task: `task_sidebar` (infinite wait loop).

### GAME_sidebar_init (line 67656)

Pools allocated:
- 256 `SidebarFactoryProductionOption` nodes (0x3000 bytes)
- 32 `SidebarFactoryProduction` nodes (0x980 bytes)

Creates main sidebar `g_sidebar` at margin=288, vertical.

Button creation order (top to bottom):
1. **Cash** — `sidebar_mode_cash_open/close`, ctx=-1
2. **Minimap** — `sidebar_mode_minimap_open/close`, ctx=-2 → `g_sidebar_button_minimap`
3. **Options** — `sidebar_mode_options_open` (no close), ctx=-3
4. **Help** — `sidebar_mode_help_open/close`, ctx=-4 → `g_help_button`
5. **Infantry** — `sidebar_mode_infantry_open/close`, ctx=-5 → `g_production_buttons[0]`
6. **Vehicles** — `sidebar_mode_vehicles_open/close`, ctx=-6 → `g_production_buttons[1]`
7. **Buildings** — `sidebar_mode_buildings_open/close`, ctx=-7 → `g_production_buttons[2]`
8. **Towers** — `sidebar_mode_towers_open/close`, ctx=-8 → `g_production_buttons[3]`
9. **Aircraft** — `sidebar_mode_aircraft_open/close`, ctx=-9 → `g_production_buttons[4]`
10. **Airstrike** — `sidebar_mode_airstrike_open/close`, ctx=-10 → `g_airstrike_button` (created on null sidebar = floating)

Faction detection: if Evolved (player_num==2 singleplayer, or faction byte set in MP) → offset mobd indices by 11 for alternate skin.

---

## SidebarButton (0x28 bytes)

```
0x00  next/prev              — button list in parent Sidebar
0x08  Task*                  task → button coroutine
0x0C  void(*)(SidebarButton*) mode_open → on-click handler
0x10  void(*)(SidebarButton*) mode_close → on-deselect handler (type2/3 only)
0x14  ptrdiff_t              icon_mobd_frame → display sprite offset
0x18  int                    base_cost → total cost for progress bar calc
0x1C  ProductionSharedState* production_state → shared production tracking
0x20  void*                  ctx → polymorphic:
                                    SidebarFactoryProductionOption* (unit buttons)
                                    UnitType (building buttons)
                                    RenderString* (cash button)
                                    negative int (main sidebar buttons: -1..-10)
0x24  Entity*                entity → visual button entity
```

### Button Creation Variants

| Creator | Task Loop | Channel | Usage |
|---------|-----------|---------|-------|
| `sidebar_button_create_1` | `task_sidebar_buttons_1_2` | SidebarButton (0xBABA) | Simple click (options) |
| `sidebar_button_create_2` | `task_sidebar_buttons_1_2` | SidebarButton (0xBABA) | Toggle (cash, minimap, production categories) |
| `sidebar_button_create_3` | `task_sidebar_buttons_3` | SidebarHelp (0xBBBB) | Special (help, airstrike) |
| `sidebar_button_create_prod` | `task_sidebar_buttons_production` | SidebarButton (0xBABA) | Production units with progress bar |

All call `sidebar_button_create_ex` (line 19428) which allocates from `g_sidebar_button_free_list_head` pool.

### Button Task Loops

#### task_sidebar_buttons_1_2 (line 18686)
Standard button behavior:
```
loop:
  yield(Task_1)        ← wait for message
  handle messages:
    TaskMessage_SidebarRefreshOptions → update icon
    TaskMessage_1514_sidebar → mode_open()
    TaskMessage_UnitSelected → mode_open(), mark selected
    TaskMessage_UnitDeselected → mode_close() if has close handler
    TaskMessage_MouseHover → track hover state
  update animation frame (normal vs hover)
```

#### task_sidebar_buttons_3 (line 18884)
Same as 1_2 but also handles:
- `TaskMessage_1513_sidebar` → right-click cancel / mode toggle

#### task_sidebar_buttons_production (line 19071)
Production unit button with progress tracking:
```
loop:
  yield(Task_Sleep, 6)    ← tick every 6 frames
  if production_state:
    update progress bar:
      segment = (base_cost - remaining_cost) / (base_cost / 16)
      entity_anim_load(progress_bar, 2312 + segment)
    update queue label:
      if num_orders > 9: show infinity (mobd 2160)
      else: show digit (mobd 2276 + num_orders)
  handle messages:
    LMB (TaskMessage_UnitSelected) → enqueue unit if !is_at_units_limit()
    RMB (TaskMessage_1513_sidebar) → dequeue unit
```

Creates child entities for progress bar (mobd 2312, 16 frames) and queue counter (mobd 2276 digits / 2160 infinity).

---

## Sidebar Mode Handlers

### Category Open Pattern (Infantry/Vehicles/Aircraft/Buildings/Towers)

All follow same pattern:
```
sidebar_mode_X_open(button):
  1. Close any other open sidebar: check g_is_sidebar_open[i], call close handler
  2. Set g_is_sidebar_open[type] = 1
  3. For each factory of this type in g_factory_production_heads[type]:
     a. Create sub-sidebar at y = 256 - 32*index
     b. Add factory color bar icon entity
     c. For each production option in factory:
        create production button via sidebar_button_create_prod
  4. Store sidebar references for cleanup

sidebar_mode_X_close(button):
  1. Remove icon entities
  2. Destroy sub-sidebars (sidebar_4102D0 each button)
  3. Set g_is_sidebar_open[type] = 0
```

### Special Modes

| Handler | Action |
|---------|--------|
| `sidebar_mode_order_unit` | Calls `enqueue_infantry()` if `!is_at_units_limit()` |
| `sidebar_mode_place_building` | Sets `planner` payload, sends `TaskMessage_BuildingPlacementModeBegin` to `g_task_47C028` |
| `sidebar_mode_options_open` | Sends `TaskMessage_ToggleIngameMenu` to `g_task_47C028` |
| `sidebar_mode_cash_open` | Creates UiString showing cash amount |
| `sidebar_mode_cash_close` | Removes cash UiString |
| `sidebar_mode_help_open` | Sets `is_help_mode = 1` on cursor |
| `sidebar_mode_help_close` | Clears `is_help_mode` |
| `sidebar_mode_airstrike_open` | Sets `is_airstrike_targeting = 1` |
| `sidebar_mode_airstrike_close` | Clears `is_airstrike_targeting` |

---

## Production Pipeline

### Data Flow
```
Sidebar click → sidebar_mode_order_unit
  → enqueue_infantry(cash, state, bandwidth, notification_task, notification_arg, key)
    → enqueue_infantry_ex(cash, state, base_cost, bandwidth, ...)
      → allocate FactoryProductionOption from g_47A530
      → link into FactoryProduction from g_47A4B0
      → set remaining_cost, remaining_cash, accumulator=0
         ↓
  Per-frame tick (sub_4280A0):
    for each FactoryProduction:
      effective_bandwidth = base_bandwidth / num_active_productions
      for each FactoryProductionOption:
        accumulator += effective_bandwidth
        while accumulator >= 256:
          if *remaining_cash > 0:
            --*remaining_cash
            --*remaining_cost
            accumulator -= 256
        if *remaining_cost <= 0:
          if !is_at_units_limit():
            TASK_send_message(sender, TaskMessage_UnitReady, notification_arg, notification_task)
            if *num_orders > 1:
              --*num_orders
              *remaining_cost = base_cost   ← reset for next in queue
            else:
              remove option, recycle
```

### Key Formulas
- **Bandwidth**: `(cost << 8) / (production_time × 60)` — money per tick in 1/256ths
- **Effective bandwidth**: split among concurrent productions in same factory
- **256 accumulator = $1**: fractional spending accumulation
- **Progress bar**: 16 segments, `segment = (base_cost - remaining_cost) / (base_cost / 16)`

### AI vs Player Production
- **Player**: `notification_task = g_game_update_loop_task`, `key = slot + 16*type`, progress bar visible
- **AI**: `notification_task = factory->task`, `key = -1` (ungrouped), no UI

### ProductionSharedState (0x10 bytes)
Bridge between sidebar UI and production engine:
```
0x00  int                remaining_cost    → shared with FactoryProductionOption
0x04  int                num_orders        → queue size, shared
0x08  Entity*            progress_bar      → mobd 2312 (16 frames)
0x0C  Entity*            queue_size_label  → mobd 2276 (1-9) or 2160 (∞)
```

### SidebarFactoryProduction (0x4C bytes)
Per-factory production group:
```
0x00  next/prev                  — linked in g_factory_production_heads[type]
0x08  SidebarFactoryProductionType type
0x0C  Unit*                      factory_or_factory_type
0x10  Sidebar*                   sidebar → sub-sidebar panel
0x14  SidebarFactoryProductionOption* production_list_head
0x18  SidebarFactoryProductionOption* production_list_tail
0x1C  Unit*                      base_building_for_aircraft_spawn
0x40  int                        key → slot_index + 16 * PRODUCTION_GROUP_ID
0x44  int                        factory_header_color_idx → color bar for factory icon
0x48  Entity*                    icon_entity → factory header entity
```

### FactoryProduction (0x40 bytes)
Core production engine node:
```
0x00  next/prev                  — active production list
0x08  FactoryProductionOption*   production_list_head
0x0C  FactoryProductionOption*   production_list_tail
0x3C  int                        key → matches sidebar key, -1 for AI
0x40  int                        num_active_productions → bandwidth divisor
```

---

## SidebarPayload — Airstrike Button Controller (0x14 bytes)

```
0x00  void(*)(SidebarPayload*) mode       → state function
0x04  int                      timer      → countdown frames
0x08  Entity*                  entity     → countdown number display
0x0C  Entity*                  airstrike_button_entity
0x10  Task*                    task
```

State machine:
```
sidebar_mode_401A40 (hidden) → sidebar_mode_4019A0 (activating)
  → sidebar_mode_401A80 (countdown visible) → sidebar_mode_401A40 (hidden again)
```

Manages airstrike cooldown display. Entity positioned at fixed screen coords (155648, 73728). Animation 2276 frames 0-8 for countdown digits.

---

## UI Rendering

### render_state_handler_ui
All UI entities use this handler. Positions in **screen space** (not world-relative). Palette tinting, z-ordering for layering.

### render_state_handler_cursor (line 69039)
Cursor entity positioned relative to camera:
```c
node->x = (entity->x >> 8) - (camera.x >> 8) - frame->x;
node->y = (entity->y >> 8) - (camera.y >> 8) - frame->y;
node->z = entity->z + 0x40000000;  // very high z for overlay
```

### Cursor Animation Offsets
| Offset | Cursor |
|--------|--------|
| 0 | Default arrow (hidden initially) |
| 12 | Normal pointer |
| 48 | Select/identify |
| 144 | Wrench (repair) |
| 188 | Deploy base |
| 216 | Research/upgrade |
| 244 | Infiltrate |
| 304 | Red target (attack) |
| 384 | Green arrow (move) |

---

## Key Global Variables

| Variable | Line | Purpose |
|----------|------|---------|
| `g_cursor` | 5445 | Pointer to CursorState (on stack in game loop) |
| `g_game_update_loop_task` | 5441 | Main game loop task — receives UnitReady etc |
| `g_task_47C028` | 5532 | Ingame menu task — receives ToggleIngameMenu, BuildingPlacementModeBegin |
| `g_sidebar` | 5610 | Main sidebar instance |
| `g_production_buttons[5]` | 5622 | Production category buttons [Inf/Veh/Bld/Twr/Air] |
| `g_help_button` | 5619 | Help button instance |
| `g_airstrike_button` | 5620 | Airstrike button (floating, no parent sidebar) |
| `g_sidebar_button_minimap` | 5621 | Minimap toggle button |
| `g_factory_production_heads[5]` | 5609 | SidebarFactoryProduction sentinel per type |
| `g_is_sidebar_open[5]` | — | Bool per production type |
| `g_sidebar_color_bars[6]` | — | Bitmask of used color indices per type |
| `g_key_modifier` | — | Scancode_None / Scancode_Ctrl / Scancode_Alt |

---

## Decompilation Issues

1. **`cursor->is_rmb_scrolling = BuildingConstructionStage_1`** — should be `TRUE` (value 1). Decompiler picked wrong enum.
2. **`g_task_47C028`** — cryptic global name. Actually the **ingame menu controller task**. Suggest: `g_ingame_menu_task`.
3. **GAME_sidebar_init loop bug** (line 67732): `while ((int)v4 < (int)&g_sidebar)` — iterates `g_factory_production_heads` array but uses address of next global as terminator. Works by accident but fragile.
4. **`sub_44CA30`** in event dispatch — used everywhere to resolve entity IDs to `Unit*`. Suggest: `unit_find_by_entity_id`.
5. **Negative ctx values** (-1 through -10) for main sidebar buttons — clever hack to distinguish "system" buttons from production buttons. Cast to `void*` then checked as signed int.
6. **`dword_47CA2C`** — set to 0 in init, checked in save/load. Likely `g_sidebar_initialized` or `g_has_active_game`.
7. **Task channels**: `0xBABA = TaskChannel_SidebarButton`, `0xBBBB = TaskChannel_SidebarHelp`. Named well already.
8. **`_sidebar_payload_4`** → `airstrike_cooldown_timer`.
9. **`_sidebar_payload_8`** → `countdown_digit_entity`.

## Naming Suggestions

| Current | Suggested |
|---------|-----------|
| `g_task_47C028` | `g_ingame_menu_task` |
| `sub_44CA30` | `unit_find_by_entity_id` |
| `sub_42B600` | `cursor_handle_sidebar_hover` |
| `sub_42B230` | `cursor_issue_move_command` |
| `sub_42D390` | `cursor_netz_debug_spawn` |
| `cursor_429D40` | `cursor_classify_unit_archetype` |
| `cursor_42AFD0` | `cursor_assign_repair_bay` |
| `cursor_42C9E0` | `cursor_begin_box_select` |
| `sidebar_4102D0` | `sidebar_destroy_button` |
| `sidebar_mode_401A40` | `airstrike_hud_mode_hidden` |
| `sidebar_mode_4019A0` | `airstrike_hud_mode_activating` |
| `sidebar_mode_401A80` | `airstrike_hud_mode_countdown` |
| `sidebar_mode_401AF0` | `airstrike_hud_mode_activate_immediate` |
| `sidebar_task_401C30` | `task_airstrike_hud` |
| `MessageHandler_401B80_sidebar_payload` | `MessageHandler_AirstrikeHud` |
| `task_sidebar_buttons_1_2` | `task_sidebar_button_standard` |
| `task_sidebar_buttons_3` | `task_sidebar_button_toggle` |
| `task_sidebar_buttons_production` | `task_sidebar_button_production` |
| `sidebar_button_create_ex` | `sidebar_button_alloc` |
| `enqueue_infantry` | `production_enqueue` |
| `enqueue_infantry_ex` | `production_enqueue_ex` |
| `sub_4280A0` | `production_tick` |
| `_sidebar_payload_4` | `airstrike_cooldown_timer` |
| `_sidebar_payload_8` | `countdown_digit_entity` |
| `dword_47CA2C` | `g_sidebar_initialized` |
| `g_47A4B0` | `g_active_productions_head` |
| `g_47A4F8` | `g_factory_production_free` |
| `g_47A530` | `g_production_option_free` |
| `g_prod_options_pool` | keep |
| `g_prod_options_free_list` | keep |
| `is_at_units_limit` | keep (good name) |
| `game_process_events` | keep (good name) |
| `cursor_group_orders` | `cursor_process_orders` |
| `cursor_load_mobd` | `cursor_set_animation` |

## Constants

| Value | Meaning |
|-------|---------|
| 320 | Sidebar panel width in pixels |
| 288 | Main sidebar margin from right edge |
| 32 | Sidebar button height (v_spacing factor) |
| 100 | CursorUnitSelection pool size |
| 8 | GameEventNode pool size |
| 256 | SidebarFactoryProductionOption pool size |
| 32 | SidebarFactoryProduction pool size |
| 16 | Progress bar segments |
| 6 | Production button tick rate (frames) |
| 0xBABA | TaskChannel_SidebarButton |
| 0xBBBB | TaskChannel_SidebarHelp |
| 0xCACA | TaskChannel_SidebarPlaceholder |
| 2312 | Progress bar mobd frame base |
| 2276 | Queue digit mobd frame base (0-9) |
| 2160 | Infinity symbol mobd frame |

---

## TODO

- [ ] Map `sub_42B600` — sidebar hover interaction handler
- [ ] Map `g_sidebar_mobds[]` table — mobd frame offsets for both factions
- [ ] Trace `TaskMessage_BuildingPlacementModeBegin` → full building placement flow through `g_task_47C028`
- [ ] Investigate minimap toggle (`sidebar_mode_minimap_open/close`) — not read yet
- [ ] Map `sidebar_mode_options_open` → `g_task_47C028` → ingame menu flow (pause, save, quit)
- [ ] Investigate `_stru9_unit_order` system (line 69672) — per-player order dispatch
