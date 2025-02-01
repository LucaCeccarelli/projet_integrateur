# Top-level Makefile for the entire project

# Directories for each group of sources
IFSHOW_DIR       = ifshow
IFNETSHOW_DIR    = ifnetshow
NEIGHBORSHOW_DIR = neighborshow

# Compiler settings
CC      = gcc
CFLAGS  = -Wall -Wextra -g

# Object files for the ifshow group (standalone ifshow command)
OBJS_IFSHOW = ifshow_main.o ifshow.o

# Object files for the ifnetshow group
OBJS_IFNETSHOW_AGENT  = ifnetshow_agent.o
OBJS_IFNETSHOW_CLIENT = ifnetshow_client.o

# Object files for the neighborshow group
OBJS_NEIGHBORSHOW_AGENT = neighborshow_agent.o
OBJS_NEIGHBORSHOW       = neighborshow.o

# Default target builds all executables
all: ifshow_cmd ifnetshow_agent ifnetshow_client neighborshow_agent neighborshow_cmd

# Link the ifshow command executable.
# (Renamed to ifshow_cmd to avoid conflict with the "ifshow" directory.)
ifshow_cmd: $(OBJS_IFSHOW)
	$(CC) $(CFLAGS) -o $@ $(OBJS_IFSHOW)

# Link the ifnetshow agent executable.
# (It reuses common functions from ifshow, so we include ifshow.o.)
ifnetshow_agent: $(OBJS_IFNETSHOW_AGENT) ifshow.o
	$(CC) $(CFLAGS) -o $@ $(OBJS_IFNETSHOW_AGENT) ifshow.o

# Link the ifnetshow client executable.
ifnetshow_client: $(OBJS_IFNETSHOW_CLIENT)
	$(CC) $(CFLAGS) -o $@ $(OBJS_IFNETSHOW_CLIENT)

# Link the neighborshow agent executable.
neighborshow_agent: $(OBJS_NEIGHBORSHOW_AGENT)
	$(CC) $(CFLAGS) -o $@ $(OBJS_NEIGHBORSHOW_AGENT)

# Link the neighborshow command executable.
# (Renamed to neighborshow_cmd to avoid conflict with the "neighborshow" directory.)
neighborshow_cmd: $(OBJS_NEIGHBORSHOW)
	$(CC) $(CFLAGS) -o $@ $(OBJS_NEIGHBORSHOW)

# Compilation rules for the ifshow group
ifshow_main.o: $(IFSHOW_DIR)/ifshow_main.c $(IFSHOW_DIR)/ifshow.h
	$(CC) $(CFLAGS) -I$(IFSHOW_DIR) -c $(IFSHOW_DIR)/ifshow_main.c -o $@

ifshow.o: $(IFSHOW_DIR)/ifshow.c $(IFSHOW_DIR)/ifshow.h
	$(CC) $(CFLAGS) -I$(IFSHOW_DIR) -c $(IFSHOW_DIR)/ifshow.c -o $@

# Compilation rules for the ifnetshow group
ifnetshow_agent.o: $(IFNETSHOW_DIR)/ifnetshow_agent.c $(IFSHOW_DIR)/ifshow.h
	$(CC) $(CFLAGS) -I$(IFNETSHOW_DIR) -I$(IFSHOW_DIR) -c $(IFNETSHOW_DIR)/ifnetshow_agent.c -o $@

ifnetshow_client.o: $(IFNETSHOW_DIR)/ifnetshow_client.c
	$(CC) $(CFLAGS) -I$(IFNETSHOW_DIR) -c $(IFNETSHOW_DIR)/ifnetshow_client.c -o $@

# Compilation rules for the neighborshow group
neighborshow_agent.o: $(NEIGHBORSHOW_DIR)/neighborshow_agent.c $(NEIGHBORSHOW_DIR)/neighborshow.h
	$(CC) $(CFLAGS) -I$(NEIGHBORSHOW_DIR) -c $(NEIGHBORSHOW_DIR)/neighborshow_agent.c -o $@

neighborshow.o: $(NEIGHBORSHOW_DIR)/neighborshow.c $(NEIGHBORSHOW_DIR)/neighborshow.h
	$(CC) $(CFLAGS) -I$(NEIGHBORSHOW_DIR) -c $(NEIGHBORSHOW_DIR)/neighborshow.c -o $@

# 'clean' target removes only the intermediate object files.
clean:
	rm -f *.o

# 'distclean' target removes both the object files and the final executables.
distclean: clean
	rm -f ifshow_cmd ifnetshow_agent ifnetshow_client neighborshow_agent neighborshow_cmd

.PHONY: all clean distclean
