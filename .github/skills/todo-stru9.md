# _stru9_unit_order — Player Selection Context

## Identity

`_stru9_unit_order` is the **per-player unit selection and command dispatch context**. One per active player. Holds:
- Circular doubly-linked list of selected units (using `_stru9_unit_order_2` nodes)
- Pre-allocated pool of 100 selection nodes
- Pending attack/move order payloads
- Player number and "owns selection" flag

Stored as `task->ctx` on the per-player `TaskChannel_UnitLifecycle` task.

---

## Struct Layout

### _stru9_unit_order — PlayerSelectionContext (0x34 = 52 bytes)

| Offset | Type | Current Name | Proposed Name | Role |
|--------|------|-------------|---------------|------|
| 0x00 | `_stru9_unit_order*` | `next` | `selection_head` | First selected unit node (sentinel = self when empty) |
| 0x04 | `_stru9_unit_order*` | `prev` | `selection_tail` | Last selected unit node (sentinel = self when empty) |
| 0x08 | `Task*` | `_stru9_unit_task_field_8` | `_unused_08` | Not accessed in traced code |
| 0x0C | `_stru9_unit_order_2*` | `_stru9_unit_task_pool` | `node_pool` | Base of 100-node pool allocation |
| 0x10 | `_stru9_unit_order_2*` | `_stru9_unit_task_head` | `node_free` | Free list head |
| 0x14 | `Task*` | `_stru9_unit_task_field_14` | `owner_task` | The `g_47CAF0[player]` task |
| 0x18 | `int` | `player_num` | `player_num` | Player index (0–6) |
| 0x1C | `int` | `_stru9_unit_task_field_18` | `has_own_units` | 1 if any selected unit belongs to this player |
| 0x20 | `AttackOrderPayload` | `attack_order` | `attack_order` | {player_num, target_unit*} — pending attack command |
| 0x28 | `AttackOrderPayload` | `move_order` | `move_order` | **BUG: should be `MoveOrderPayload`** {player_num, dst_x, dst_y} |
| 0x30 | `int` | `_stru9_unit_task_field_30` | (part of move_order.dst_y) | Belongs to move_order if retyped |

**Total**: 0x34 bytes. Confirmed by `TASK_Alloc(task, 0x34u)`.

### _stru9_unit_order_2 — SelectionNode (0x0C = 12 bytes)

| Offset | Type | Current Name | Proposed Name | Role |
|--------|------|-------------|---------------|------|
| 0x00 | `_stru9_unit_order_2*` | `next` | `next` | Next node (selection or free chain) |
| 0x04 | `_stru9_unit_order*` | `_stru9_unit_task_2_field4` | `prev` | Previous node (polymorphic: points to another node OR the sentinel) |
| 0x08 | `Task*` | `_stru9_unit_task_2_field8` | `unit_task` | Task* of selected unit |

**Pool**: 100 nodes × 12 bytes = 0x4B0. Confirmed by `TASK_Alloc(task, 0x4B0u)`.

---

## Decompilation Bugs

### 1. `move_order` typed as `AttackOrderPayload` — should be `MoveOrderPayload`

**Evidence**: In `GameEvent_MoveCommand`:
```c
ctx[11] = *(_DWORD *)v2->payload;      // dst_x
ctx[12] = *(_DWORD *)&v2->payload[4];  // dst_y
TASK_send_message(ctx[5], TaskMessage_MoveOrder, ctx + 10, ...);
```
Handler `unit_xl_on_move(unit, payload)` accesses `payload[0]` = player_num, `payload[1]` = dst_x, `payload[2]` = dst_y → `MoveOrderPayload` (12 bytes).

`AttackOrderPayload` is `{int player_num; Unit* target}` (8 bytes) — wrong type, wrong size.

**Fix**: Change `move_order` from `AttackOrderPayload` to `MoveOrderPayload`. Then `_stru9_unit_task_field_30` disappears (absorbed as `dst_y`).

### 2. `_stru9_unit_order_2.prev` typed as `_stru9_unit_order*`

Field +4 is `prev` in a doubly-linked list. For the head node it points to the sentinel (`_stru9_unit_order*`), but for interior nodes it points to `_stru9_unit_order_2*`. Decompiler picked the sentinel type. Should be `void*` or use a union.

### 3. `next/prev` on `_stru9_unit_order` typed as self-pointers

Fields +0x00/+0x04 are typed `_stru9_unit_order*` but actually hold `_stru9_unit_order_2*` nodes (or self-sentinel). The sentinel trick: `head == &sentinel` means list is empty. This is the standard intrusive circular list pattern where the sentinel's link fields are compatible with the node type.

---

## Globals

| Current | Proposed | Purpose |
|---------|----------|---------|
| `g_47CAF0[7]` | `g_player_selection_tasks[7]` | Per-player lifecycle tasks (one per active player) |
| `g_47CAE0` | `g_current_game_event` | Current game event being processed (single-player) |
| `byte_47CA80[96]` | `g_mp_event_buffer` | Per-player event buffer for multiplayer (7 × 13 bytes) |

---

## Functions

