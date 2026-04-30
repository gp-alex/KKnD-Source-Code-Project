# KKND Minimap & Fog of War System

## Overview

KKND implements a tile-based fog of war with a minimap overlay. The minimap is rendered as a downscaled pixel representation of the map where each map tile = 2×2 pixels. Fog of war uses a 16-tile transition tileset for smooth edges between revealed and unrevealed areas.

---

## Architecture

```
┌─────────────────────────────────────────────────────────────┐
│ g_fog_of_war_scrl (MapdScrlImage)                           │
│   - tile grid: (map_tiles_x + 4) × (map_tiles_y + 4)       │
│   - each cell holds pointer to one of 16 fog tile sprites   │
│   - tile0 = fully revealed (transparent)                    │
│   - tile1 = fully shrouded (opaque)                         │
│   - tiles 2-15 = edge transitions (N/S/E/W + corners)      │
└─────────────────────────────────────────────────────────────┘
           │ rendered as scroll layer via g_fog_of_war RenderNode
           │
┌──────────────────────────────────────────────────────────────┐
│ Minimap (pixel buffer)                                       │
│   - g_minimap_revealed_pixels: true terrain colors           │
│   - g_minimap_unrevealed_pixels: working buffer (displayed)  │
│   - size: (2 * map_tiles_x) × (2 * map_tiles_y) pixels      │
│   - palette-indexed 8-bit                                    │
│   - unit dots drawn per-frame based on visibility            │
└──────────────────────────────────────────────────────────────┘
```

---

## Fog of War Tile Semantics

The fog layer has a 16-tile tileset loaded from the MAPD section. Stored as pointers in globals:

| Variable | Tile ID | Meaning |
|----------|---------|---------|
| `dword_47CFC0` | 0 | Fully revealed (NULL/transparent) |
| `g_fog_of_war_tile1` | 1 | Fully shrouded (opaque black) |
| `g_fog_of_war_tile2` | 2 | Edge: top |
| `g_fog_of_war_tile3` | 3 | Edge: right |
| `g_fog_of_war_tile4` | 4 | Edge: bottom |
| `g_fog_of_war_tile5` | 5 | Edge: left |
| `g_fog_of_war_tile6` | 6 | Corner: top-left (outer) |
| `g_fog_of_war_tile7` | 7 | Corner: top-right (outer) |
| `g_fog_of_war_tile8` | 8 | Corner: bottom-right (outer) |
| `g_fog_of_war_tile9` | 9 | Corner: bottom-left (outer) |
| `g_fog_of_war_tile10` | 10 | Corner: top-left (inner) |
| `g_fog_of_war_tile11` | 11 | Corner: top-right (inner) |
| `g_fog_of_war_tile12` | 12 | Corner: bottom-right (inner) |
| `g_fog_of_war_tile13` | 13 | Corner: bottom-left (inner) |
| `g_fog_of_war_tile14` | 14 | Combination tile (varies) |
| `g_fog_of_war_tile15` | 15 | Combination tile (varies) |

**Note**: The decompiler created individual globals for each tile pointer (`g_fog_of_war_tile0` through `g_fog_of_war_tile15`). These are at addresses 47CB28–47CBB8. In reality, this is likely an array `g_fog_tiles[16]`.

---

## Key Functions

### minimap_init (kknd.c:70932) — `MINI_init`

**300-line initialization.** Does:

1. Finds "MAPD" section in level hunk
2. Creates fog of war scroll layer via `MAPD_Draw(MenuId_Multiplayer, 0, 0x10000000)` — the fog overlay is stored as a MAPD entry
3. Allocates `g_fog_of_war_scrl`: custom `MapdScrlImage` struct with `(map_x+4) × (map_y+4)` tile slots
4. Copies all 16 tile pointers from the MAPD source into globals
5. Initializes ALL fog tiles to `g_fog_of_war_tile1` (fully shrouded)
6. Sets `dword_47CFC0 = 0` (the "revealed" sentinel value)
7. Calculates minimap dimensions: `width = 2 * map_tiles_x`, `height = 2 * map_tiles_y`
8. Allocates `g_minimap_revealed_pixels` — the TRUE terrain colors (never changes after init)
9. Allocates `g_minimap_unrevealed_pixels` — working buffer, starts filled with `dword_47CB4C` (shroud color = first pixel of tile1)
10. Allocates `dword_47CB8C` — minimap border frame buffer (width+4 × height+4) with decorative border bytes (0xA6, 0xA0, 0x93)
11. **Builds terrain preview**: For each 2×2 minimap pixel block, samples the corresponding 16×16 terrain tile, finds the most common palette index (mode), stores in `g_minimap_revealed_pixels`
12. Creates minimap entity with `sub_44A500` as its coroutine task (click handler)
13. Sets entity render handler to `render_state_handler_ui`
14. Starts with render flags `|= 0x40000000` (hidden)

