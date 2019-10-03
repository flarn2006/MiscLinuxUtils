CC="gcc"
CPP="g++"
CFLAGS=-O3
TARGETS=apply bgexec execstop extpath faketty fpoke rlcat seek setns tell xchg

all: $(TARGETS)
clean:
	rm -rf $(TARGETS)

apply: apply.c
	$(CC) $(CFLAGS) apply.c -o apply

bgexec: bgexec.c
	$(CC) $(CFLAGS) bgexec.c -o bgexec

execstop: execstop.c
	$(CC) $(CFLAGS) execstop.c -o execstop

extpath: extpath.c
	$(CC) $(CFLAGS) extpath.c -o extpath

faketty: faketty.c
	$(CC) $(CFLAGS) faketty.c -o faketty

fpoke: fpoke.c
	$(CC) $(CFLAGS) fpoke.c -o fpoke

not: not.c
	$(CC) $(CFLAGS) not.c -o not

rlcat: rlcat.c
	$(CC) $(CFLAGS) rlcat.c -lreadline -o rlcat

seek: seek.c
	$(CC) $(CFLAGS) seek.c -o seek

setns: setns.c
	$(CC) $(CFLAGS) setns.c -o setns

tell: tell.c
	$(CC) $(CFLAGS) tell.c -o tell

xchg: xchg.c
	$(CC) $(CFLAGS) xchg.c -o xchg
