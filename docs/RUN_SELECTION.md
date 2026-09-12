# Run selection

NCE-DIAG 0.2 supports suite, single-test, and range execution without source edits.

Selection precedence:

1. command-line arguments
2. `sdmc:/nce_diag.cfg`
3. `nce_diag.cfg`
4. default: all tests

Supported command-line forms:

```text
--all
--test=CPU.NZCV.CINC.001
--range=1-3
--persist
--debug-only
--no-config
```

Supported config keys:

```text
run=all
test=CPU.NZCV.CINC.001
range=1-3
persist=0
```

The config file is read before any testcase begins. Internal testcase checkpoints never perform filesystem I/O.

Stable 1-based registry order for 0.2:

1. `CPU.NZCV.CINC.001`
2. `CPU.NZCV.CSEL.001`
3. `CPU.REG.PRESERVE.001`
4. `IPC.SM.GET_SERVICE.001`
5. `IPC.SVC21.REPEATED.001`