### unit_reveal_fog_of_war (kknd.c:71476) — `FOG_reveal_around_unit`

**~600 lines. The core reveal function.** Called every frame for each player unit.

**Algorithm**:
1. Only runs for `unit->player_num == g_player_num` (local player only)
2. Calculates unit position in minimap coordinates: `mx = 2 * (entity->x >> 13)`, `my = 2 * (entity->y >> 13)`
3. Calculates reveal radius from `unit->stats->view_range`: `radius = max(4, view_range >> 5) * 2`
4. **Phase 1 — Minimap pixel copy**: Copies `g_minimap_revealed_pixels` → `g_minimap_unrevealed_pixels` in a rectangular area `[mx-radius, my-radius] to [mx+radius, my+radius]` (clamped to bounds). This reveals the terrain colors in the working buffer.
5. **Phase 2 — Fog tile update**: Converts to fog tile coordinates, then walks the **border** of the reveal rectangle setting appropriate edge/corner transition tiles. The interior tiles are set to `dword_47CFC0` (revealed/transparent).
6. **Edge logic**: For each border tile, examines its neighbor tiles (above, left, right, below) to determine the correct transition tile. This creates smooth fog edges. The logic handles all 16 combinations of adjacent revealed/shrouded tiles.
7. Calls `sub_44BC80(row_start, row_end)` at the end to mark affected scroll tiles dirty.

**Key insight**: Fog is **permanent reveal** — once revealed, never re-shrouds. No "fog of war regrow" mechanic. This matches KKND's gameplay (shroud, not true fog).

### sub_44BC80 (kknd.c:72181) — `FOG_mark_tiles_dirty`

Marks fog scroll tiles as needing redraw. Iterates affected tile range in both `node[0]` and `node[1]` scroll layers, clears bit 2 on each tile's flags (`&= ~2u`). This triggers the renderer to re-blit those tiles.

### sub_44BD50 (kknd.c:72240) — `FOG_mark_all_tiles_dirty`

Marks ALL fog tiles dirty (sets bit 2: `|= 2u`). Called when fog needs full redraw (e.g., after load).

### sub_44B0D0 (kknd.c:71466) — `FOG_is_tile_revealed`

```c
BOOL __fastcall sub_44B0D0(int x, int y)
{
    return fog_tile_at(x >> 13, y >> 13) != g_fog_of_war_tile1;
}
```

Checks if a world position is NOT fully shrouded. Used by cursor code to prevent clicking on shrouded areas.

**Rename**: `FOG_is_position_visible`

### sub_44A500 (kknd.c:70751) — `MINI_click_handler_task`

Coroutine task for the minimap entity. Handles click-to-scroll:
1. Yields each frame waiting for messages
2. On `TaskMessage_UnitSelected`: reads mouse position, converts relative to minimap entity position → world camera coordinates
3. Clamps camera to map bounds
4. Sets `g_mapd_camera.x/y` to scroll the viewport
5. Updates `dword_47CB68/6C` = camera position in minimap tile coords

### minimap_open (kknd.c:70807) — Shows minimap

```c
g_minimap_entity->rend->flags &= ~0x40000000u;  // unhide
g_minimap_entity->shape = &off_4705A8;           // enable click hitbox
```

### minimap_close (kknd.c:70817) — Hides minimap

```c
g_minimap_entity->rend->flags |= 0x40000000u;   // hide
g_minimap_entity->shape = nullptr;               // disable click
```

### enable_minimap (kknd.c:67946) — `MINI_unlock`

Called when Outpost/Clanhall reaches upgrade level that unlocks minimap. Sends `TaskMessage_1514_sidebar` to `g_sidebar_button_minimap->task` to make the button clickable.

### minimap_disable (kknd.c:67961) — `MINI_disable`

Closes minimap and sends `TaskMessage_SidebarRefreshOptions` to refresh sidebar state.

### sub_44AE30 (kknd.c:71257) — `MINI_redraw`

Per-frame minimap update. Called from `sub_44C4B0` (unit anchor update loop).

1. Copies `g_minimap_unrevealed_pixels` → border frame buffer (display copy)
2. Gets tech level to determine what units to show:
   - **Level 1-2**: Only shows player's own units (dots in player color)
   - **Level 3-5**: Shows ALL units (enemy units visible on minimap)
3. Iterates `g_unit_list_head`, converts each unit's world position to minimap coords
4. Draws 2×2 pixel dot in `byte_47CB50[player_num]` color (palette index per player)
5. Only draws if the pixel is NOT shroud color (`dword_47CB4C`) — respects fog

