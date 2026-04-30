# KKND AI System — `Enemyai.cpp`

Original source: `C:\k\Scripts\Enemyai.cpp` (confirmed via rand call at line 14965).

## Overview

AI runs as a **Callback task** on `TaskChannel_UnitLifecycle`, ticking every **60 frames** via `TASK_yield(task, Task_Sleep, 60)`. Each AI player gets its own `AiController` (0x364 bytes) allocated via `TASK_Alloc`. Up to 6 AI players supported (slots 1–6 in `dword_470510[]`).

Three AI variants exist, selected by `g_current_lvl_id`:
| Variant | Level | Task Function | Message Handler |
|---------|-------|--------------|-----------------|
| **Ambush** | `Mute_05_Ambush` | `ai_task_42DF20_mute_05_ambush` | `MessageHandler_AiController_Mute05_Ambush` |
| **Convoy** | `Mute_08_SmashTheConvoy` | `ai_task_42DC70_mute_08_smash_the_convoy` | `MessageHandler_AiController_Mute08_SmashTheConvoy` |
| **General** | Everything else | `ai_task_40B700_general_ai` → `ai_task_409770` | `MessageHandler_AiController_General` |

## AiController Struct (0x364 bytes)

```
Offset  Type                     Field                          Suggested Name
------  ----                     -----                          --------------
0x000   AiController*            next                           → intrusive linked list (self-referencing for convoy/ambush)
0x004   AiController*            prev
0x008   Unit*                    _ai_controller_8               → used as target pointer in _1B4 list traversal
0x00C   AiController_struC*      _ai_controller_C               → squad sub-struct for convoy AI

=== Ai_stru10_Node pool (convoy/ambush only) ===
0x010   Ai_stru10_Node*          _ai_controller_10_pool
0x014   Ai_stru10_Node*          _ai_controller_10_free

=== Wanderer list: "pending" (have cplc_spawn_param countdown) ===
0x018   AiWandererNode*          _ai_controller_18_head         → wanderer_pending_head
0x01C   AiWandererNode*          _ai_controller_18_tail         → wanderer_pending_tail

0x020   int                      _ai_controller_20              → ai_strategic_value_threshold (convoy)
0x024   int                      _ai_controller_24              → target enemy node ptr (squads)

=== Wanderer node pool ===
0x028   AiWandererNode*          _ai_controller_28_pool         → wanderer_pool
0x02C   AiWandererNode*          _ai_controller_28_free         → wanderer_free

=== Wanderer list: "active" (hunting enemies) ===
0x030   AiWandererNode*          _ai_controller_30_head         → wanderer_active_head
0x034   AiWandererNode*          _ai_controller_30_tail         → wanderer_active_tail

=== Second wanderer pool (for deselected/killed tracking) ===
0x040   AiWandererNode*          _ai_controller_40_pool         → wanderer_pool_2
0x044   AiWandererNode*          _ai_controller_40_free         → wanderer_free_2

=== Attacker list: unassigned (waiting for squad) ===
0x048   AiAttackerNode*          _ai_controller_58_head         → attacker_unassigned_head  (NOTE: declared at 0x48 but named _58)
0x04C   AiAttackerNode*          _ai_controller_58_tail

=== Attacker node pool ===
0x058   AiAttackerNode*          _ai_controller_58_pool         → attacker_pool
0x05C   AiAttackerNode*          _ai_controller_58_free         → attacker_free

=== Attacker list: rallying to base (moving toward rally point) ===
0x060   AiAttackerNode*          _ai_controller_60_head         → attacker_rallying_head
0x064   AiAttackerNode*          _ai_controller_60_tail

=== Second attacker pool (convoy AI only) ===
0x070   AiAttackerNode*          _ai_controller_70              → attacker_pool_2 (convoy)
0x074   AiAttackerNode*          _ai_controller_74              → attacker_free_2

=== Building node list ===
0x078   AiBuildNode*             build_head
0x07C   AiBuildNode*             build_tail
0x094   AiBuildNode*             build_pool
0x098   AiBuildNode*             build_free

=== Drillrig node list ===
0x09C   AiDrillrigNode*          drillrig_head
0x0A0   AiDrillrigNode*          drillrig_tail
0x0D4   AiDrillrigNode*          drillrig_pool
0x0D8   AiDrillrigNode*          drillrig_free

=== Tanker node list ===
0x0DC   AiTankerNode*            tanker_head
0x0E0   AiTankerNode*            tanker_tail
0x0EC   AiTankerNode*            tanker_pool
0x0F0   AiTankerNode*            tanker_free

=== Power plant node list ===
0x0F4   AiPowerPlantNode*        powerplant_head
0x0F8   AiPowerPlantNode*        powerplant_tail
0x100   AiPowerPlantNode*        powerplant_pool
0x104   AiPowerPlantNode*        powerplant_free

=== Enemy node list ===
0x108   AiEnemyNode*             enemy_tail
0x10C   AiEnemyNode*             enemy_head
0x114   AiEnemyNode*             enemy_pool
0x118   AiEnemyNode*             enemy_free

=== Squad lists (Ai_stru160_Node) ===
0x11C   Ai_stru160_Node*         _ai_controller_11C_head        → squad_idle_head
0x120   Ai_stru160_Node*         _ai_controller_11C_tail        → squad_idle_tail
0x160   Ai_stru160_Node*         _ai_controller_160_pool        → squad_pool
0x164   Ai_stru160_Node*         _ai_controller_160_free        → squad_free

0x168   int                      _ai_controller_168_head        → squad_patrolling_head
0x16C   int                      _ai_controller_168_tail        → squad_patrolling_tail
0x1AC   int                      _ai_controller_1AC             → (counter/state)

0x1B4   Ai_stru160_Node*         _ai_controller_1B4_head        → squad_marching_head
0x1B8   Ai_stru160_Node*         _ai_controller_1B4_tail        → squad_marching_tail
0x1F8   int                      _ai_controller_1F8             → (counter/state)

0x200   int                      _ai_controller_200_head        → (4th squad list head)
0x204   int                      _ai_controller_200_tail
0x244   int                      _ai_controller_244             → (counter/state)

=== Rally point / home base squad ===
0x24C   Ai_stru160_Node*         _ai_controller_24C             → rally_squad (the main accumulator)

=== Base bounding box ===
0x250   int                      base_area_min_x
0x254   int                      base_area_min_y
0x258   int                      base_area_max_x
0x25C   int                      base_area_max_y

=== Build order queue (Ai_stru26C_Node) ===
0x260   Ai_stru26C_Node*         _ai_controller_26C_head        → build_order_head
0x264   Ai_stru26C_Node*         _ai_controller_26C_tail
0x26C   Ai_stru26C_Node*         _ai_controller_26C_pool        → build_order_pool
0x270   Ai_stru26C_Node*         _ai_controller_26C_free
0x274   Ai_stru26C_Node*         _ai_controller_274             → build_order_current

=== Rally point coordinates ===
0x278   int                      _ai_controller_278_x           → rally_x (-1 = none)
0x27C   int                      _ai_controller_27C_y           → rally_y

=== Patrol waypoints (4 waypoints × 2 sets, X/Y pairs) ===
0x280   int[2][4]                _ai_controller_280             → patrol_waypoints[set][waypoint] = {x, y}

=== Player identity ===
0x2A0   int                      player_num
0x2A4   Race                     player_race
0x2A8   int*                     cash                           → pointer into g_cash.cash[]

=== Combat params ===
0x2AC   int                      _ai_controller_2AC             → attacker_count (current production-spawned attackers)
0x2B0   int                      _ai_controller_2B0             → max_attackers = 549/(num_ai_players+1)
0x2B4   int                      _ai_controller_2B4             → squad_threshold (200 general, 4 mission)
0x2B8   int                      _ai_controller_2B8             → attack_confidence (-50)
0x2BC   int                      _ai_controller_2BC             → base_threat (sum of enemy threat in base area)
0x2C0   int                      _ai_controller_2C0             → max_squad_threat (highest threat seen across squads)
0x2C4   int                      _ai_controller_2C4_2C8_idx     → best_patrol_waypoint_idx
0x2C8   int[5]                   _ai_controller_2C8             → patrol_threat[5] (4 waypoints + sentinel)

=== Production state ===
0x2DC   UnitType                 last_unit_produced
0x2E0   UnitType                 last_unit_produced_factory
0x2E4   BOOL                     tanker_production_in_progress
0x2E8   AiDrillrigNode*          _ai_controller_2E8             → drillrig_needing_tanker

=== Mobile derrick placement queue ===
0x2EC   AiMobileDerrickNode*     _ai_controller_2EC_head        → placement_queue_head
0x2F0   AiMobileDerrickNode*     _ai_controller_2EC_tail
0x310   AiMobileDerrickNode*     _ai_controller_310             → derrick_node_pool (general only)
0x314   AiMobileDerrickNode*     _ai_controller_314             → derrick_node_free
0x318   AiMobileDerrickNode*     _ai_controller_318_head        → pending_derrick_head (waiting for unit)
0x31C   AiMobileDerrickNode*     _ai_controller_318_tail

=== Construction state ===
0x33C   int                      _ai_controller_33C
0x340   int                      _ai_controller_340
0x344   int                      _ai_controller_344             → construction_stage (0→1→2→3)
0x348   int                      _ai_controller_348_base_cost   → building_base_cost
0x34C   int                      _ai_controller_34C             → building_remaining_cost
0x350   int                      _ai_controller_350             → construction_cooldown
0x354   int                      _ai_controller_354             → construction_cost_per_tick
0x358   Task*                    _ai_controller_358             → construction_task

=== Airstrike ===
0x35C   int                      airstrike_interval             → countdown (decremented by 60 per tick)
0x360   int                      airstrike_count                → remaining strikes
```

