# Single Makefile for the entire project

CC      = gcc
CFLAGS  = -Wall -Wextra -g

# Object files for ifshow group (standalone ifshow command)
OBJS_IFSHOW = ifshow_main.o ifshow.o

# Object files for ifnetshow group (agent and client reusing ifshow.o)
OBJS_IFNETSHOW_AGENT = ifnetshow_agent.o ifshow.o
OBJS_IFNETSHOW_CLIENT = ifnetshow_client.o

# Object files for neighborshow group
OBJS_NEIGHBORSHOW_AGENT = neighborshow_agent.o
OBJS_NEIGHBORSHOW = neighborshow.o

# Default target builds all executables
all: ifshow ifnetshow_agent ifnetshow_client neighborshow_agent neighborshow

# Build rule for standalone ifshow command
ifshow: $(OBJS_IFSHOW)
	$(CC) $(CFLAGS) -o $@ $(OBJS_IFSHOW)

# Build rule for ifnetshow_agent
ifnetshow_agent: $(OBJS_IFNETSHOW_AGENT)
	$(CC) $(CFLAGS) -o $@ $(OBJS_IFNETSHOW_AGENT)

# Build rule for ifnetshow_client
ifnetshow_client: $(OBJS_IFNETSHOW_CLIENT)
	$(CC) $(CFLAGS) -o $@ $(OBJS_IFNETSHOW_CLIENT)

# Build rule for neighborshow_agent
neighborshow_agent: $(OBJS_NEIGHBORSHOW_AGENT)
	$(CC) $(CFLAGS) -o $@ $(OBJS_NEIGHBORSHOW_AGENT)

# Build rule for neighborshow command
neighborshow: $(OBJS_NEIGHBORSHOW)
	$(CC) $(CFLAGS) -o $@ $(OBJS_NEIGHBORSHOW)

# Compilation rules for individual source files
ifshow_main.o: ifshow_main.c ifshow.h
	$(CC) $(CFLAGS) -c ifshow_main.c

ifshow.o: ifshow.c ifshow.h
	$(CC) $(CFLAGS) -c ifshow.c

ifnetshow_agent.o: ifnetshow_agent.c ifshow.h
	$(CC) $(CFLAGS) -c ifnetshow_agent.c

ifnetshow_client.o: ifnetshow_client.c
	$(CC) $(CFLAGS) -c ifnetshow_client.c

neighborshow_agent.o: neighborshow_agent.c neighborshow.h
	$(CC) $(CFLAGS) -c neighborshow_agent.c

neighborshow.o: neighborshow.c neighborshow.h
	$(CC) $(CFLAGS) -c neighborshow.c

# 'clean' target removes only the intermediate object files
clean:
	rm -f *.o

# 'distclean' removes both object files and all executables
distclean: clean
	rm -f ifshow ifnetshow_agent ifnetshow_client neighborshow_agent neighborshow

.PHONY: all clean distclean
