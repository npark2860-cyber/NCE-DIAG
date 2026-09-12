#!/usr/bin/env python3
import re
import sys
from pathlib import Path

if len(sys.argv) != 2:
    raise SystemExit("usage: verify_raw_sequence.py <objdump.txt>")

text = Path(sys.argv[1]).read_text(encoding="utf-8").lower()
start = text.find("<nce_diag_raw_cpu_nzcv_cinc_001>:")
if start < 0:
    raise SystemExit("missing raw microtest symbol")

body = text[start:]
patterns = [
    r"\bcmp\s+w16,\s*#(?:0x0|0)\b",
    r"\bmrs\s+x10,\s*nzcv\b",
    r"\bstp\s+w17,\s*w18,\s*\[x14,\s*#(?:0xc|12)\]",
    r"\bmrs\s+x11,\s*nzcv\b",
    r"\bmov\s+w2,\s*#(?:0x2|2)\b",
    r"\bcinc\s+w2,\s*w2,\s*ne\b",
]

cursor = 0
for pattern in patterns:
    match = re.search(pattern, body[cursor:])
    if not match:
        raise SystemExit(f"missing or out-of-order instruction: {pattern}")
    cursor += match.end()

print("raw sequence verified: cmp -> mrs -> stp w17,w18,+0x0c -> mrs -> mov #2 -> cinc ne")
