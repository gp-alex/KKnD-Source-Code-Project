# Network System (NETZ) — DirectPlay Multiplayer

## Overview

KKND uses Microsoft DirectPlay (DPLAYX) for multiplayer networking. The NETZ layer wraps DirectPlay with a connection abstraction supporting 3 transport protocols: TCP/IP, IPX, and Serial/Modem. Architecture: host/client model with lockstep synchronization during gameplay.

Build date embedded in player info: **Oct 23 1997, 17:00:00**.

---

## Architecture Layers

```
┌──────────────────────────────────────────────────┐
│ GAME LAYER                                        │
│  KKND_Main → menu loop → gameplay loop            │
│  g_netz_synchronized_lockstep_mode gates entities  │
│  TaskNetzFlags on tasks control MP execution       │
├──────────────────────────────────────────────────┤
│ NETZ ABSTRACTION (sub_42F8xx-sub_42FFxx)          │
│  Init, Host/Join, Send/Receive, Cleanup           │
│  Message protocol: type byte + seq byte + payload │
│  Player slot management (7 slots max)             │
├──────────────────────────────────────────────────┤
│ DIRECTPLAY BACKEND (sub_430xxx)                   │
│  Session enum, Player enum, Send/Receive          │
│  Service provider GUIDs, Connection creation      │
│  DPLAYX_1 (DirectPlayCreate), DPLAYX_2 (EnumSP)  │
└──────────────────────────────────────────────────┘
```

---

## Connection Types

3 transport protocols, indexed 0-2. Each has a 26-int structure starting at `off_468B74`:

| Index | Protocol | Name Strings | GUID Globals |
|-------|----------|--------------|--------------|
| 0 | TCP/IP | "DirectX IPX", "DirectX IPX" | `dword_468CF0..CFC` |
| 1 | IPX | — | `dword_468D10..D1C` |
| 2 | Serial/Modem | "DirectX modem", "DirectX serial" | `dword_468D00..D0C` |

**NOTE**: Index 0 name says "DirectX IPX" but GUID is TCP/IP. Decompiler artifact — the names array `off_468B74` is interleaved with other data. Connection type 0 = TCP/IP based on GUID and `sub_42FB60` logic.

Each connection type struct (26 ints = 104 bytes) contains:
- `[0]`: Capability flag (is this protocol available?)
- `[24]`: Error string callback function pointer (`dword_468BD4`)
- Names at `off_468B74`

---

## Message Protocol

### Wire Format
```
Byte 0: sequence number (wrapping 0-255)
Byte 1: message type
Bytes 2+: payload data
```

Per-player send buffers at `byte_47AAA0 + 288*slot`. Each slot has:
- `byte_47AAA0[288*i]`: Last received sequence
- `byte_47AAA1[288*i]`: Next send sequence
- `byte_47AAA3[288*i]`: Message type
- `unk_47AAA4 + 288*i`: Payload data
- `dword_47ABBC[72*i]`: Message size (payload + 2 header bytes)

### Message Types
| Type | Direction | Purpose |
|------|-----------|---------|
| 30 | Client → All | Join request (broadcast player info) |
| 31 | Host → Clients | Join confirmation with player data |
| ≥ 50 | Game-level | Gameplay messages routed to `dword_47A838` callback |
| < 50 | NETZ-internal | Protocol handshake only |

### Join Handshake Flow
```
Client connects via DirectPlay session
  ↓
Host receives msg type 30:
  → Allocates player slot (sub_430D10)
  → Sends msg type 31 with "PLAYER" + build date to all connected
  ↓
Client receives msg type 31:
  → Host allocates slot, stores player DirectPlay ID
  → Notifies game via dword_47A838 callback
```

### Sequence Number Anti-Replay
Each slot tracks expected sequence. If received seq != expected, message is dropped. This prevents duplicate processing of retransmitted DirectPlay messages.

---

## Player Slot System

