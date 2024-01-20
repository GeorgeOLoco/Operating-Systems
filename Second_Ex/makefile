TARGETS = child parent

# Targets to build when running `make`
all: $(TARGETS)

# Phony targets. Executed always.
.PHONY: all clean

# Targets to build `child` binary
child: child.o
	gcc child.o -o child
child.o: child.c
	gcc child.c -c -o child.o

# Targets to build `parent` binary
parent: parent.o
	gcc parent.o -o gates
parent.o: parent.c
	gcc parent.c -c -o parent.o

# Run `make clean` to delete binaries and object files.
clean:
	rm -rf $(TARGETS) *.o