**Gameplay implication**: Higher Outpost/Clanhall level = more minimap intelligence.

### sub_44A6B0 (kknd.c:70826) — `MINI_set_position`

Positions the minimap entity on screen: `x = (screen_x - minimap_width - 4) << 8`

### sub_44A700 (kknd.c:70840) — `MINI_send_hover_to_unit`

Called when hovering minimap. Finds unit under cursor via `sub_447310()`, sends UnitSelected/UnitDeselected + MouseHover messages to show tooltip.

### sub_44BC00 (kknd.c:72168) — `MINI_cleanup`

Frees all minimap resources: fog render node, minimap entity, pixel buffers, fog scroll struct.

### GAME_load_fog_of_war (kknd.c:70863) — `FOG_load_state`

Restores minimap from save file. Iterates fog tile grid — for each tile that equals `dword_47CFC0` (revealed), copies the revealed terrain pixels into the unrevealed buffer. Otherwise fills with shroud color.

### sub_44C4B0 (kknd.c:72690) — `GAME_update_unit_anchors_and_minimap`

Called from main game loop. Updates ALL unit mobd anchor points (turret positions etc.), then calls `MINI_redraw()`. Not purely minimap — mixed responsibility.

---

## Global Variables

### Fog of War Layer

| Address | Current Name | Rename | Type | Purpose |
|---------|-------------|--------|------|---------|
| 47CBB0 | `g_fog_of_war` | keep | `MapdRenderNode*` | The fog overlay render node |
| 47CB34 | `g_fog_of_war_scrl` | keep | `MapdScrlImage*` | Custom fog tile grid (allocated) |
| 47CB30 | `g_fog_of_war_scrl_2` | `g_fog_of_war_source` | `MapdScrlImage*` | Original MAPD fog tileset (ROM) |
| 47CB80 | `g_fog_of_war_num_tiles_x` | keep | `int` | = map_tiles_x + 4 |
| 47CB2C | `g_fog_of_war_num_tiles_y` | keep | `int` | = map_tiles_y + 4 |
| 47CBB4 | `g_fog_of_war_tile0` | `g_fog_tiles_base` | `MapdScrlImageTile*` | Pointer to first tile in grid |
| 47CB28–47CBB8 | `g_fog_of_war_tile1..15` | `g_fog_tile[1..15]` | pointers | 16 transition tile sprites |
| 47CFC0 | `dword_47CFC0` | `g_fog_tile_revealed` | `int` | = 0, sentinel for "revealed" state |
| 47CFC8 | `dword_47CFC8[255]` | — | `int[]` | (unrelated — g_angle_to_orientation follows) |

### Minimap Display

| Address | Current Name | Rename | Type | Purpose |
|---------|-------------|--------|------|---------|
| 47CB7C | `g_minimap_width` | keep | `int` | = 2 * map_tiles_x |
| 47CB34 | `g_minimap_height` | keep | `int` | = 2 * map_tiles_y |
| 47CB74 | `g_minimap_revealed_pixels` | keep | `void*` | True terrain colors (immutable after init) |
| 47CB88 | `g_minimap_unrevealed_pixels` | `g_minimap_display_pixels` | `void*` | Working buffer — terrain where revealed, shroud color where not |
| 47CB40 | `g_minimap_entity` | keep | `Entity*` | Minimap UI entity (renderable + clickable) |
| 47CB8C | `dword_47CB8C` | `g_minimap_frame_buffer` | `void*` | Border frame + pixels combined for rendering |
| 47CBAC | `dword_47CBAC` | `g_minimap_frame_pixels_start` | `int` | Offset into frame buffer where pixel data begins |
| 47CB98 | `dword_47CB98` | `g_minimap_current_pixels` | `int` | Pointer to currently displayed pixel buffer |
| 47CB4C | `dword_47CB4C` | `g_fog_shroud_color` | `int` | Palette index of shroud (sampled from tile1 pixel[0]) |
| 47CB68 | `dword_47CB68` | `g_minimap_camera_x` | `int` | Camera position in minimap-tile units |
| 47CB6C | `dword_47CB6C` | `g_minimap_camera_y` | `int` | Camera position in minimap-tile units |
| 47CB50 | `byte_47CB50[]` | `g_minimap_player_colors` | `char[]` | Per-player dot color on minimap (palette index) |

### Sidebar Button

| Address | Current Name | Purpose |
|---------|-------------|---------|
| — | `g_sidebar_button_minimap` | Sidebar button entity for minimap toggle |

---

## Coordinate Systems

```
World coords:      entity->x, entity->y  (fixed-point, <<8)
Tile coords:       world >> 13  (each tile = 8192 world units = 32 pixels << 8)
Minimap coords:    2 * tile = 2 * (world >> 13)
Fog grid coords:   tile + 2  (fog grid is padded by 2 on each side)
```

