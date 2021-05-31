#!/usr/bin/env python3

import sys

line = list(sys.stdin)[-1]
split = line.split(' ')
addr = split[-3]
addr = list(addr)
# addr[0] = "a"
print("".join(addr))