7 player slots (indices 0-6), each 11 ints (44 bytes) in parallel arrays:

| Array | Offset | Purpose |
|-------|--------|---------|
| `dword_47A968[11*i]` | +0 | Slot occupied flag (0=free, 1=allocated) |
| `dword_47A940[11*i]` | +0 | Connection state (0=none, 2=connected) |
| `dword_47A964[11*i]` | +0 | DirectPlay player ID for this slot |

`dword_47B3A0` = active player count.

Slot lifecycle:
1. `sub_430D10()` — allocate: find free slot, mark occupied, return index
2. `sub_430D70(slot)` — free: clear occupied flag, decrement count
3. `sub_430CE0()` — reset all: zero entire `dword_47A968[78]`, reset count

---

## Key Functions

### NETZ Abstraction Layer (42F8xx-42FFxx)

| Function | Line | Rename Suggestion | Purpose |
|----------|------|-------------------|---------|
| `NETZ_Init` | 46894 | keep | Init network: store callback, enumerate providers, connect |
| `sub_42F820` | 46888 | `NETZ_Stub` | Stub — returns 0 |
| `sub_42F8C0` | 46934 | `NETZ_HostOrJoin` | Setup as host (create session) or client (prepare to join) |
| `sub_42F8E0` | 46976 | `NETZ_SwitchRole` | Toggle between host/client mode |
| `sub_42F930` | 46994 | `NETZ_ChangeTransport` | Switch connection type (0=TCP, 1=IPX, 2=Serial) |
| `sub_42F980` | 47007 | `NETZ_Disconnect` | Cleanup and set `dword_468B68 = -1` |
| `sub_42F9A0` | 47018 | `NETZ_Shutdown` | Double-cleanup: calls `sub_42FCA0` twice + disconnect |
| `sub_42F9C0` | 47030 | `NETZ_Poll` | Process pending messages + timer events |
| `sub_42F9E0` | 47042 | `NETZ_CreatePlayer` | Create local player, send type-31 join message |
| `sub_42FA00` | 47071 | `NETZ_Send` | Send message to one player or broadcast (-1) |
| `sub_42FAC0` | 47112 | `NETZ_FindProviderByName` | Case-insensitive provider name lookup |
| `sub_42FB10` | — | `NETZ_FB10` | Forward-declared, no body (external/import) |
| `sub_42FB20` | — | `NETZ_FB20` | Forward-declared, no body |
| `sub_42FB30` | — | `NETZ_HostReconnect` | Forward-declared, no body. Called on host reconnect. |
| `sub_42FB40` | — | `NETZ_FB40` | Forward-declared, no body |
| `sub_42FB50` | — | `NETZ_LeaveSession` | Forward-declared, no body. Called during post-mission MP cleanup. |
| `sub_42FB60` | 47144 | `NETZ_SelectProvider` | Find provider by GUID for connection type, activate it |
| `sub_42FCA0` | 47241 | `NETZ_CleanupSession` | Destroy DirectPlay session, release interface, free player/session lists |
| `sub_42FD30` | 47291 | `NETZ_ProcessMessages` | Receive DirectPlay msgs + re-enumerate players, sync lists |
| `sub_42FF10` | 47433 | `NETZ_SendToPlayer` | Build message packet, DirectPlay Send to specific player |
| `sub_42FFB0` | 47461 | `NETZ_ResendLast` | Resend cached message to player |
| `sub_4300C0` | 47483 | `NETZ_RoleSwitch` | Internal: switch between host/client DirectPlay state |

### DirectPlay Backend (430xxx)

