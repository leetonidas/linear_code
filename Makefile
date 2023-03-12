#CC = /home/iead/projects/llvm/.local/bin/clang
#CXX = /home/iead/projects/llvm/.local/bin/clang++
CC = clang
CXX = clang++
OPT = opt
#CFLAGS = -target arm-linux-gnueabihf -s -O1 -msse4.2 -mpclmul --sysroot /home/iead/crosscompile/bullseye-armhf/
#CXXFLAGS = -target arm-linux-gnueabihf -s -O1 -msse4.2 -mpclmul --sysroot /home/iead/crosscompile/bullseye-armhf/
#LDFLAGS = -target arm-linux-gnueabihf -fuse-ld=lld -s -Wl,-z,relro,-z,now  --sysroot /home/iead/crosscompile/bullseye-armhf/

CFLAGS = -O1 -msse4.2 -mpclmul
CXXFLAGS = -O1 -msse4.2 -mpclmul
LDFLAGS = -s -Wl,-z,relro,-z,now

OBJS = ptrace.o main.o protect.o linkmap.o crc.o perm.o

all: chall

ll: main.ll

%.ll: %.c
	${CC} ${CFLAGS} -emit-llvm -Xclang -disable-O0-optnone -S -c $^

main.o: main.c
	${CC} ${CFLAGS} -flinearize-cfg -c $< 2>/dev/null
#	${CC} ${CFLAGS} -fpass-plugin=Linearize.so -c $<

crc.o: crc.c
	${CC} ${CFLAGS} -c $<

protect.o: protect.c
	${CC} ${CFLAGS} -flinearize-cfg -c $< 2>/dev/null

#linkmap.o: linkmap.c
#	${CC} ${CFLAGS} -flinearize-cfg -c $< 2>/dev/null

linkmap.o: linkmap_lin.bc
	${CC} ${CFLAGS} -c -o $@ $<

linkmap_lin.bc: linkmap.ll
	${OPT} -linearize -o $@ $< 2>/dev/null

ptrace.o: ptrace.c
	${CC} ${CFLAGS} -flinearize-cfg -c $< 2>/dev/null

perm.o: perm.c
	${CC} ${CFLAGS} -flinearize-cfg -c $< 2>/dev/null

chall: ${OBJS}
	${CC} ${LDFLAGS} -o $@ $^

clean:
	rm -f chall ${OBJS}

.phony: all ll clean