| Current | Address | Proposed | Purpose |
|---------|---------|----------|---------|
| `UNIT_tasks_init` | 0x449670 | `player_selection_init` | Alloc per-player selection context + node pool |
| `sub_448F30` | 0x448F30 | `player_event_dispatch` | Process GameEvents: box select, click select, move/attack commands, etc. |
| `MessageHandler_448EF0` | 0x448EF0 | `player_selection_on_unit_deselected` | Remove single unit from selection list |
| `sub_44C970` | 0x44C970 | `selection_box_begin` | Set bounding box for box-select iteration |
| `sub_44C9A0` | 0x44C9A0 | `selection_box_next_unit` | Iterator: next unit in box |
| `sub_44CA30` | 0x44CA30 | `find_unit_by_id` | Linear search unit list by unit_id |
| `sub_449800` | 0x449800 | `player_selection_cleanup` | Terminate all per-player tasks |
| `sub_449820` | 0x449820 | `netz_event_sync_loop` | Multiplayer lockstep event synchronization |

---

## Selection List Operations

### Empty State
```
sentinel.next = &sentinel  (self-referential)
sentinel.prev = &sentinel
```

### Add Node (box select / single select)
```c
node = free_head;                    // pop from free list
free_head = node->next;
node->unit_task = unit->task;        // set payload
node->next = sentinel.next;          // insert at head
node->prev = &sentinel;
sentinel.next->prev = node;
sentinel.next = node;
```

### Remove Node (unit deselected/destroyed)
```c
// search for node where node->unit_task == sender
node->next->prev = node->prev;
node->prev->next = node->next;
node->next = free_head;              // return to free list
free_head = node;
```

### Clear All (deselect all / recall control group)
```c
sentinel.prev->next = free_head;     // bulk-free: link tail to free chain
free_head = sentinel.next;           // old head becomes new free head
sentinel.next = &sentinel;           // empty sentinel
sentinel.prev = &sentinel;
```

---

## Event Dispatch Flow

```
Input (mouse/keyboard)
  ↓
Cursor system creates GameEvent → g_current_game_event / g_mp_event_buffer
  ↓
sub_448F30 (player_event_dispatch) reads event each tick:
  ├─ SelectedBox → box iterate → add nodes → selection list grows
  ├─ SelectedUnit → clear + add single node
  ├─ SelectionCleared → bulk-free all nodes
  ├─ MoveCommand → iterate selection → send TaskMessage_MoveOrder to each
  ├─ AttackCommand → iterate selection → send TaskMessage_AttackOrder to each
  ├─ AssignedControlGroup → tag units with group number
  └─ RecalledControlGroup → clear + rebuild from tagged units

Unit death/removal
  → sends TaskMessage_UnitDeselected
  → MessageHandler_448EF0 removes node from selection
```

---

## Design Insights

1. **Sentinel-based circular doubly-linked list**. `_stru9_unit_order` itself IS the sentinel — its `next/prev` fields double as list head/tail. Empty = self-referential. Classic C intrusive list pattern.

2. **Node pool is per-player, per-task-allocation**. 100 nodes via `TASK_Alloc` — allocated from the task's memory arena, not from a global pool. Freed when the task terminates. Max 100 simultaneously selected units per player.

3. **Selection nodes store `Task*` not `Unit*`**. This is intentional — commands are dispatched via `TASK_send_message` which needs the task pointer. The unit is accessible via `task->ctx` on the receiving end.

4. **Move vs Attack payloads at fixed offsets**. `ctx+8` = attack payload, `ctx+10` = move payload. The event dispatcher fills these in, then passes the pointer to `TASK_send_message` as the `payload` arg. Units read the fields directly from the pointer. Zero-copy.

5. **Multiplayer sync**: In multiplayer, events are serialized into 13-byte `GameEvent` structs and exchanged via lockstep. Each player's `sub_448F30` processes events indexed by their player_num into the shared buffer. This ensures all clients process the same events in the same order.

---

## Naming Proposal

| Current | Proposed |
|---------|----------|
| `_stru9_unit_order` | `PlayerSelectionContext` |
| `_stru9_unit_order_2` | `SelectionNode` |
| `_stru9_unit_task_field_8` | `_unused_08` (or remove) |
| `_stru9_unit_task_pool` | `node_pool` |
| `_stru9_unit_task_head` | `node_free` |
| `_stru9_unit_task_field_14` | `owner_task` |
| `_stru9_unit_task_field_18` | `has_own_units` |
| `_stru9_unit_task_field_30` | (absorbed into `move_order.dst_y`) |
| `_stru9_unit_task_2_field4` | `prev` |
| `_stru9_unit_task_2_field8` | `unit_task` |

---

## Open Questions

1. **`_stru9_unit_task_field_8` (offset 0x08)** — Never accessed in traced code. Dead field? Or used in an untraced code path? Typed as `Task*` by decompiler.

2. **Control group storage** — `AssignedControlGroup` accesses `*(BYTE*)(unit + offset)` indexed by player_num. Where exactly on the `Unit` struct are control group tags stored? Appears to be `unit->control_groups[player_num]` (at offset `Unit + 0x290 + player_num`).

3. **100-node limit** — If a player box-selects >100 units, the free list exhausts and remaining units silently don't get selected. No error handling. Is 100 enough for KKND's unit cap?

4. **`has_own_units` flag** — Set to 1 when selection contains at least one of this player's units. Used by cursor system to determine command mode (friendly vs enemy selection). But never cleared per-unit — once set, stays 1 until full clear. Could lead to stale state if all own units deselect individually via `MessageHandler_448EF0` without bulk clear.