| Function | Line | Rename Suggestion | Purpose |
|----------|------|-------------------|---------|
| `sub_4301E0` | 47521 | `DP_ConnectToPlayer` | Establish connection to specific player via DirectPlay |
| `sub_430340` | 47617 | `DP_ReceivePump` | Main DirectPlay message receive loop |
| `sub_430610` | 47776 | `DP_StartEnumSessions` | Begin session enumeration |
| `sub_430640` | 47793 | `DP_StopEnumSessions` | Stop session enumeration |
| `sub_430670` | 47810 | `DP_SetEnumContext` | Set callback context for enum |
| `sub_430690` | 47827 | `DP_CloseSession` | Close player + release DirectPlay interface |
| `sub_4306C0` | 47848 | `DP_EnumProvidersCallback` | Service provider enumeration callback (allocates 28-byte node) |
| `sub_430780` | 47884 | `DP_InitProviders` | Enumerate all DirectPlay providers, create interfaces |
| `sub_430910` | 47999 | `DP_CreateInterface` | Create DirectPlay interface from selected provider |
| `sub_4309A0` | 48043 | `DP_EnumPlayersCallback` | Player enumeration callback (allocates 48-byte node) |
| `sub_430A70` | 48083 | `DP_EnumSessionsCallback` | Session enumeration callback (allocates 40-byte node) |
| `sub_430B10` | 48113 | `DP_OpenSession` | Open/create DirectPlay session + create player |
| `sub_430CE0` | 48208 | `DP_ResetPlayerSlots` | Zero all 78 player slot entries |
| `sub_430D10` | 48227 | `DP_AllocPlayerSlot` | Find free slot, initialize, return index |
| `sub_430D70` | 48265 | `DP_FreePlayerSlot` | Release player slot |
| `sub_430DA0` | 48278 | `DP_ProcessTimers` | Tick timer queue, fire expired callbacks |
| `NETZ_ErrorString` | 48312 | keep | Error code → human-readable string |

### Gameplay Integration

| Function | Line | Rename Suggestion | Purpose |
|----------|------|-------------------|---------|
| `sub_42E7F0` | 46176 | `NETZ_StartGame` | Set `g_netz_is_game_started=1`, assign player number |
| `sub_42F650` | — | `NETZ_F650` | External/import. Called during cleanup, returns int. |

---

## DirectPlay API Mapping

The DirectPlay interface `dword_47A898` is an `IDirectPlay3` vtable pointer. Methods accessed by vtable offset:

| Offset | Method | Used In |
|--------|--------|---------|
| +8 | `Release()` | `sub_42FCA0` |
| +16 | `Close()` | `sub_42FCA0`, `sub_4300C0` |
| +24 | `CreatePlayer()` | `sub_430B10` |
| +36 | `DestroyPlayer()` | `sub_42FCA0`, `sub_430690`, `sub_4300C0` |
| +52 | `EnumPlayers()` | `sub_42FD30` |
| +96 | `Open()` | `sub_430B10` |
| +100 | `Receive()` | `sub_430340` |
| +104 | `Send()` | `sub_42FF10`, `sub_42FFB0` |
| +124 | `EnumSessions()` | `sub_430610`, `sub_430640`, `sub_430670` |

External imports:
- `DPLAYX_1` (addr 459E5C) — `DirectPlayCreate()`
- `DPLAYX_2` (addr 459E62) — `DirectPlayEnumerate()` (enumerate service providers)

---

## Lockstep Synchronization

### How It Works

When `g_netz_synchronized_lockstep_mode` is set (from `dword_47A738 != 0`), the game enters a restricted execution mode:

1. **Entity updates gated**: Only entities whose `task->netz_flags & TaskNetzFlags_EnabledForMultiplayer` can:
   - Update positions (line 39487)
   - Advance animations (line 39924)
   - Process actions (line 9050)

2. **Input suppressed**: Mouse actions (building placement, etc.) blocked during lockstep (lines 41273, 41568, 41640)

3. **UI messaging restricted**: Certain task messages suppressed (lines 41143, 41148)

4. **Unit selection messaging**: Suppressed in lockstep mode (line 43213)

### Lockstep Triggers

