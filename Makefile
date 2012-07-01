CC = g++
CFLAGS = -ggdb -Wall

CALC_OBJS = calc.o
BLOOMTEST_OBJS = util.o BloomFilterTest.o 
FLAT_OBJS = util.o flat.o
IP2MAC_OBJS = util.o ip2mac.o
CLICK_OBJS = util.o click.o
PACKET_OBJS = util.o packet.o
OVERLAP_OBJS = util.o overlap.o
LSFIB_OBJS = util.o lsfib.o
MACTRACE_OBJS = util.o mactrace.o
CBF_OBJS = util.o cbf.o

.PHONY: all clean 

all: clean bloomtest flat overlap click packet lsfib

.cc.o:
	$(CC) $(CFLAGS) -c $<

.c.o:
	$(CC) $(CFLAGS) -c $<

cbf: $(CBF_OBJS)
	$(CC) $(CFLAGS) -o cbf $(CBF_OBJS) -lnsl -pg

bloomtest: $(BLOOMTEST_OBJS)
	$(CC) $(CFLAGS) -o bloomtest $(BLOOMTEST_OBJS) -lnsl -pg

flat: $(FLAT_OBJS)
	$(CC) $(CFLAGS) -o flat $(FLAT_OBJS) 

IP2MAC: $(IP2MAC_OBJS)
	$(CC) $(CFLAGS) -o IP2MAC $(IP2MAC_OBJS) 

click: $(CLICK_OBJS)
	$(CC) $(CFLAGS) -o click $(CLICK_OBJS) 

packet: $(PACKET_OBJS)
	$(CC) $(CFLAGS) -o packet $(PACKET_OBJS) 

overlap: $(OVERLAP_OBJS)
	$(CC) $(CFLAGS) -o overlap $(OVERLAP_OBJS) 

lsfib: $(LSFIB_OBJS)
	$(CC) $(CFLAGS) -o lsfib $(LSFIB_OBJS) 

mactrace: $(MACTRACE_OBJS)
	$(CC) $(CFLAGS) -o mactrace $(MACTRACE_OBJS) 


clean:
	rm -f *.o *~ calc bloomtest IP2MAC click packet overlap lsfib mactrace flat cbf
