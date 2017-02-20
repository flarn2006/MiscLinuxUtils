CC="gcc"
CPP="g++"
CFLAGS=-O3
TARGETS=apply bgexec rlcat seek tell

all: $(TARGETS)
clean:
	rm -rf $(TARGETS)

apply: apply.c
	$(CC) $(CFLAGS) apply.c -o apply

bgexec: bgexec.c
	$(CC) $(CFLAGS) bgexec.c -o bgexec

rlcat: rlcat.c
	$(CC) $(CFLAGS) rlcat.c -lreadline -o rlcat

seek: seek.c
	$(CC) $(CFLAGS) seek.c -o seek

tell: tell.c
	$(CC) $(CFLAGS) tell.c -o tell
