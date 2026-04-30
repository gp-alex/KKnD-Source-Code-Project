# UiString — Text Rendering System

## Identity

`UiString` is a **grid-based text rendering widget**. It allocates a 2D grid of character cells (cols × rows), each backed by a `stru8` → `RenderNode` chain. Characters are displayed by patching MOBD font sprite pointers into render node image fields. Supports word wrapping, scrolling, newlines, and border frames (9-patch style from MOBD corner/edge sprites).

---

## Structs

### UiString (32 bytes, 0x20)
```
Offset  Type                    Field                          Proposed Name
0x00    UiString*               next                           next
0x04    UiString*               prev                           prev
0x08    stru8*                  pstru8                         glyph_nodes
0x0C    int                     _ui_string_field_C             cols
0x10    int                     _ui_string_field_10            rows
0x14    LevelMobdSurface*       mobd_font__see_sub_405A60      font
0x18    int                     _ui_string_field_18            cursor_col
0x1C    int                     num_lines                      cursor_row
```

### stru8 — GlyphNode (8 bytes)
```
Offset  Type                    Field           Proposed Name
0x00    stru8*                  next            next
0x04    RenderNode*             rn              render_node
```

Singly-linked list. Each node owns one RenderNode that displays one character glyph. Total nodes per UiString = `cols × rows` (the grid), plus border cells.

---

## Pool Architecture

### Two separate pools allocated in `sub_445690` (0x445690):

| Pool | Global | Size | Entry Size | Count | Purpose |
|------|--------|------|------------|-------|---------|
| GlyphNode pool | `dword_47C774` | 0x1800 (6144) | 8 bytes | 768 | stru8 nodes (character cells) |
| UiString pool | `dword_47C77C` | 0x200 (512) | 32 bytes | 16 | UiString structs |

### Pool globals
| Current | Proposed | Purpose |
|---------|----------|---------|
| `dword_47C76C` | `g_ui_glyph_free_count` | Free glyph nodes remaining (starts 768) |
| `dword_47C770` | `g_ui_string_initialized` | Init flag (1 = pools allocated) |
| `dword_47C774` | `g_ui_glyph_pool` | GlyphNode pool memory block |
| `dword_47C778` | `g_ui_glyph_free` | GlyphNode free list head |
| `dword_47C77C` | `g_ui_string_pool` | UiString pool memory block |
| `dword_47C780` | `g_ui_string_free` | UiString free list head |
| `dword_47C784` | `g_ui_string_active` | UiString active list head (doubly-linked) |

GlyphNode free list: singly-linked via `next` field (offset 0), stride 8.
UiString free list: singly-linked via `next` field (offset 0), stride 32.

Budget check: `cols × rows > g_ui_glyph_free_count` → reject creation.

---

## Functions

### Core Lifecycle

| Current | Address | Proposed | Purpose |
|---------|---------|----------|---------|
| `sub_445690` | 0x445690 | `ui_string_sys_init` | Alloc both pools, set initial counts |
| `ui_string_create` | 0x445870 | `ui_string_create` | Alloc UiString + N glyph nodes, set up 9-patch border, add to active list |
| `ui_string_free` | 0x445A60 | `ui_string_free` | Mark render nodes deleted (flag 0x80000000), return glyphs+struct to free lists |
| `sub_445B30` | 0x445B30 | `ui_string_sys_cleanup` | Free ALL active UiStrings, free pool memory |

### Text Operations

| Current | Address | Proposed | Purpose |
|---------|---------|----------|---------|
| `sub_445770` | 0x445770 | `ui_string_write_text` | Word-wrap and lay out characters into glyph grid |
| `sub_443D80` | 0x443D80 | `ui_string_append_text` | Append text at cursor position (same layout logic, different entry) |
| `sub_445AE0` | 0x445AE0 | `ui_string_clear` | Reset interior glyph images to default (preserves border) |
| `ui_string_405A60` | 0x405A60 | `ui_string_write_line` | Write characters with fine x/y positioning (credits/subtitle style) |

### Measurement

