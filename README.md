# linear\_code

Reversing challenge from hxp CTF 2022

Category | Reversiong
Estimate | Hard
Solves   |
Flag     | hxp{BFGQS0C0DUV0EJM1G0I0Z1K0LST1N0P0Z0QR0YZ4WX3}

## Building

1. Build the Docker container of the [Obfuscator](https://github.com/leetonidas/linearize) and tag it *hxp:obfuscator*.
2. Use build.sh to build the challenge with the obfuscator from (1)
3. Reorder the relocations to move the _IRELATIV_ relocation between _RELATIVE_ and _GLOB_DAT_
```Python3
with open("chall", "r+b") as f:
    f.seek(0x6e8 + 3 * 0x18)
    data = f.read(10 * 0x18)
    entry = f.read(0x18)
    f.seek(0x6e8 + 3 * 0x18)
    f.write(entry)
    f.write(data)
```
4. Patch the Binary in two places:
    - move the relocation address of the ifunc to 0x8ba0
    - place a breakpoint at the first instruction of the call to memfrob (0x11e8)
5. Fix the crc checksum of the binary (fixcrc.py chall \<offset to end of text segment rounded to next 0x10\> (0x68e0))

## Modifying
When modifying you need to fix manual offsets

- protect.c
    1. l. 203: offset of crc64\_we
    2. l. 210: address of graph
    3. l. 215: offset of \_ITM\_consumeTMCloneTable in main.c 
- main.c
    1. l. 142: offset of permute, offset of the breakpoint (in memfrob)

