# Targets & general dependencies
PROGRAM = atsim
HEADERS = 
OBJS = atsim.o airport2.o atcprint.o pqueue.o
SRC = atsim.c airport2.c atcprint.c pqueue.c makefile runme.sh \
      airport.h atcprint.h pqueue.h
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
	$(LINK) $(OBJS) $(ADD_OBJS) -l pthread

atsimg: atsim3.o airport2.o atcprint.o pqueue3.o utils.o
	$(LINK) atsim3.o airport2.o atcprint.o pqueue3.o utils.o -l pthread

lib: atcsim_gold.o 
	 ar -r libatcsim.a atcsim_gold.o 

clean:
	rm -f *.o $(PROGRAM) atsimg

zip:
	make clean
	rm -f atsim2.zip
	zip atsim2.zip $(SRC)
