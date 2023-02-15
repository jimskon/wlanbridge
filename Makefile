# Make bridge_c

PROGRAMS = bridge testbr

SRCFILES = $(wildcard *.cpp) $(wildcard pdu/*.cpp)
OBJFILES = $(patsubst %.cpp,%.o,$(SRCFILES))

CXX     := g++
CPPFLAGS = -I.
CXXFLAGS = -Wall -Wextra -std=c++11 -O1 $(CPPFLAGS)
CHECK    = cppcheck -q --enable=style,warning $(CPPFLAGS)
CTAGS    = ctags -a
RM       = rm -f


all: $(PROGRAMS)

bridge: bridge.o brmap.o iface.o pdu/pdu.o pdu/pdu_eth.o pdu/pdu_ipv4.o pdu/pdu_udp.o pdu/pdu_tcp.o icmp4.o macaddr.o brmap.h logger.h
	$(CXX) -o $@ $^ -lpthread

testbr: testbr.o brmap.o brmap.h
	$(CXX) -o $@ $^ -lpthread

testsend: testsend.o brmap.o
	$(CXX) -o $@ $^ -lpthread

clean:
	$(RM) $(PROGRAMS) $(OBJFILES)

check:
	$(CHECK) $(SRCFILES)

tags:
	find /usr/include -type f -name "*.h" | xargs $(CTAGS)

.PHONY: all clean check tags


%.o: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ -c $<

