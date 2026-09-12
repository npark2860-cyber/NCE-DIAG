#!/usr/bin/env python3
import argparse
import re
import sys
from collections import Counter

TEST_PREFIX = 0x4E43455400000000
CKPT_PREFIX = 0x4E43454300000000
PREFIX_MASK = 0xFFFFFFFF00000000
HASH_MASK = 0x00000000FFFFFFFF

TEST_IDS = [
    "CPU.NZCV.CINC.001",
    "CPU.NZCV.CSEL.001",
    "CPU.REG.PRESERVE.001",
    "IPC.SM.GET_SERVICE.001",
    "IPC.SVC21.REPEATED.001",
]

CHECKPOINT_IDS = [
    "00_ENTER",
    "90_RESULT_RETURNED",
    "10_PRE_SEQUENCE",
    "20_POST_SEQUENCE",
    "30_RESULT_CAPTURED",
    "05_SM_READY_CHECK",
    "06_SM_NOT_READY",
    "10_HANDLE_READY",
    "15_PRE_REQUEST_BUILD",
    "20_REQUEST_BUILT",
    "30_PRE_SVC",
    "40_POST_SVC",
    "45_PRE_RESULT_PARSE",
    "50_RESULT_PARSED",
    "55_PRE_CLOSE",
    "56_POST_CLOSE",
    "60_DONE",
    "60_ITER_DONE",
    "70_LOOP_DONE",
]

PRE_RE = re.compile(
    r"IMP008_REENTRY_STATE_PRE\s+n=(?P<n>\d+).*?"
    r"svc=(?P<svc>[0-9A-Fa-f]+).*?"
    r"x19=(?P<x19>[0-9A-Fa-f]+)\s+x20=(?P<x20>[0-9A-Fa-f]+)"
)


def fnv1a32(text: str) -> int:
    value = 2166136261
    for byte in text.encode("ascii"):
        value ^= byte
        value = (value * 16777619) & 0xFFFFFFFF
    return value


def build_map(items):
    result = {}
    for item in items:
        key = fnv1a32(item)
        if key in result:
            raise RuntimeError(f"FNV collision: {result[key]} vs {item}")
        result[key] = item
    return result


TEST_MAP = build_map(TEST_IDS)
CKPT_MAP = build_map(CHECKPOINT_IDS)


def decode_tag(value: int, prefix: int, mapping):
    if value & PREFIX_MASK != prefix:
        return None
    return mapping.get(value & HASH_MASK, f"UNKNOWN_HASH_{value & HASH_MASK:08X}")


def decode_lines(lines):
    occurrences = Counter()
    decoded = []
    for line in lines:
        match = PRE_RE.search(line)
        if not match:
            continue
        if int(match.group("svc"), 16) != 0x27:
            continue

        x19 = int(match.group("x19"), 16)
        x20 = int(match.group("x20"), 16)
        test_id = decode_tag(x19, TEST_PREFIX, TEST_MAP)
        checkpoint_id = decode_tag(x20, CKPT_PREFIX, CKPT_MAP)
        if test_id is None or checkpoint_id is None:
            continue

        key = (test_id, checkpoint_id)
        occurrences[key] += 1
        decoded.append(
            {
                "n": int(match.group("n")),
                "test": test_id,
                "checkpoint": checkpoint_id,
                "occurrence": occurrences[key],
                "x19": x19,
                "x20": x20,
            }
        )
    return decoded


def self_test():
    test_tag = TEST_PREFIX | fnv1a32("CPU.NZCV.CINC.001")
    checkpoint_tag = CKPT_PREFIX | fnv1a32("20_POST_SEQUENCE")
    sample = (
        "Core.ARM <Error> RunThread: IMP008_REENTRY_STATE_PRE n=30 path=post "
        f"pc=0 svc=00000027 x19={test_tag:016X} x20={checkpoint_tag:016X} "
        "x29=0 x30=0"
    )
    decoded = decode_lines([sample])
    assert len(decoded) == 1
    assert decoded[0]["test"] == "CPU.NZCV.CINC.001"
    assert decoded[0]["checkpoint"] == "20_POST_SEQUENCE"
    assert decoded[0]["occurrence"] == 1
    print("checkpoint decoder self-test: PASS")


def main():
    parser = argparse.ArgumentParser(
        description="Decode NCE-DIAG x19/x20 checkpoint witnesses from an Eden log"
    )
    parser.add_argument("log", nargs="?", help="Eden log file")
    parser.add_argument("--self-test", action="store_true")
    args = parser.parse_args()

    if args.self_test:
        self_test()
        return 0
    if not args.log:
        parser.error("log path is required unless --self-test is used")

    with open(args.log, "r", encoding="utf-8", errors="replace") as handle:
        decoded = decode_lines(handle)

    if not decoded:
        print("No NCE-DIAG checkpoint witness found.")
        return 1

    for item in decoded:
        print(
            f"n={item['n']} test={item['test']} checkpoint={item['checkpoint']} "
            f"occurrence={item['occurrence']} "
            f"x19={item['x19']:016X} x20={item['x20']:016X}"
        )

    last = decoded[-1]
    print(
        "LAST "
        f"test={last['test']} checkpoint={last['checkpoint']} "
        f"occurrence={last['occurrence']} n={last['n']}"
    )
    return 0


if __name__ == "__main__":
    sys.exit(main())