| Current | Address | Proposed | Purpose |
|---------|---------|----------|---------|
| `ui_string_445C00` | 0x445C00 | `ui_string_count_lines` | Count wrapped lines for text at given column width |
| `ui_string_445C80` | 0x445C80 | `ui_string_measure_width` | Max line width in characters after word wrapping |
| `sub_443D60` | 0x443D60 | `strlen_to_newline` | Characters until `\n` or `\0` |

### Scrolling / Animation

| Current | Address | Proposed | Purpose |
|---------|---------|----------|---------|
| `ui_string_4059C0` | 0x4059C0 | `ui_string_scroll_line_up` | Move one row of glyphs up by 1px; return 1 if scrolled off-screen |
| `ui_string_405A20` | 0x405A20 | `ui_string_set_line_y` | Set Y position for all glyphs in a row |

---

## Font System

Fonts are MOBD surfaces — `LevelMobdSurface` contains `MobdAnimFrame frames[256]` (one per ASCII byte). Each frame holds:
- `sprt->blitter` — the blitter function pointer (from stru2 fixup)
- `x`, `y` — glyph origin offset
- `points[0]` — advance width (kerning data at `points[0].id >> 8`)

Character rendering: lookup `font->frames[char_byte]`, extract image pointer from frame, set on RenderNode's `cmd.image`.

Two font MOBDs used:
- **MOBD 27** — Normal game font (used for unit tooltips, cash display, title screen)
- **MOBD 80** — Menu/dialog font (used for all menu screens, save/load, skirmish options)

---

## 9-Patch Border System

`ui_string_create` allocates `cols × rows` glyph nodes but uses the first/last row and first/last column as **border decoration**, not text cells. The border sprites come from fixed offsets in the font MOBD:

| Position | MOBD offset | Sprite source |
|----------|-------------|---------------|
| Top-left corner | `frames[0].y` | Corner sprite |
| Top-right corner | `frames[0].sprt->blitter` | Corner sprite |
| Bottom-left corner | `frames[0].points[0].id` | Corner sprite |
| Bottom-right corner | `frames[0].points[0].y` | Corner sprite |
| Left edge | `frames[0].flags` | Vertical edge |
| Right edge | `frames[0].sound_id` | Vertical edge |
| Top edge | `frames[0].shape->box` | Horizontal edge |
| Bottom edge | `frames[0].points[0].x` | Horizontal edge |
| Interior cell | `MobdSurface[3]...` | Background fill |

**Decompilation artifact**: These aren't actually "y", "blitter", "flags" etc. — the decompiler is interpreting sequential DWORDs through the `MobdAnimFrame` struct overlay. Really the font MOBD has a sprite sheet with 9-patch elements at fixed indices, accessed by raw offset arithmetic.

**Usable text area** = `(cols - 2) × (rows - 2)` characters (border takes 1 cell on each side).

---

## Global UiString Instances

| Global | Proposed | Context |
|--------|----------|---------|
| `dword_47C65C` | `g_menu_text_area` | Multi-purpose menu text panel — credits, mission objectives, save/load list, skirmish settings, player name, multiplayer lobby |
| `dword_47A730` | `g_chat_input_string` | Multiplayer chat input box |
| `g_sidebar_cash_string` | `g_sidebar_cash_string` | In-game sidebar cash display (already named) |
| `g_4780F8_movie_str` | `g_movie_subtitle_string` | FMV movie subtitle text overlay |

---

## Usage Patterns

### Pattern 1: Static Text Display
```c
str = ui_string_create(viewport, font, x, y, cols, rows, z, char_w, char_h);
ui_string_clear(str);          // sub_445AE0
str->cursor_col = 0;
str->cursor_row = 0;
ui_string_append_text(str, "Hello World", 0);  // sub_443D80
// ... display for a while ...
ui_string_free(str);
```

### Pattern 2: Dynamic Updates (Sidebar Cash)
```c
// Each frame:
g_sidebar_cash_string->cursor_col = 0;
g_sidebar_cash_string->cursor_row = 0;
ui_string_write_text(g_sidebar_cash_string, formatted_cash, 0);  // sub_445770
```

