import sys

if __name__ == "__main__":
    if len(sys.argv) != 3:
        sys.exit(1)

    gf.<x> = GF(2)[]
    r_we = gf([*map(int,bin(0x42F0E1EBA9EA3693)[:1:-1])]) + x^64
    crc64f.<y> = gf.quotient(r_we)
    cv = int(sys.argv[1], 0)
    idx = int(sys.argv[2], 0)

    marker = crc64f([*map(int,bin(0xbadc0dedeadbeef)[:1:-1])])
    crcval = crc64f([*map(int,bin(cv)[:1:-1])])

    pos = y^(idx * 8 + 64)
    should = marker + crcval / pos
    should_py = int("".join(map(str, list(should)))[::-1],2)
    print(f"{should_py:#x}")
