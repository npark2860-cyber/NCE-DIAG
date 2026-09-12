# NCE-DIAG

Independent internal guest-side conformance and diagnostic workload for Windows ARM64 NCE validation.

## 0.2 crash-observability milestone

Primary artifact: `NCE-DIAG.nro`.

The 0.2 harness is designed for the case where the Eden host process itself may terminate during a guest test. The primary diagnostic channel is a direct `svcOutputDebugString` call (SVC 0x27), not GUI rendering and not the final JSON file.

Every testcase follows:

```text
BEGIN
CKPT before dangerous operation
CKPT after dangerous operation
PASS / FAIL
END
```

If the host disappears after a PRE checkpoint and before the matching POST checkpoint, the last Eden guest-debug line defines the crash boundary.

No GPU console, HID polling loop, audio, network, or presentation path is required by the harness.

## Included stable tests

1. `CPU.NZCV.CINC.001`
2. `CPU.NZCV.CSEL.001`
3. `CPU.REG.PRESERVE.001`
4. `IPC.SM.GET_SERVICE.001`
5. `IPC.SVC21.REPEATED.001`

No new testcase was added for 0.2; this milestone changes observability and execution control only.

## Selection

Default: run the full suite.

Command-line forms:

```text
--all
--test=CPU.NZCV.CINC.001
--range=1-3
--persist
--debug-only
--no-config
```

If no explicit test/range argument is supplied, the harness looks for `sdmc:/nce_diag.cfg`, then `nce_diag.cfg`.

Config examples:

```text
run=all
persist=0
```

```text
test=CPU.NZCV.CINC.001
persist=0
```

```text
range=4-5
persist=1
```

Persistent mode appends safe lifecycle markers to `sdmc:/nce_diag_journal.log` (fallback: local path) and flushes each record. Internal CKPT markers remain debug-output only so file IPC cannot overwrite a prepared TLS IPC request.

Normal completion also writes `sdmc:/nce_diag_result.json` with a local-path fallback.

## Build

Requirements: devkitPro, devkitA64, libnx.

```sh
make
```

Expected executable:

```text
NCE-DIAG.nro
```

CI also verifies the embedded raw `CPU.NZCV.CINC.001` instruction shape. The standalone raw ELF/BIN remains an auxiliary build artifact only; it is not the final diagnostic deliverable.

See `docs/CHECKPOINTS.md`, `docs/RUN_SELECTION.md`, and `docs/TESTS.md`.
