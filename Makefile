# on Archlinux you need to install wxgtk and apparently also wxgtx2.8
# the code is not maintained anymore so there will be probably many warnings
# casued by deprecated methods


CPPFLAGS = -Wall -Wcast-align -O2 -mtune=native -c 
DEFINES = 
WXCONFIG = wx-config-2.8
WXFLAGS = `${WXCONFIG} --cppflags`
WXLIBS = `${WXCONFIG} --libs`

OBJS = BilliardObject.o Billiard.o main.o

all: Billiard

Billiard: $(OBJS)
	g++ -o Billiard $(OBJS) $(WXLIBS)

Billiard.o: Billiard.h Billiard.cpp
	g++ $(CPPFLAGS) $(WXFLAGS) $(DEFINES) Billiard.cpp

BilliardObject.o: BilliardObject.h BilliardObject.cpp
	g++ $(CPPFLAGS) $(WXFLAGS) $(DEFINES) BilliardObject.cpp

main.o: Billiard.h main.cpp
	g++ $(CPPFLAGS) $(WXFLAGS) main.cpp

clean:
	rm -f *.o

