#!/usr/bin/env python3

from sys import argv, exit
from ucrc import crc64_we
import subprocess

if __name__ == "__main__":
    if len(argv) != 3:
        exit(1)

    marker = marker = 0xbadc0dedeadbeef.to_bytes(8, 'big')
    with open(argv[1], "rb+") as f:
        data = f.read(int(argv[2], 0))
        idx = data.index(marker)
        print(f"[+] found marker @ {idx}")
        crc = hex(crc64_we(data))
        print(f"[+] crc was {crc}")
        res = subprocess.check_output(["sage", "fixcrc.sage", crc, f"{len(data) - idx - 8}"])
        print(f"[+] marker should be {res}")
        should = int(res, 0)
        f.seek(idx)
        f.write(should.to_bytes(8, 'big'))

