# Makefile for ifshow, ifnetshow_agent, and ifnetshow_client
# Compiler and flags
CC      = gcc
CFLAGS  = -Wall -Wextra -g

# Object files for each target
OBJS_IFSHOW_AGENT = ifnetshow_agent.o ifshow.o
OBJS_IFSHOW       = ifshow_main.o ifshow.o
OBJS_CLIENT       = ifnetshow_client.o

# Default target builds all programs
all: ifshow ifnetshow_agent ifnetshow_client

# Build rules for the standalone ifshow command
ifshow: $(OBJS_IFSHOW)
	$(CC) $(CFLAGS) -o $@ $(OBJS_IFSHOW)

# Build rules for the agent
ifnetshow_agent: $(OBJS_IFSHOW_AGENT)
	$(CC) $(CFLAGS) -o $@ $(OBJS_IFSHOW_AGENT)

# Build rules for the client
ifnetshow_client: $(OBJS_CLIENT)
	$(CC) $(CFLAGS) -o $@ $(OBJS_CLIENT)

# Compile individual source files
ifnetshow_agent.o: ifnetshow_agent.c ifshow.h
	$(CC) $(CFLAGS) -c ifnetshow_agent.c

ifshow.o: ifshow.c ifshow.h
	$(CC) $(CFLAGS) -c ifshow.c

ifshow_main.o: ifshow_main.c ifshow.h
	$(CC) $(CFLAGS) -c ifshow_main.c

ifnetshow_client.o: ifnetshow_client.c
	$(CC) $(CFLAGS) -c ifnetshow_client.c

# 'clean' target removes only the object files, leaving the executables intact
clean:
	rm -f *.o

# Optionally, you can add a target to remove everything (executables and objects)
distclean: clean
	rm -f ifshow ifnetshow_agent ifnetshow_client

.PHONY: all clean distclean
