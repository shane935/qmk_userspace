# Known bugs

Things that are wrong and not worth fixing yet. Each one says what it takes to
fix, so the decision does not have to be made again from scratch.

## A host that dies mid-WITHIN sends copy-mode keys to the program

Entering WITHIN offline is fine: the target resolves to `copy` and `F19` opens
copy mode, so the keys land in copy mode where they belong. The problem is the
host dying while WITHIN is *already* open.

After 1500 ms the target flips to `copy`, but nothing re-runs the entry, so copy
mode is not open — and the copy column's letters go straight to whatever is in
the pane. `b`, `w`, `0`, `$`, `n`, `N`, `V` and `Enter`, to a program that was
being driven by the transcript or hunk column a moment ago.

The sharpest case is a Claude pane. `claude-safe` exists so that nothing can put
a letter into a live prompt, and that guarantee holds only while the host is
alive: pull the cable and WORD+← types a `b` into the message box. `q` is still
unreachable from any column, so hunk cannot be quit.

**Fix:** in `raw_hid_receive`, notice the alive→dead edge and re-run
`within_send(WK_ENTER)` while `tmux_mode == _TMUX_WITHIN`. That opens copy mode
for real and makes the offline column honest. Needs a flag for the previous
liveness, since `host_alive()` is computed rather than stored, and liveness goes
stale by a timer rather than by a report — so the edge has to be checked
somewhere that runs without one, probably `housekeeping_task_user`.