### Pattern 3: Scrolling Credits
```c
str = ui_string_create(...);
// Fill initial visible lines
for each visible_line:
    ui_string_write_line(str, text, 0, y_pos);  // 405A60
// Each tick:
    if (ui_string_scroll_line_up(str, line, 0))  // 4059C0 returns 1 = off screen
        // recycle line: set new y, write next text
        ui_string_set_line_y(str, line, new_y);  // 405A20
        ui_string_write_line(str, next_text, 0, new_y);
```

### Pattern 4: Menu Entity Attachment
```c
str = ui_string_create(...);
menu_register_entity(entity, str, 0, 1, 0);  // sub_4439F0
// entity->ctx = str
// On menu cleanup: sub_443B40 iterates all entities, calls ui_string_free(entity->ctx)
```

---

## Design Insights

1. **Fixed-grid character cells**, not variable-width. Word wrapping counts characters, not pixels. Proportional spacing comes from MOBD glyph advance widths at render time, but layout is grid-based.

2. **No text buffer storage**. UiString stores NO string data — only glyph render nodes with image pointers. To "read back" displayed text, you'd have to inspect render node images. This is why clear+rewrite is the update pattern.

3. **`_ui_string_field_C` = cols, `_ui_string_field_10` = rows** confirmed by:
   - Pool budget: `cols × rows` deducted from `g_ui_glyph_free_count`
   - Word wrap in `sub_445770`: `v8 > a1[3] - a1[6] - 2` (cols - cursor_col - 2 for border)
   - Row overflow: `a1[7] >= a1[4] - 2` (cursor_row >= rows - 2 for border)

4. **Two text-writing functions** with slightly different interfaces:
   - `sub_445770` (0x445770) — main word-wrapping writer, tracks col/row cursor as `a1[6]/a1[7]`
   - `sub_443D80` (0x443D80) — similar but used for appending to existing text position
   - `ui_string_405A60` (0x405A60) — pixel-positioned writer for credits/scrolling (uses `cmd.x/y` directly)

5. **16 UiString max simultaneously** — enough for one main text area + a few option selectors + cash display. Menu screens rarely need more than ~10 concurrent text widgets.

---

## Naming Conventions

| Pattern | Convention |
|---------|-----------|
| `stru8` | `GlyphNode` — each node = one character cell |
| `UiString` | Keep name, or `TextWidget` / `TextGrid` |
| `_ui_string_field_C` | `cols` (grid width in character cells) |
| `_ui_string_field_10` | `rows` (grid height) |
| `_ui_string_field_18` | `cursor_col` (current write column) |
| `num_lines` | `cursor_row` (current write row — "num_lines" is misleading, it's cursor position) |
| `mobd_font__see_sub_405A60` | `font` |
| `pstru8` | `glyph_nodes` (head of glyph linked list) |

---

## Open Questions

1. **`sub_445770` vs `sub_443D80`** — Both do word-wrapped text layout. Are they truly different entry points, or is one a wrapper? The code looks nearly identical. May be same function with different calling conventions (fastcall vs thiscall artifact).

2. **Border sprite mapping** — The 9-patch offsets interpreted through MobdAnimFrame fields are clearly wrong struct overlay. Need to verify: is there a separate "frame border sprite sheet" embedded in the font MOBD, or does `frames[0]` truly hold 9 different pointers at those offsets?

3. **`dword_47C65C` reuse** — This single global is reused across credits, mission objectives, save/load, and multiple menu screens. It's freed between uses. Is there ever a case where two systems try to use it simultaneously?

4. **Scroll behavior** — `ui_string_4059C0` moves one pixel per call. At 60fps and ~200px visible height, a full page scrolls in ~3.3 seconds. Is there variable scroll speed for credits vs menus?

5. **`sub_4439F0` entity attachment** — Stores UiString pointer in `entity[31]` (offset 0x7C = ctx field). Confirm this maps to `Entity._80_attacker_unit_or__stru29__sprite__initial_hitpoints`. If so, that field is a polymorphic `void*` used for:
   - Units: attacker pointer or initial hitpoints
   - Menu entities: UiString context pointer
   - Sprite entities: sub_4439F0 return value
   This supports renaming it to just `ctx` or `context_data`.