| Event | Sets `dword_47A738` to | Effect |
|-------|------------------------|--------|
| `GameEvent_GamePaused` | 1 | Enter lockstep |
| `GameEvent_GameResumed` | 0 | Exit lockstep |
| `GameEvent_NetzPlayerDisconnected` | 1 | Force lockstep (freeze game) |
| Mission outcome | — | `g_netz_synchronized_lockstep_mode = 1` directly |
| Cleanup/shutdown | 0 | Reset |

### Per-Frame Sync (in gameplay loop)
```c
if (!g_is_single_player)
    g_netz_synchronized_lockstep_mode = dword_47A738 != 0;
```
This means lockstep is re-evaluated every frame from the master flag.

---

## Task Network Flags

```c
enum __bitmask KKND::TaskNetzFlags : unsigned __int32 {
    TaskNetzFlags_EnabledForMultiplayer = 0x1,
};
```

Field in Task struct: `KKND::TaskNetzFlags netz_flags;` (kknd.h line 1234)

Tasks marked with this flag can execute during lockstep mode. Core systems that need it:
- Mission outcome handlers (line 37747)
- Main game tasks (line 41076)
- Input processing (line 49281)
- UI/menu systems (lines 49474-49606)

Tasks WITHOUT the flag (most unit/entity tasks) freeze during lockstep — this ensures deterministic multiplayer by only allowing synchronized commands through.

---

## Global Variables

### Core State
| Variable | Type | Rename | Purpose |
|----------|------|--------|---------|
| `g_is_single_player` | `BOOL=1` | keep | SP/MP mode flag |
| `g_netz_is_game_started` | `BOOL` | keep | Game session active |
| `g_netz_is_game_host` | `BOOL` | keep | Host role flag |
| `g_netz_synchronized_lockstep_mode` | `__int16` | keep | Lockstep active flag |
| `dword_47A838` | `fn ptr` | `g_netz_message_callback` | Game-level message handler |
| `dword_47A830` | `int` | `g_netz_game_active` | Additional game-active flag (set with is_game_started) |
| `dword_47A778` | `int` | `g_netz_mission_completed` | MP mission done, controls reconnect flow |
| `dword_47A738` | `int` | `g_netz_lockstep_requested` | Master lockstep control, drives sync mode per frame |

### Connection State
| Variable | Type | Rename | Purpose |
|----------|------|--------|---------|
| `dword_468B68` | `int=-1` | `g_netz_connection_index` | Active connection type (0=TCP, 1=IPX, 2=Serial, -1=none) |
| `dword_468B7C[]` | `int[26*3]` | `g_netz_providers` | Provider capability structs (3 × 26 ints) |
| `dword_468B60` | `int=1` | `g_netz_systems_down` | 1 during init/shutdown, 0 normal |
| `dword_468B50` | `int=1` | `g_netz_player_count_cfg` | Player count config (used as `/ 2 + 1`) |
| `dword_468B54` | `int=-1` | `g_netz_B54` | Unknown, init -1 |
| `dword_468B58` | `int=-1` | `g_netz_local_player_index` | Local player index (used: `g_player_num = dword_468B58 + 1`) |

### DirectPlay Interface
| Variable | Type | Rename | Purpose |
|----------|------|--------|---------|
| `dword_47A898` | `int` | `g_dp_interface` | IDirectPlay3 interface pointer |
| `dword_47A890` | `int` | `g_dp_local_player_id` | Local DirectPlay player ID |
| `dword_47A89C..A8` | `int[4]` | `g_dp_provider_guid` | Active service provider GUID (16 bytes) |
| `dword_47A8DC` | `int` | `g_dp_provider_list` | Linked list of discovered providers (28-byte nodes) |
| `dword_47A8E0` | `void*` | `g_dp_session_list` | Linked list of discovered sessions (40-byte nodes) |
| `dword_47A8E4` | `int` | `g_dp_session_desc` | Session descriptor for enum |
| `dword_47A8E8` | `int` | `g_dp_enum_flags` | Session enumeration flags (0x21 = active) |
| `dword_47A88C` | `void*` | `g_dp_player_list` | Linked list of enumerated players (48-byte nodes) |
| `dword_47A934` | `int` | `g_dp_enum_players_active` | Player enumeration active flag |
| `dword_47A914` | `int` | `g_dp_enum_context` | Callback context for session enum |
| `dword_468D20` | `int=-1` | `g_netz_accepted_player_id` | Player ID accepted for join (-1 = accepting) |

