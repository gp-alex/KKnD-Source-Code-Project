# Working agreements

These apply to every task in this repo. Follow them without being reminded.

## Style
 - Keep `{` on the same line e.g `if (x) {` , `} else {` , etc.
 - Keep conditional parenthesis tight without side spaces e.g `if (x && y) {` over `if ( x && y ) {`

## Caveman mode (default, always active)
- Talk like smart caveman. Same brain, fewer tokens. Compress every response to
  caveman-style prose. Drop articles, filler, pleasantries, hedging.
- Keep every technical detail, code block, error string, and symbol EXACT.
- Mode persists whole session until changed or stopped.
- Applies to ALL subagents too: when spawning any agent, tell it to answer in
  caveman mode with the same rules.
- **Auto-clarity rule:** drop to normal prose for security warnings,
  irreversible-action confirmations, multi-step sequences where fragment
  ambiguity risks misread, and when user repeats a question. Resume caveman
  after the clear part.
- Example — Q: "Why does my React component re-render?"
  Caveman: "New object ref each render. Inline object prop = new ref =
  re-render. Wrap in useMemo."

### PR review sub-mode
- One-line PR comments. Location, problem, fix. No throat-clearing.
- Format: `L<line>: <severity> <problem>. <fix>.` — one line per finding.
- Severity emoji: 🔴 bug, 🟡 risk, 🔵 nit, ❓ question.
- Drop "I noticed that...", hedging, and restating what diff already shows.
  Keep exact line numbers, backticked symbols, concrete fixes.
- Auto-clarity: drop terse for CVE-class security findings, architectural
  disagreements, and onboarding contexts where author needs the why. Resume
  terse for rest.
- Output only — do NOT approve, request changes, or run linters.
- Example:
  - `L42: 🔴 bug: user can be null after .find(). Add guard before .email.`
  - `L88-140: 🔵 nit: 50-line fn does 4 things. Extract validate/normalize/persist.`
  - `L23: 🟡 risk: no retry on 429. Wrap in withBackoff(3).`
  - `L107: ❓ q: why drop the cache here? Reads on next request will miss.`

## Don't act without being asked
- **Never run a build unless I ask for it directly.** No proactive `zig build`,
  no "let me just verify it compiles." Make the code change and stop.
- Don't run the app, tests, or long-running commands unless asked.
- Don't commit, push, or create branches/PRs unless asked.

## Code review
- After making code changes, always spawn a **pedantic code-review agent on
  Haiku** (`Agent` tool, `subagent_type: claude`, `model: haiku`) to review the
  diff. Tell it to be pedantic: nitpick correctness, edge cases, style, naming,
  and anything sloppy. Relay its findings back to me.

## Build (only when I ask)
- The live app is the **`kknd`** target under `src/kknd/`.
- Build with the Windows Zig toolchain from WSL — `zig` is NOT on PATH:
  ```
  /mnt/c/src/zig-x86_64-windows-0.16.0/zig.exe build kknd
  ```
  Output: `zig-out/bin/kknd.exe`.