## Node Types

### AiWandererNode (0x10 bytes)
Units spawned by CPLC (pre-placed on map). Two pools: `_28` and `_40`.
- Two lists: **pending** (`_18` — has `cplc_spawn_param` countdown) and **active** (`_30` — hunting).
- `_ai_wanderer_node_8`: unused/padding.
- Wanderers countdown `cplc_spawn_param` each tick. When it hits 0 (and not Warlord), move to active list.
- Active wanderers seek nearest enemy via `ai_42DF40` (manhattan distance) and send `TaskMessage_AttackOrder`.

### AiAttackerNode (0x10 bytes)
Units spawned by production. Tracked in `_ai_controller_2AC` count.
- `_ai_attacker_node_8`: pointer to owning `Ai_stru160_Node` (squad), or NULL if unassigned.
- Lists: **unassigned** (`_58`), **rallying** (`_60`), or **in a squad** (linked via squad's `_ai_stru160_node_C`).

### Ai_stru160_Node — Squad (0x44 bytes)
Central combat unit. Groups attackers for coordinated movement/attack.
```
0x00  next/prev           — linked in squad lists (idle/patrolling/marching)
0x08  _ai_stru160_node_8  — merge target squad (for marching squads)
0x0C  _ai_stru160_node_C  — attacker list head (intrusive doubly-linked)
0x10  _ai_stru160_node_10 — attacker list tail
0x1C  _ai_stru160_node_1C — attacker count
0x20  _ai_stru160_node_20 — (unused?)
0x24  _ai_stru160_node_24 — target enemy node (for attack orders)
0x28  _ai_stru160_node_28 — tick counter (reset to 0 on attack order)
0x2C  _ai_stru160_node_2C — total threat weight of members
0x30  _ai_stru160_node_30 — enemy threat in area (from ai_get_threat_in_area)
0x34  _ai_stru160_node_34 — centroid x
0x38  _ai_stru160_node_38 — centroid y
0x3C  _ai_stru160_node_3C — destination x
0x40  _ai_stru160_node_40 — destination y
```

Squad lifecycle:
1. **Rally squad** (`_24C`): new attackers join here at rally point
2. When `threat_weight >= squad_threshold` (200) → split off into **idle** list (`_11C`)
3. Idle squads evaluate attack confidence: `100*(own_threat - enemy_threat)/(own_threat + enemy_threat + 1) >= _2B8`
4. If confident → send `AttackOrder` → stay idle. If not → find nearest idle squad to merge with → move to **marching** (`_1B4`)
5. Marching squads converge. When within 0x4000 dist → members merge into target squad, node freed.
6. **Patrolling** (`_168`): squads sent to patrol waypoints. Return to idle when threat clears or they reach destination.

### AiBuildNode (0x1C bytes)
Tracks production buildings. `remaining_cost`/`base_cost`/`cost_per_tick` for ongoing production.

### AiDrillrigNode (0x38 bytes)
- `tanker_next/prev`: sub-list of assigned tankers
- `_ai_drillrig_node_24`: guard squad (Ai_stru160_Node*)
- `_ai_drillrig_node_28`: nearest power plant
- `_ai_drillrig_node_30`: current tanker count
- `_ai_drillrig_node_34`: desired tanker count (1–3 based on distance/51200)
- `_ai_drillrig_node_2C`: threat in area

### AiTankerNode (0x10 bytes)
- `drillrig`: assigned AiDrillrigNode* (or NULL if unassigned)

### AiEnemyNode (0x0C bytes)
- Tracks all enemy units. Pool of 599 (indices 0–598).

### AiMobileDerrickNode (0x24 bytes)
- Building placement queue. Stores target position, unit type, grid anchors.
- Two lists: `_2EC` (placement queue) and `_318` (pending: waiting for mobile derrick unit to arrive).

### Ai_stru26C_Node — Build Order (0x0C bytes)
- `_ai_stru26C_node_8`: creature_id (maps to unit type via `g_464DD0` table)
- Circular linked list; `_ai_controller_274` is current pointer.

### AiController_struC — PHANTOM STRUCT (decompiler bug)
**Does not exist.** The decompiler created this 0x134-byte struct because `_ai_controller_C` was mistyped as `AiController_struC*` instead of `Ai_stru160_Node*`. All `_ai_controller_strucC_*` field accesses are actually `Ai_stru160_Node` (0x44 bytes) fields:

| strucC field | Offset | Actual Ai_stru160_Node field | Meaning |
|---|---|---|---|
| `_strucC_0` | 0x00 | `next` | Next squad in list |
| `_strucC_4` | 0x04 | `prev` | Prev squad in list |
| `_strucC_C` | 0x0C | `_ai_stru160_node_C` | Escort member head |
| `_strucC_10` | 0x10 | `_ai_stru160_node_10` | Escort member tail |
| `_strucC_1C` | 0x1C | `_ai_stru160_node_1C` | Escort member count |

**Proof**: allocated from `_ai_controller_160_free` (squad pool). Pool = `malloc(0x880)` / 32 entries = **0x44 bytes per entry**. The 77 extra `int` fields (0x44–0x130) never exist in memory — they'd overflow into adjacent pool entries.

### Ai_stru10_Node — Convoy Vehicle Node (0x10 bytes)
Tracks convoy tanker vehicles (`UnitType_TankerConvoy = 25`). Convoy-only.
```
0x00  next/prev  — linked into AiController's intrusive list (reuses ai->next/prev chain!)
0x08  unit       — the convoy tanker Unit*
0x0C  _ai_stru10_node_C  → escort_squad (Ai_stru160_Node* for this vehicle's bodyguards)
```
When vehicle created: `escort_squad = nullptr`. Squad allocated on-demand when first escort assigned.

## AI_init (line 15192, ~648 lines)

### Pool Sizes
| Pool | General | Convoy | Ambush | Count | Alloc Size |
|------|---------|--------|--------|-------|------------|
| Enemy | ✓ | ✓ | ✓ | 599 | 0x1C14 |
| Wanderer (×2 pools) | ✓ | ✓ | ✓ | 599 each | 0x2570 each |
| Attacker (unassigned) | ✓ | ✓ | ✓ | `_2B0+16` | 16×count |
| Squad (Ai_stru160) | ✓ | ✓ | ✓ | 32 | 0x880 |
| Escort Attacker (`_70`) | — | ✓ | — | `_2B0+16` | 16×count |
| Convoy Vehicle (`_10`) | — | ✓ | — | 599 | 0x2570 |
| Build | ✓ | — | — | 32 | 0x380 |
| Drillrig | ✓ | — | — | 16 | 0x380 |
| Tanker | ✓ | — | — | 32 | 0x200 |
| PowerPlant | ✓ | — | — | 16 | 0xC0 |
| MobileDerrick | ✓ | — | — | 64 | 0x900 |
| BuildOrder | ✓ | — | — | 32 | 0x180 |

### Key Initialization
- `_ai_controller_2B0 = 549 / (num_ai_players + 1)` — per-AI attacker cap
- General: `_2B4 = 200` (squad threshold), `_2B8 = -50` (attack confidence)
- Mission: `_2B4 = 4`, `_2B8 = -50`
- Base area initialized to invalid bounds (0x40000000 / -1) in general; updated as buildings placed.
- Cash set from `g_lvl_desc[level].ai_starting_cash` or `superlvl_ai_cash` (ally).
- Airstrike config from `g_lvl_desc[level].superlvl_ai_airstrike_count_enemy/ally`.

## Main Tick: ai_task_409770 (line 13369, ~1060 lines)

Executes every 60 frames. Nine phases:

### Phase 1: Tanker Assignment (line ~13460)
Find idle tanker (`EventHandler_Passive`), assign to `_ai_controller_2E8` (drillrig needing tanker). Send `TaskMessage_TankerAssignedNewDrillrig` + `TaskMessage_TankerAssignedNewPowerPlant`.

### Phase 2: Drillrig Defense (line ~13480)
For each drillrig with a guard squad:
- Update guard squad centroid from member positions
- Check threat at drillrig location (radius 81920)
- If no threat: send guard squad to attack (→ idle list), check if drillrig needs more tankers

### Phase 3: Tanker/Derrick Production (line ~13530)
If drillrig needs tanker and no production in progress → find factory matching `last_unit_produced_factory`, start producing `last_unit_produced`.

### Phase 4: Unit Production from Build Orders (line ~13560)
If no pending `_318` (derrick) nodes: cycle through build order list (`_26C`), find matching factory, produce unit. Cost reduced by `superlvl_ai_cost_reduction >> 8`.

### Phase 5: Airstrike (line ~13780)
Only for superlevel missions (16–25). Timer decrements by 60. At 0: call `ai_40B490` to find best target, spawn Wasp/equivalent, reset timer = `(rng_min + rand(rng_max)) >> 2`.

### Phase 6: Building Placement (line ~13810)
If cash > 0: iterate `_2EC` placement queue, skip locations with threat ≥ GiantScorpion's threat weight (unless strategic value ≥ PowerStation). Create building, set up construction payment. Progress construction stages at 85%/171% thresholds.

### Phase 7: Squad Position Update (line ~13870)
For each squad in idle (`_11C`), patrolling (`_168`), and marching (`_1B4`) lists:
- Compute centroid from member positions
- Call `ai_get_threat_in_area(radius=0x10000)` for idle/patrolling, `radius=81920` for marching
- Track `max_squad_threat` across all squads
- Empty squads freed

### Phase 8: Rally Point Management (line ~14110)
`ai_40B230` evaluates base threat + 4 patrol waypoints.

If rally point exists:
- Move unassigned attackers (`_58`) to rallying list (`_60`) with MoveOrder to rally point
- Rallying attackers within 49152 of rally → join rally squad (`_24C`)
- When rally squad `threat_weight >= squad_threshold`:
  - If patrol waypoint available and `_2C4_2C8_idx == 0`: send to patrol → patrolling list
  - Otherwise: send attack order → idle list
  - Allocate new rally squad

### Phase 9: Squad Attack Decisions (line ~14170)
For marching squads (`_1B4`): if target within 0x4000 → merge members into target squad. If no target → move back to idle.

For patrolling squads (`_168`): if no threat and reached destination (within 49152) → move to idle.

For idle squads (`_11C`):
- Attack confidence formula: `100 * (own_threat - enemy_threat) / (own_threat + enemy_threat + 1) >= _2B8`
- If `_2BC` (base threat) > 0 → always attack
- If confident → `ai_40B020_send_attack_order`
- If not → find nearest idle squad via `ai_40B410`, set as merge target, send MoveOrder → marching list

### Phase 10: Wanderer Attacks (line ~14428)
Call `ai_42E070_attack_order` — process wanderer countdown + active wanderer target finding.

### Sleep
`TASK_yield(task, Task_Sleep, 60)` — wait 60 frames.

## MessageHandler_AiController_General (line 12522)

Handles `TaskMessage_UnitCreated` and `TaskMessage_UnitDeselected` (unit death/removal).

### UnitCreated
- **Enemy units**: allocated into `enemy_pool`. Race-matching triggers diplomacy alliance detection.
- **Own tanker**: → tanker node, link to tanker list
- **Own drillrig**: → drillrig node, call `ai_409650` to pair with nearest powerplant
- **Own powerplant**: → powerplant node, expand base area
- **Own production building** (Outpost/Clanhall/MachineShop/Blacksmith/BeastEnclosure): → build node, parse CPLC spawn params for build orders + rally/patrol waypoints
- **Own mobile unit with weapon + cplc_spawn_param**: → **wanderer** (pending list `_18`)
- **Own mobile unit with weapon, no cplc_spawn_param**: → **attacker** (unassigned list `_58`), increment `_2AC`

### UnitDeselected (death)
- **Enemy**: remove from enemy list, clear references in squads
- **Own tanker**: unlink from drillrig, return to free
- **Own drillrig**: release guard squad to idle, unassign tankers to unassigned tanker list, queue replacement mobile derrick
- **Own powerplant**: clear references in drillrig nodes, re-pair drillrigs, add to placement queue
- **Own production building**: cancel production, return to free
- **Own attacker**: if in squad, subtract threat weight; if squad empty, free squad. Decrement `_2AC`.
- **Own wanderer**: return to free. If missing: `"Warning: unregistered Loner"`

## Helper Functions

### ai_get_threat_in_area (line 14841)
Sum `ai_threat_weight` of all enemy units within `±radius` (manhattan box) of (x, y). Skips destroyed and zero-weight units.

### ai_40B230 — Evaluate Base Threat + Patrol Waypoints (line 14878)
1. Sum enemy threat weight within base bounding box ± 49152 → `_2BC`
2. For each of 4 patrol waypoints: sum enemy threat in 81920 radius. Mobile units add, buildings subtract half.
3. Pick most-threatened waypoint → `_2C4_2C8_idx`. Ties broken randomly.

### ai_40B410 — Find Nearest Idle Squad (line 14982)
Manhattan distance from (x,y) to each idle squad centroid. Returns closest (excluding self).

### ai_40B490 — Airstrike Location Selection (line 15028)
Divides map into 0x4000-unit grid cells. Iterates enemies, builds density heatmap. Buildings weighted by `(strategic_value + threat_weight + 10) >> 2`. Returns coordinates of densest cell.

### ai_40AFC0_send_move_order (line 14720)
Sends `TaskMessage_MoveOrder` to all attackers in a squad.

### ai_40B020_send_attack_order (line 14740)
Finds best enemy target: prefers within 0x10000 range (closest), otherwise weights by `distance / strategic_value`. Sends `TaskMessage_AttackOrder` to all squad members.

### ai_42DF40 — Find Nearest Enemy (line 45567)
Manhattan distance to all enemies. Warlord units skip search (return NULL). Used by wanderers.

### ai_42E070_attack_order — Wanderer Attack Logic (line 45657)
Two passes:
1. **Pending wanderers** (`_18`): decrement `cplc_spawn_param`. If odd & enemy within 51200 → reset to 1 (trigger soon). When 0 → move to active list (skip Warlords).
2. **Active wanderers** (`_30`): if idle (no target), find nearest enemy via `ai_42DF40`, send AttackOrder.

### ai_update_base_area (line 13202)
Expand base bounding box to include entity position.

### ai_409650 — Pair Drillrigs with Powerplants (line 13275)
For each drillrig: find nearest non-destroyed powerplant by manhattan distance → `_ai_drillrig_node_28`. Calculate tanker count: `distance / 51200` clamped to [1, 3]. Special: `Mute_03_BubblinCrude` always uses 1.

## Airstrike System

Config per level via `AiAirstrikeConfig`:
```c
struct AiAirstrikeConfig {
    __int16 count;            // total strikes allowed
    __int16 interval;         // initial countdown
    unsigned __int16 rng_interval_min;
    unsigned __int16 rng_interval_max;
};
```

Levels with airstrikes:
| Level | Count | Initial Interval | Min | Max |
|-------|-------|-----------------|-----|-----|
| surv_24 | 10 | 40000* | 25000 | 3000 |
| surv_25 | 10 | 30000 | 25000 | 3000 |
| mute_23 | 6 | 35000* | 30000 | 3000 |
| mute_24 | 4 | 19000 | 17000 | 1000 |
| mute_25 | 30 | 30000 | 25000 | 3000 |

*Negative `__int16` wrapping of large unsigned values.

Reset formula: `(rng_min + rand(rng_max)) >> 2`

## Key Constants

| Constant | Value | Meaning |
|----------|-------|---------|
| Max attackers per AI | `549/(num_ai+1)` | Scales inversely with AI count |
| Squad threshold (general) | 200 | Threat weight to split from rally |
| Squad threshold (mission) | 4 | Very aggressive splitting |
| Attack confidence | -50 | `100*(own-enemy)/(total+1) >= -50` |
| Base area buffer | 49152 | Expanded bounds for threat check |
| Patrol/guard radius | 81920 | Threat scanning radius |
| Rally arrival dist | 49152 | Manhattan dist to join rally squad |
| Squad merge dist | 0x4000 | Close enough to merge |
| Wanderer alert dist | 51200 | Enemy proximity triggers early activation |
| Tick interval | 60 frames | AI update frequency |

## Naming Suggestions

| Current | Suggested |
|---------|-----------|
| `_ai_controller_18_head/tail` | `wanderer_pending_head/tail` |
| `_ai_controller_30_head/tail` | `wanderer_active_head/tail` |
| `_ai_controller_58_head/tail` | `attacker_unassigned_head/tail` |
| `_ai_controller_60_head/tail` | `attacker_rallying_head/tail` |
| `_ai_controller_11C_head/tail` | `squad_idle_head/tail` |
| `_ai_controller_168_head/tail` | `squad_patrolling_head/tail` |
| `_ai_controller_1B4_head/tail` | `squad_marching_head/tail` |
| `_ai_controller_24C` | `rally_squad` |
| `_ai_controller_278_x/27C_y` | `rally_x/rally_y` |
| `_ai_controller_280` | `patrol_waypoints` |
| `_ai_controller_2AC` | `attacker_count` |
| `_ai_controller_2B0` | `max_attackers` |
| `_ai_controller_2B4` | `squad_split_threshold` |
| `_ai_controller_2B8` | `attack_confidence_threshold` |
| `_ai_controller_2BC` | `base_area_threat` |
| `_ai_controller_2C0` | `max_squad_threat` |
| `_ai_controller_2C4_2C8_idx` | `best_patrol_idx` |
| `_ai_controller_2C8` | `patrol_threat_scores` |
| `_ai_controller_2E8` | `drillrig_needing_tanker` |
| `_ai_controller_274` | `current_build_order` |
| `_ai_controller_350` | `construction_cooldown` |
| `_ai_controller_358` | `construction_task` |
| `Ai_stru160_Node` | `AiSquadNode` |
| `Ai_stru26C_Node` | `AiBuildOrderNode` |
| `_ai_stru160_node_C` | `member_head` (attacker list) |
| `_ai_stru160_node_8` | `merge_target` |
| `_ai_stru160_node_24` | `attack_target_enemy` |
| `_ai_stru160_node_28` | `idle_ticks` |
| `_ai_stru160_node_2C` | `total_threat_weight` |
| `_ai_stru160_node_30` | `area_enemy_threat` |
| `_ai_stru160_node_34/38` | `centroid_x/y` |
| `_ai_stru160_node_3C/40` | `destination_x/y` |
| `ai_42DF40` | `ai_find_nearest_enemy` |
| `ai_42E070_attack_order` | `ai_process_wanderers` |
| `sub_42E030` | `ai_wanderer_attack_nearest` |
| `ai_40B230` | `ai_evaluate_threats` |
| `ai_40B410` | `ai_find_nearest_idle_squad` |
| `ai_40B490` | `ai_find_airstrike_target` |
| `ai_40AFC0_send_move_order` | `ai_squad_send_move` |
| `ai_40B020_send_attack_order` | `ai_squad_send_attack` |
| `ai_409650` | `ai_pair_drillrigs_powerplants` |
| `ai_update_base_area` | `ai_expand_base_bounds` |
| `AiController_struC` | **DELETE** — phantom struct, use `Ai_stru160_Node*` |
| `_ai_controller_C` | `escort_squad` (convoy) — actually `Ai_stru160_Node*` |
| `Ai_stru10_Node` | `AiConvoyVehicleNode` |
| `_ai_stru10_node_C` | `escort_squad` |
| `_ai_controller_10_pool` | `convoy_vehicle_pool` |
| `_ai_controller_10_free` | `convoy_vehicle_free` |
| `_ai_controller_70` (convoy) | `escort_attacker_pool` |
| `_ai_controller_74` (convoy) | `escort_attacker_free` |
| `_ai_controller_58_head` (ambush) | `neutral_head` (dormant hidden units) |
| `_ai_stru160_node_20` | `dead_member_free` (per-squad dead escort recycling) |
| `ai_task_42DA90` | `ai_convoy_tick` |
| `ai_task_42DE80` | `ai_ambush_tick` |
| `ai_task_42DC70_mute_08_smash_the_convoy` | `ai_convoy_entry` |
| `ai_task_42DF20_mute_05_ambush` | `ai_ambush_entry` |
| `unit_425820` | `unit_find_nearest_player_in_range` |
| `unit_4258C0` | `unit_change_owner` |

## Decompilation Issues

1. **ai_40B020**: `attack_order.player_num` stores `AiController*` pointer, then used as `int` for comparison at offset 700 (`_2BC`). Decompiler confused variable reuse in register.
2. **AiController struct offsets**: Header shows `_ai_controller_58_head` at what should be offset 0x48. Likely misaligned struct layout or padding confusion.
3. **AiController_struC**: **PHANTOM STRUCT** — doesn't exist. `_ai_controller_C` should be `Ai_stru160_Node*`. Pool allocates 0x44 bytes per entry, not 0x134. All `_ai_controller_strucC_*` fields map to `Ai_stru160_Node` fields at matching offsets. The 77 extra int fields past offset 0x44 are fabricated.
4. **`_ai_controller_8` field**: Overloaded — used as `Unit*` in `_1B4` list iteration but as generic pointer elsewhere.
5. **`char _ai_controller_38[8]`** and similar padding gaps: likely contain list sentinel data that decompiler couldn't resolve.
6. **`_ai_controller_30_head`** typed as `AiWandererNode*` but convoy code casts to `AiAttackerNode*`. Both node types are 0x10 bytes with same layout `{next, prev, extra, unit}`. Decompiler chose wrong type for one context.
7. **`_ai_stru160_node_20`** in convoy: used as per-squad free list for dead escort members. Dead attacker nodes pushed here but never returned to global pool — potential memory leak or intentional squad-scoped recycling.

## TODO

- [x] ~~Investigate mission-specific AIs (Mute_05 ambush, Mute_08 convoy) in detail~~
- [x] ~~Map AiController_struC fields from convoy AI usage~~ → PHANTOM STRUCT, doesn't exist
- [ ] Trace `sub_40AB60` — placement validation function
- [ ] Map `_ai_controller_D0`, `_ai_controller_E4/E8`, `_ai_controller_FC` fields
- [ ] Investigate `_ai_controller_200` list purpose (4th squad state?)
- [ ] Verify `_ai_stru160_node_20` leak in convoy (dead escorts never returned to global pool)

---

## Convoy AI — Mute_08_SmashTheConvoy (Deep Dive)

### Architecture
Convoy AI protects tanker vehicles being escorted across the map. Uses the general `AiController` (0x364) struct but with a **different unit classification** scheme and **no squad-level attack decisions**.

Key difference from general AI: `AiController.next/prev` forms an intrusive linked list of **convoy vehicles** (via `Ai_stru10_Node`), not an AI player chain. Self-referencing sentinel (`ai->next = ai->prev = ai` initially) marks empty list.

### Pools Allocated (Convoy Init, line ~15430)

| Pool | Type | Count | Size | Purpose |
|------|------|-------|------|---------|
| Enemy | AiEnemyNode | 599 | 0x1C14 | Track hostile units |
| Squad | Ai_stru160_Node | 32 | 0x880 | Escort squads per vehicle |
| Escort Attacker | AiAttackerNode | `_2B0+16` | 16×count | Bodyguard nodes |
| Neutral Attacker | AiAttackerNode | `_2B0+16` | 16×count | Pre-placed neutral units |
| Convoy Vehicle | Ai_stru10_Node | 599 | 0x2570 | Tanker tracking nodes |
| Wanderer | AiWandererNode | 599×2 | 0x2570×2 | Pre-placed combat units |

### Unit Classification (Message Handler, line 44949)

On `TaskMessage_UnitCreated`:

| Condition | Pool/List | Node Type | Notes |
|-----------|-----------|-----------|-------|
| Enemy unit | `enemy_pool` → `enemy_tail` | AiEnemyNode | Standard |
| `unit->type == 25` (TankerConvoy) | `_10_free` → `ai->next` chain | Ai_stru10_Node | `escort_squad = NULL` |
| `cplc_spawn_param > 1` | `_28_free` → `_18_head` | AiWandererNode | Pending wanderer |
| `cplc_spawn_param == 1` | `_74` (escort pool) → `_60_head` | AiAttackerNode | Rallying escort, sets `path_flags |= 0x200` |
| `player_num == 0` | `_58_free` → `_58_head` | AiAttackerNode | Neutral unit |

On `TaskMessage_UnitDeselected` (death):

| Condition | Action |
|-----------|--------|
| `unit->type == 25` (convoy vehicle dies) | 1. Detach ALL escorts from vehicle's squad → move to `_30` (active wanderers), clear orders. 2. Free escort squad → `_160_free`. 3. Free vehicle node → `_10_free` |
| `path_flags & 0x80` (wanderer) | Free to `_40_free` |
| `path_flags & 0x200` (escort) + has squad | Find self in squad member list, remove, push to squad's `_stru160_node_20` (per-squad dead pool) |
| `path_flags & 0x200` (escort) + no squad | Free to `_74` (escort pool) |
| Neutral (no special flags) | Free to `_58_free` |
| Enemy | Free to `enemy_free` |

### Main Tick — ai_task_42DA90 (line 45248)

```
for each rallying attacker in _60 list:
    find nearest non-destroyed convoy vehicle (iterate ai->next chain)
    if vehicle found:
        if vehicle has no escort squad:
            allocate Ai_stru160_Node from _160_free
            link into _11C (idle squad list)
            init empty member sentinel: _C = _10 = &self._C
            set member_count (_1C) = 0
        assign attacker to vehicle's escort squad:
            send TaskMessage_Follow(player_num, vehicle_unit) to vehicle task
            unlink from _60 list
            link into squad's member list (_C)

call ai_42E070_attack_order()  ← process wanderers
sleep 60
```

**Key insight:** Convoy AI assigns escorts to **individual vehicles**, not to strategic targets. Each vehicle accumulates its own squad. No attack confidence evaluation — escorts just follow their vehicle. When a vehicle dies, its escorts become free wanderers that attack nearest enemy.

### Flowchart

```
Unit spawns → message handler classifies:
  Enemy → enemy list
  TankerConvoy → vehicle list (no escort initially)
  cplc_spawn_param > 1 → pending wanderer (countdown)
  cplc_spawn_param == 1 → rallying escort
  player_num == 0 → neutral (unused in convoy?)

Every 60 frames:
  For each rallying escort:
    Find nearest convoy vehicle
    Create/reuse escort squad for that vehicle
    Attach escort, send Follow order

  Wanderer attack logic (shared with general AI)

Vehicle dies:
  Escorts → active wanderers (attack nearest enemy)
  Squad freed

Escort dies:
  Removed from vehicle's squad (recycled to squad dead pool)
```

---

## Ambush AI — Mute_05_Ambush (Deep Dive)

### Architecture
Ambush AI implements a **one-shot trigger** mechanic. Hidden (neutral) units lie dormant until an enemy approaches, then all spring to life simultaneously.

Simpler than convoy. Three unit categories, **no convoy vehicle tracking**, no escort mechanics.

### Pools Allocated (Ambush Init, line ~15280)

| Pool | Type | Count | Size | Purpose |
|------|------|-------|------|---------|
| Enemy | AiEnemyNode | 599 | 0x1C14 | Track hostile units |
| Squad | Ai_stru160_Node | 32 | 0x880 | Unused in ambush (allocated but no code references them) |
| Neutral | AiAttackerNode | `_2B0+16` | 16×count | Dormant hidden units |
| Wanderer | AiWandererNode | 599×2 | 0x2570×2 | Active combat units |

**Not allocated** (set to NULL): `_ai_controller_70` (escort pool), `_ai_controller_10_pool` (convoy pool).

### Unit Classification (Message Handler, line 45390)

On `TaskMessage_UnitCreated`:

| Condition | Pool/List | Notes |
|-----------|-----------|-------|
| Enemy | `enemy_pool` → `enemy_tail` | Standard |
| `player_num != 0` (own unit) | `_28_free` → `_18_head` | Wanderer, sets `cplc_spawn_param = 2` |
| `player_num == 0` (neutral) | `_58_free` → `_58_head` | Dormant hidden unit |

On `TaskMessage_UnitDeselected`:

| Condition | Action |
|-----------|--------|
| `player_num == ai->player_num` | Free wanderer to `_40_free` |
| `player_num != 0 && != ai` | Free enemy |
| `player_num == 0` | Free neutral to `_58_free` |

### Main Tick — ai_task_42DE80 (line 45497)

```
if neutral list (_58) not empty:
    check = unit_425820(first_neutral->unit, 76800)
    if any player unit within 76800 manhattan distance:
        === AMBUSH TRIGGERED ===
        for each neutral in _58 list:
            unit_4258C0(unit, g_player_num)  ← change ownership to player
            broadcast TaskMessage_UnitCreated  ← re-classifies as wanderer
            unit_mode_415540(unit)             ← activate unit AI
            free neutral node to _58_free

call ai_42E070_attack_order()  ← process wanderers
sleep 60
```

### Trigger Mechanics

1. **Detection**: Only checks the **first** neutral unit's proximity to player units. If first neutral is far from action, ambush won't trigger even if other neutrals are close.
2. **Radius 76800**: Manhattan distance (~3 screen widths in game coordinates).
3. **All-or-nothing**: When triggered, **every** neutral converts simultaneously. No partial activation.
4. **Re-classification**: Converted neutrals broadcast `TaskMessage_UnitCreated`. The same message handler receives this, sees `player_num != 0`, classifies them as wanderers with `cplc_spawn_param = 2`.
5. **Post-trigger**: Wanderer attack logic (`ai_42E070_attack_order`) handles the rest — countdown from 2, then attack nearest enemy.

### `unit_425820` — Find Nearest Player Unit In Range (line 38004)

Iterates global `g_unit_list_head`. For each non-destroyed unit belonging to `g_player_num`: manhattan distance check against `radius`. Returns first unit within range, or NULL.

### `unit_4258C0` — Change Unit Ownership (line 38062)

Changes unit's player affiliation. Updates `g_num_player_units` / `g_num_ai_units` counters. Resets entity sprite/palette for new owner.

### Flowchart

```
Map loads → units classified:
  Neutral (player_num=0) → _58 list (dormant)
  Own units → wanderer pending list (_18)
  Enemies → enemy list

Every 60 frames:
  If neutrals exist AND player unit within 76800 of first neutral:
    ╔═══════════════════════════╗
    ║    AMBUSH TRIGGERED!      ║
    ╚═══════════════════════════╝
    All neutrals → convert to player units → wanderers

  Wanderer attack logic:
    Pending: countdown cplc_spawn_param (starts at 2)
    Active: attack nearest enemy

Post-ambush: pure wanderer combat, no further neutral checks
```

---

## Shared Convoy/Ambush: ai_42E070_attack_order (line 45657)

Used by all three AI variants. Two-phase wanderer processing:

### Phase 1: Pending Wanderers (`_18` list)
```
for each wanderer in pending list:
    if cplc_spawn_param > 0:
        if (cplc_spawn_param & 1) AND ai_42DF40 finds enemy within 51200:
            reset cplc_spawn_param = 1  ← triggers next tick
        decrement cplc_spawn_param
        if now 0 AND type != Warlord:
            move to active list (_30)
```

**Alert mechanism**: On odd ticks, checks for nearby enemies. If found within 51200, fast-tracks activation by resetting countdown to 1. This means wanderers "notice" approaching enemies and activate early.

**Warlord exception**: `UnitType_Mute_Warlord` units never join the active attack list. They remain in pending forever (countdown reaches 0 but skipped). Likely because Warlords have their own AI or are cinematic units.

### Phase 2: Active Wanderers (`_30` list)
```
for each wanderer in active list:
    if unit has no locked_target AND no order_target, OR mode == idle:
        enemy = ai_42DF40(ai, unit)  ← find nearest enemy
        if enemy found:
            send TaskMessage_AttackOrder
```

### ai_42DF40 — Find Nearest Enemy (line 45567)
Manhattan distance scan of entire enemy list. Skips destroyed units and units with same `player_num`. **Warlord units return NULL** (never attack). Outputs distance via `int* a3` parameter.

### Target Selection Algorithm (`ai_40B020_send_attack_order`)

Two-tier scoring system when picking which enemy to attack:

**Tier 1 — Close-range aggro** (within `0x10000` = 65536 world units ≈ 4 tiles):
```
for each enemy in enemy_list:
    skip if destroyed or ai_strategic_value == 0
    dx = abs(enemy.x - squad_center_x)
    dy = abs(enemy.y - squad_center_y)
    if dx < 0x10000 AND dy < 0x10000:
        pick closest by manhattan distance (dx + dy)
        lock tier — no tier-2 can override
```

**Tier 2 — Strategic value weighting** (only if no tier-1 target exists):
```
score = (dx + dy) / ai_strategic_value
pick lowest score (high-value targets selected even if far)
```

**Implication**: Nearby enemies always take priority (reactive defense). Distant high-value buildings (Outpost=300, Drillrig=500) get attacked over distant low-value units.

### The Confidence Formula

Each idle squad evaluates whether to ATTACK or MERGE:

$$\text{confidence} = \frac{100 \times (\text{own\_threat} - \text{enemy\_threat})}{\text{own\_threat} + \text{enemy\_threat} + 1}$$

- Range: -100 (vastly outgunned) to +100 (no enemies)
- Default threshold: `-50` (attack even when at significant disadvantage)
- Squad attacks if ANY of:
  - `confidence >= threshold` (-50)
  - `base_area_threat > 0` (base under attack → emergency attack regardless)
  - Drillrig defense needed (`v164` flag)

**Attack confidence -50 means**: squad attacks unless enemy area threat is >3× own threat. Very aggressive AI.

### Override Conditions (Always Attack)
```c
if (base_threatened || drillrig_needs_defense || confidence >= -50):
    → ATTACK nearest valuable target
else:
    → MERGE: find nearest idle squad, move toward it, combine forces
```

### Squad Lifecycle State Machine

```
[Production] → attacker_unassigned
    ↓ (rally point exists)
attacker_rallying → (arrive at rally) → join rally_squad
    ↓ (squad threat >= squad_threshold=200)
squad_idle
    ├── confidence OK → ATTACK → stays idle (re-evaluates next tick)
    ├── confidence LOW → MERGE → squad_marching (toward nearest idle)
    │       ↓ (within 0x4000)
    │   members merge into target squad → original freed
    └── patrol available → squad_patrolling
            ↓ (no threat + near destination)
        → squad_idle
```

---

## Threat Evaluation System

### `ai_get_threat_in_area(ai, radius, x, y)`
Square area check (NOT circular):
```
threat = sum(stats->ai_threat_weight) for all enemies within ±radius manhattan of (x,y)
```
Excludes destroyed units and those with `ai_threat_weight == 0`.

### Patrol Waypoint Assessment (`ai_40B230`)

**Phase A — Base defense**:
Sum enemy threat within base bounding box ± 49152 (3 tiles buffer).

**Phase B — Patrol hotspot detection**:
```
for each of 4 waypoints:
    score = 0
    for each enemy within 81920 radius:
        if mobile (speed > 0): score += threat_weight
        if building (speed == 0): score -= threat_weight >> 1
    pick waypoint with highest score (ties random)
```

**Key insight**: Mobile enemies increase patrol urgency. Enemy buildings *reduce* patrol score (by half weight). AI focuses patrols on areas with mobile threats, not static defenses.

---

## Economic Strategy

### Production Priority (descending)
1. **Tanker** — absolute priority. If any drillrig needs a tanker, produce one before anything else.
2. **Scripted queue** (`_ai_controller_318`) — level-designer-specified fixed production list
3. **Build order rotation** (`_ai_controller_274`) — circular CPLC-defined queue from Outpost/Clanhall

### Build Order Queue Source

Populated when an Outpost/Clanhall is registered. CPLC spawn params contain a linked list of child nodes, each with a `creature_id` at offset +24. These are transcribed into the circular `Ai_stru26C` linked list.

The AI cycles through this queue endlessly (circular). Each entry's `creature_id` is resolved via `g_464DD0[]` (a linear lookup table of `{creature_id, unit_type}` pairs terminated by `TaskType_Invalid`) to determine the actual `UnitType` to produce.

### `g_464DD0` — Creature ID Mapping Table

```c
struct KKND::_ai_stru57 {
    KKND::TaskType creature_id;  // CPLC script identifier
    KKND::UnitType unit_type;    // game unit type
};
```
Linear search table. Maps level-designer creature script IDs to engine UnitTypes.

### Cost Reduction Formula

$$\text{effective\_cost} = \frac{\text{cost\_factor} \times \text{base\_cost}}{256}$$

Where `cost_factor`:
- Demo mode: `g_ai_unit_cost_reductions[g_difficulty_mult]`
- Campaign: `g_lvl_desc[level].superlvl_ai_cost_reduction`

Factor of 256 = full price. Factor of 128 = half price. Minimum effective cost = 10.

**Towers**: Always use `cost >> 2` (quarter price, bypasses the cost_factor system).

### Production Timing
```
production_time_ticks = max(60 * g_unit_stats[type].production_time, 1)
cost_per_tick = (effective_cost << 8) / production_time_ticks
```
Factory "pays off" the unit at this bandwidth rate. When `remaining_cost <= 0`, unit spawns.

---

## Building Placement Strategy

### When to Build
- Cash > 0
- No current construction task active
- Placement queue (`_ai_controller_2EC`) has entries

### Safety Check
```c
threat = ai_get_threat_in_area(ai, 0x10000, location_x, location_y);
if (threat >= GIANT_SCORPION_THREAT_WEIGHT  // 50
    && location_strategic_value < POWER_STATION_STRATEGIC_VALUE)  // 120
{
    SKIP this location  // too dangerous for low-value building
}
```
High-value structures (strategic_value ≥ 120) bypass the safety check — AI will build Power Stations and above even in contested areas.

### Construction Stages
Building completion progresses through 4 visual stages based on remaining cost:
```
remaining_ratio = (remaining_cost << 8) / base_cost   // 0-256 scale

> 171 (>66% left):  Stage 0 — just foundations
85-171 (33-66%):    Stage 1 — partial structure
< 85 (<33%):        Stage 2 — nearly complete
== 0:               Stage 3 — operational
```

---

## Airstrike Target Selection (`ai_40B490`)

Density heatmap algorithm:

1. **Grid**: Map divided into 0x4000-unit (1 tile) cells
2. **Stamp enemies** with 3×3 kernel (±1 cell around position):
   - **Buildings**: weight = `(rand() & 7) + 1` (random 1-8, effectively noise)
   - **Mobile units**: weight = `(ai_strategic_value + ai_threat_weight + 10) >> 2`
3. **Max cell wins**: linear scan for highest accumulated value
4. **Result**: world coordinates of densest cell

**Key insight**: Buildings are de-prioritized via randomization (1-8 weight vs 20+ for combat units). Airstrikes target **clusters of mobile combat units**, not static buildings.

---

## AI Threat/Value Reference Table

| Unit | threat_weight | strategic_value | Notes |
|------|:---:|:---:|-------|
| Rifleman | 10 | 10 | Baseline |
| Berserker | 12 | 12 | |
| Flamer/SWAT | 15 | 15 | |
| Sapper | 20 | 20 | |
| RPG Launcher | 30 | 30 | |
| Sniper | 40 | 40 | |
| Giant Scorpion | 50 | 50 | Threat threshold for "dangerous area" |
| Anaconda Tank | 60 | 60 | |
| ATV Flamethrower | 70 | 70 | |
| War Mastodon | 80 | 80 | |
| Barrage Craft | 80 | 80 | |
| Guard Tower | **100** | 100 | High threat (defensive) |
| Autocannon/Missile Crab | **120** | 100 | Strongest combat units |
| Wasp/Bomber | **120** | 100 | Air units |
| El Presidente/Saboteur | 10 | **100** | Low threat, high strategic (priority target) |
| Power Station | 0 | **120** | Building placement threshold |
| Oil Tanker | 0 | **200** | Economic target |
| Mobile Outpost | 0 | **200** | |
| Machine Shop | 0 | **200** | |
| Outpost/Clanhall | 0 | **300** | High-priority target |
| Drill Rig | 0 | **500** | Highest value target |
| Mobile Derrick | 0 | **500** | |

**Pattern**: Combat units have threat ≈ strategic value. Economic/production buildings have zero threat but very high strategic value (AI targets them from afar via tier-2 scoring).

---

## Difficulty Scaling

| Factor | Effect |
|--------|--------|
| `cost_reduction_factor` (per level/difficulty) | Lower = cheaper AI units = faster army buildup |
| `ai_starting_cash` (per level) | Higher = AI starts richer |
| `num_ai_players` | Army cap = `549/(N+1)` — more AIs = smaller individual armies |
| `squad_threshold` (200 general, 4 mission) | Lower = AI attacks with smaller groups more often |
| `attack_confidence` (-50) | Lower = AI attacks even when outgunned (very aggressive) |
| Tower cost `>> 2` | Towers always cost 1/4 for AI (anti-rush defense) |
| Airstrike config | Per-level count, interval, and randomization range |

### Effective Behaviors by Difficulty
- **Easy**: High cost factor (≈256) → slow production. AI armies smaller and rarer.
- **Hard**: Low cost factor (≈128 or less) → units produced at half price or less. AI floods with units.
- **Mission AI** (squad_threshold=4): Splits squads immediately (every 4 threat = 1 rifleman). Sends constant small raids.

---

## Drillrig Defense Logic

```
for each drillrig with guard_squad:
    threat = ai_get_threat_in_area(81920, drillrig.x, drillrig.y)
    if threat == 0:
        release guard squad → idle (will attack elsewhere)
        if tanker_count < desired_tankers:
            flag drillrig_needing_tanker
    else:
        keep guard squad assigned (defend)
```

Desired tanker count = `distance_to_powerplant / 51200`, clamped [1, 3]. Farther drillrigs get more tankers (longer round-trip needs more tankers to maintain throughput).

---

## Wanderer AI (Pre-Placed Units)

Units placed by level designer with `cplc_spawn_param` become "wanderers" — independent of the squad system.

### Activation Logic
```
for each pending wanderer:
    decrement cplc_spawn_param
    if (param is odd) AND (enemy within 51200):
        reset param to 1  // triggers next tick
    if param == 0:
        move to active list (skip Warlords)

for each active wanderer:
    if unit is idle (EventHandler_Passive):
        target = ai_find_nearest_enemy(manhattan distance)
        send TaskMessage_AttackOrder
```

**Odd countdown trick**: When the countdown is on an odd number and an enemy is nearby, the timer is forced to 1 — meaning it activates next tick. This creates "proximity activation" for pre-placed ambush units.

---

## Complete Decision Flowchart (Per AI Tick — 60 frames)

```
┌─────────────────────────────────────────────────────────┐
│                    AI TICK START                          │
├──────────────────────────────────────────────────────────┤
│                                                          │
│  1. TANKER ASSIGNMENT                                    │
│     Any drillrig needs tanker? → Find idle tanker → Assign│
│                                                          │
│  2. DRILLRIG DEFENSE                                     │
│     For each guarded drillrig:                           │
│       threat==0? → Release guard squad, check tankers    │
│       threat>0?  → Keep defending                        │
│                                                          │
│  3. PRODUCTION PRIORITY                                  │
│     ┌── Drillrig needs tanker? → Produce tanker          │
│     └── Scripted queue? → Produce scripted unit          │
│         └── Build order rotation → Produce next in cycle │
│                                                          │
│  4. AIRSTRIKE (super-levels only)                        │
│     Timer <= 0? → Density heatmap → Spawn bomber at max  │
│                                                          │
│  5. BUILDING PLACEMENT                                   │
│     Cash > 0 + no active construction?                   │
│       → Evaluate placement queue                         │
│       → Skip if threat ≥ 50 AND value < 120             │
│       → Start construction (4 stages over time)          │
│                                                          │
│  6. SQUAD POSITION UPDATE                                │
│     Compute centroids, area threats, track max           │
│                                                          │
│  7. RALLY MANAGEMENT                                     │
│     Unassigned → rallying → arrive → join rally squad    │
│     Rally squad full? → Split to idle/patrol             │
│                                                          │
│  8. SQUAD ATTACK DECISIONS                               │
│     ┌─ Marching: near target? → Merge into target squad  │
│     ├─ Patrolling: safe + arrived? → Return to idle      │
│     └─ Idle:                                             │
│         confidence = 100*(own-enemy)/(total+1)           │
│         ├── ≥-50 OR base_threatened → ATTACK             │
│         │   (pick by distance if close, value if far)    │
│         └── <-50 → MERGE with nearest idle squad         │
│                                                          │
│  9. WANDERER PROCESSING                                  │
│     Countdown pending → activate → attack nearest        │
│                                                          │
│  SLEEP(60 frames)                                        │
└──────────────────────────────────────────────────────────┘
```