### Player Slots
| Variable | Type | Rename | Purpose |
|----------|------|--------|---------|
| `dword_47A968[78]` | `int[]` | `g_netz_player_slots` | 7 slots × 11 ints: occupied flags |
| `dword_47A940[]` | `int[]` | `g_netz_player_states` | Player connection states (0/2) |
| `dword_47A964[]` | `int[]` | `g_netz_player_dp_ids` | DirectPlay IDs per slot |
| `dword_47B3A0` | `int` | `g_netz_active_player_count` | Count of occupied slots |
| `byte_47AAA0[]` | `char[]` | `g_netz_recv_seq` | Per-slot receive sequence numbers |
| `byte_47AAA1[]` | `char[]` | `g_netz_send_seq` | Per-slot send sequence counters |
| `dword_47ABBC[505]` | `int[]` | `g_netz_msg_sizes` | Per-slot message sizes |
| `dword_47B3B0` | `int` | `g_netz_timer_queue` | Timer event linked list head |

### Service Provider GUIDs
| Variable | Hex Values | Protocol |
|----------|-----------|----------|
| `dword_468CF0..CFC` | 685BC400, 11D1A21C, ... | TCP/IP (DPSPGUID_TCPIP) |
| `dword_468D10..D1C` | 0F1D6860, 11CE9C49, ... | IPX (DPSPGUID_IPX) |
| `dword_468D00..D0C` | 44EAA760, 11CE9C58, ... | Serial (DPSPGUID_SERIAL) |

### Player Info Message
Sent during join handshake (type 31, 0x21 = 33 bytes):
```
dword_47A840: "PLAYER" (6 bytes + null)
dword_47A844 high byte: from dword_468CE4 (build version)
dword_47A848: from dword_468CE8
byte_47A84C: build date string ("Oct 23 1997")
byte_47A858: build time string ("17:00:00")
```

---

## Menu Flow

```
Single Player:
  MenuId_Main → MenuId_NewCampaign → gameplay

Multiplayer (fresh):
  dword_468B68 == 0 → MenuId_IpxSetup (no connection yet)
  dword_468B68 != 0 → MenuId_Multiplayer (transport selected)
    → MenuId_HostGame or MenuId_JoinGame

Multiplayer (reconnect after mission):
  dword_47A778 == 1 (mission completed):
    Host → sub_42FB30(0) + MenuId_HostGame
    Client → MenuId_JoinGame
```

---

## Error Codes

Error base: `16646144` (0xFE0000). NETZ_ErrorString maps:

| Code | Offset | String |
|------|--------|--------|
| +0 | 16646144 | "Out of memory" |
| +1 | 16646145 | "Link in use already" |
| +2 | 16646146 | "no free links" |
| +3 | 16646147 | "Link is not connected" |
| +4 | 16646148 | "Protocol is not present" |
| +5 | 16646149 | "Link not open" |
| +7 | 16646151 | "Wrong type of link" |
| +8 | 16646152 | "Port/socket in use" |
| +9 | 16646153 | "Internal resource error" |
| +10 | 16646154 | "Packet is too big" |
| +11 | 16646155 | "Could not create" |
| +12 | 16646156 | "Wrong mode for network" |
| +13 | 16646157 | "Name is not unique" |
| +14 | 16646158 | "Failed" (generic — returned for uninitialized state) |
| +15 | 16646159 | "Operating system error" |
| +16 | 16646160 | "NETZ requires a 486" |
| +17 | 16646161 | "Link lost" |
| +18 | 16646162 | "Disabled" |
| +19 | 16646163 | "Not yet implemented" |

