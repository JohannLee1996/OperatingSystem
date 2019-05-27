# Targets & general dependencies
PROGRAM = atsim
HEADERS = 
OBJS = atsim.o 
# add your additional object files here
ADD_OBJS = 

# compilers, linkers, utilities, and flags
CC = gcc
CFLAGS = -Wall -g
COMPILE = $(CC) $(CFLAGS)
LINK = $(CC) $(CFLAGS) -o $@ 

# implicit rule to build .o from .c files
%.o: %.c $(HEADERS)
	$(COMPILE) -c -o $@ $<


# explicit rules
all: $(PROGRAM)

$(PROGRAM): $(OBJS) $(ADD_OBJS)
	$(LINK) $(OBJS) $(ADD_OBJS)

clean:
	rm -f *.o $(PROGRAM) atsimg

zip:
	make clean
	rm -f atsim.zip
	zip atsim.zip atsim.c makefile runme.sh