Conversions:
- World → minimap pixel: `mx = 2 * (entity->x >> 13)`
- World → fog tile: `fx = (entity->x >> 13) + 2`
- Minimap pixel → fog tile: `fx = mx/2 + 2`

The fog grid is 4 tiles larger than the map (2 on each side) to handle edge transitions cleanly.

---

## Data Flow Per Frame

```
1. Each unit calls unit_reveal_fog_of_war():
   - Copies revealed terrain into g_minimap_unrevealed_pixels (rectangle)
   - Updates fog tile transitions on border
   - Marks dirty tiles for re-render

2. sub_44C4B0 (main loop) calls sub_44AE30 (MINI_redraw):
   - Copies g_minimap_unrevealed_pixels → frame buffer
   - Draws unit dots on top
   - (Frame buffer is what the render_state_handler_ui displays)

3. Fog overlay (g_fog_of_war) is rendered as a scroll layer:
   - Covers the entire game viewport
   - Tiles reference one of 16 sprites (edge transitions)
   - Revealed areas have tile=0 (transparent)
   - sub_448390 is the custom render state handler for the fog layer
```

---

## Function Rename Summary

| Current | Address | Rename |
|---------|---------|--------|
| `sub_44A500` | 44A500 | `MINI_click_handler_task` |
| `sub_44A6B0` | 44A6B0 | `MINI_set_position` |
| `sub_44A700` | 44A700 | `MINI_send_hover_to_unit` |
| `sub_44AE30` | 44AE30 | `MINI_redraw` |
| `sub_44B0D0` | 44B0D0 | `FOG_is_position_visible` |
| `sub_44BC00` | 44BC00 | `MINI_cleanup` |
| `sub_44BC80` | 44BC80 | `FOG_mark_tiles_dirty` |
| `sub_44BD50` | 44BD50 | `FOG_mark_all_tiles_dirty` |
| `sub_44C4B0` | 44C4B0 | `GAME_update_unit_anchors_and_minimap` |
| `sub_448390` | 448390 | `FOG_render_state_handler` |
| `minimap_init` | 44A840 | `MINI_init` |
| `minimap_open` | 44A650 | `MINI_open` |
| `minimap_close` | 44A680 | `MINI_close` |
| `minimap_disable` | 447050 | `MINI_disable` |
| `enable_minimap` | 447030 | `MINI_unlock` |
| `unit_reveal_fog_of_war` | 44B100 | `FOG_reveal_around_unit` |
| `GAME_load_fog_of_war` | 44A780 | `FOG_load_state` |

---

## Design Notes

1. **Shroud, not fog**: KKND uses permanent reveal (like StarCraft's shroud). Once a tile is revealed, it stays revealed forever. There's no re-fogging when units leave.

2. **Minimap visibility tied to tech level**: At Outpost/Clanhall level 1-2, minimap only shows YOUR units. At level 3+, shows ALL units. This is a game mechanic reward for upgrading.

3. **The fog overlay is a MAPD scroll layer**: Clever reuse of the existing tile-scroll rendering system. The fog "image" is just a grid of tile pointers that get swapped between 16 possible edge tiles as areas are revealed.

4. **2:1 minimap to map-tile ratio**: Each map tile becomes 2×2 minimap pixels, computed at init time by finding the dominant color in each 16×16 tile quadrant.

5. **Border frame**: The minimap has a decorative pixel border (palette indices 0xA6, 0xA0, 0x93) drawn into a surrounding frame buffer, making it look like a UI panel.

6. **Click-to-scroll**: The minimap entity is a coroutine that listens for `TaskMessage_UnitSelected` (click). On click, it converts the click position relative to minimap bounds into a camera scroll target.

---

## Decompilation Issues

1. **`dword_47CFC0` = 0 always**: This is the "revealed" sentinel. IDA created a global for it, but it's just `NULL`/`0`. Could be `#define FOG_TILE_REVEALED 0`.

2. **16 individual tile globals**: `g_fog_of_war_tile1` through `g_fog_of_war_tile15` are consecutive in memory (47CB28-47CBB8). Should be a single array `g_fog_tiles[16]`.

3. **`sub_44C4B0` mixed responsibility**: Updates unit anchors (turret positions) AND redraws minimap. These are logically separate operations bundled for efficiency (single unit list walk).

4. **`node[0]` / `node[1]` usage in FOG_mark_tiles_dirty**: These are the two scroll layers of the main map view. The fog dirty-marking operates on both layers simultaneously — the map has dual-buffer scrolling.

5. **`enable_minimap` name is misleading**: It doesn't enable/show the minimap — it unlocks the sidebar button that ALLOWS the player to open it. Better name: `MINI_unlock`.
