# Targets & general dependencies
PROGRAM = atsim
HEADERS = 
OBJS = atsim.o airport.o atcprint.o pqueue.o
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

atsimg: atsim.o airport2.o atcprint.o pqueue.o
	$(LINK) atsim.o airport2.o atcprint.o pqueue.o

lib: atcsim_gold.o 
	 ar -r libatcsim.a atcsim_gold.o 

clean:
	rm -f *.o $(PROGRAM) atsimg

zip:
	make clean
	rm -f atsim.zip
	zip atsim.zip *