Note: `16646158` ("Failed") is the most common — returned whenever `dword_468B68 == -1` (not connected).

---

## Decompilation Issues

1. **`off_468B74` naming confusion**: Array says "DirectX IPX" twice but is at connection index 0 which maps to TCP/IP GUID. The global data layout has provider structs interleaved with string literals — decompiler sliced them wrong.

2. **Player slot arrays are parallel, not struct**: `dword_47A940`, `dword_47A964`, `dword_47A968` all use `[11*i]` indexing — these are really fields in a 44-byte struct, but decompiler split them into separate arrays. True layout:
   ```c
   struct NetzPlayerSlot {  // 44 bytes
       int state;           // 0=free, 2=connected (dword_47A940)
       int _pad[5];         // unknown fields
       int dp_player_id;    // DirectPlay ID (dword_47A964)
       int occupied;        // allocation flag (dword_47A968)
       int _pad2[3];        // unknown fields
   };
   NetzPlayerSlot g_netz_slots[7];  // 7 players max
   ```

3. **Send buffer is also parallel arrays**: `byte_47AAA0 + 288*i` indexing means each player has a 288-byte send/receive buffer. True layout:
   ```c
   struct NetzMessageBuffer {  // 288 bytes
       char recv_seq;          // +0 (byte_47AAA0)
       char send_seq;          // +1 (byte_47AAA1)
       char last_recv_seq;     // +2 (unk_47AAA2)
       char msg_type;          // +3 (byte_47AAA3)
       char payload[280];      // +4 (unk_47AAA4) — max message size
       int msg_size;           // +284 (dword_47ABBC)
   };
   NetzMessageBuffer g_netz_buffers[7];
   ```

4. **`sub_42FB10/20/30/40/50` — no bodies**: These are forward-declared but have no implementation in the decompiled output. Likely in a separate module, DLL import, or compiler-eliminated. `sub_42FB30` is called as host reconnect, `sub_42FB50` as session leave.

5. **`sub_42F650` — external**: Declared `weak` with no body. Used during cleanup. Returns int (used as: `dword_477340 = sub_42F650()`). Likely external network library function.

6. **`dword_468B50 / 2 + 1` pattern**: Used to calculate number of players from a config value. Default `dword_468B50 = 1` → `1/2 + 1 = 1`. If 2 → `2/2 + 1 = 2`. This is a player count formula.

7. **DirectPlay vtable offsets are IDirectPlay3**: The vtable offset pattern (+8 Release, +16 Close, +24 CreatePlayer, +36 DestroyPlayer, +96 Open, +100 Receive, +104 Send) matches IDirectPlay3 vtable layout for the 1997 DirectX SDK.

---

## Lifecycle Summary

```
NETZ_Init(transport, _, callback)
  ├── Enumerate providers via DPLAYX_2
  ├── Select provider by GUID (sub_42FB60)
  └── Store message callback (dword_47A838)

NETZ_HostOrJoin(is_host)          // sub_42F8C0
  ├── Host: DP_CreateInterface → DP_OpenSession(create)
  └── Client: DP_CreateInterface → prepare for join

DP_ReceivePump()                  // sub_430340 — called every frame via NETZ_Poll
  ├── DirectPlay Receive
  ├── Filter own messages + check sequence
  ├── Handle type 30 (join request) → allocate slot → respond type 31
  ├── Handle type 31 (join confirm) → store player in slot
  └── Route type ≥ 50 → dword_47A838 game callback

NETZ_Send(player, type, data)     // sub_42FA00
  ├── If player == -1: broadcast to all connected
  └── sub_42FF10 → build packet → DirectPlay Send

NETZ_Shutdown()                   // sub_42F9A0
  ├── sub_42FCA0 (×2): close session, release interface
  └── dword_468B68 = -1
```
