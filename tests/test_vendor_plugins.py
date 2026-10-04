#!/usr/bin/env python3

import os

ROOT = os.path.dirname(os.path.dirname(__file__))

cases = {
    'src/plugins/panos.c': 9,
    'src/plugins/checkpoint.c': 9,
    'src/plugins/fortinet.c': 9,
    'src/plugins/juniper.c': 9,
    'src/plugins/sonicwall.c': 9,
}

failed = []
for rel, expected in cases.items():
    path = os.path.join(ROOT, rel)
    with open(path, 'r', encoding='utf-8') as fh:
        lines = fh.read().splitlines()
    if len(lines) != expected:
        failed.append((rel, len(lines), expected))

if failed:
    for rel, actual, expected in failed:
        print(f'{rel}: expected {expected} lines, got {actual}')
    raise SystemExit(1)

print('all vendor plugin line-count checks passed')
