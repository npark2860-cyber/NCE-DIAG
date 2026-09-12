# Run selection

NCE-DIAG 0.2.1 supports suite, single-test, and range execution without source edits.

Stable 1-based registry order:

1. `CPU.NZCV.CINC.001`
2. `CPU.NZCV.CSEL.001`
3. `CPU.REG.PRESERVE.001`
4. `IPC.SM.GET_SERVICE.001`
5. `IPC.SVC21.REPEATED.001`

## Runtime arguments

```text
--all
--test=CPU.NZCV.CINC.001
--range=1-3
--persist
--debug-only
--config
--no-config
```

Default execution does not read a config file because config access requires FS/SDMC and would add IPC before the CPU tests.

Use `--config` only when filesystem startup itself is acceptable. Supported keys remain:

```text
run=all
test=CPU.NZCV.CINC.001
range=1-3
persist=0
```

## Compile-time preset without source edits

The Makefile accepts 1-based registry bounds:

```sh
make TARGET=NCE-DIAG-CINC BUILD=build-cinc NCE_DIAG_DEFAULT_FIRST=1 NCE_DIAG_DEFAULT_LAST=1
```

The CI build publishes `NCE-DIAG-CINC.nro` using this mechanism.

Any range can use the same variables. This is the preferred direct-Eden isolation method when launcher arguments are unavailable.
