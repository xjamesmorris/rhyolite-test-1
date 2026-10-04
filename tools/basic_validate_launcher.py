#!/usr/bin/env python3
import os
import subprocess
import sys

TEST_SUITE = "/home/jmorris/src/l1/src/github-public/rhyolite-test-1/tests/validate_basic.pl"

if not os.path.exists(TEST_SUITE):
    print(f"missing validation suite: {TEST_SUITE}", file=sys.stderr)
    raise SystemExit(2)

cmd = ["/usr/bin/perl", TEST_SUITE] + sys.argv[1:]
raise SystemExit(subprocess.call(cmd))
