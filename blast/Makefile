#
# use GNU make
# need: expat-devel, flex, glibc-static (or link without "-static")
#

STARLIBDIR = ../../starlib2
STAR_H = -I$(STARLIBDIR)

CXX = g++
LD = g++
CXXFLAGS = -Wall -Wno-deprecated -I. $(STAR_H) -DIS_GCC -DNDEBUG
#CXXFLAGS = -Wall -Wno-deprecated -I. $(STAR_H) -DIS_GCC -D_DEBUG -ggdb
LDFLAGS = -static
LIBS = -L$(STARLIBDIR) $(STARLIBDIR)/template.o -lstar -lexpat -ll

OBJECTS = BlastParser.o HitList.o HitDef.o

all: update-entry2 update-entry3

update-entry2: update-entry2.o $(OBJECTS)
	$(LD) $(LDFLAGS) -o update-entry2 update-entry2.o $(OBJECTS) $(LIBS)

update-entry2.o: update-entry2.cc

update-entry3: update-entry3.o $(OBJECTS) formatNMRSTAR3.o
	$(LD) $(LDFLAGS) -o update-entry3 update-entry3.o $(OBJECTS) formatNMRSTAR3.o $(LIBS)

update-entry3.o: update-entry3.cc

#BmrbEntry.o: BmrbEntry.cc BmrbEntry.h
BlastParser.o: BlastParser.cc BlastParser.h
HitList.o: HitList.cc HitList.h
HitDef.o: HitDef.cc HitDef.h
formatNMRSTAR3.o: formatNMRSTAR3.cc

.PHONY: clean
clean:
	- rm *.o update-entry2 update-entry3 core.*

%.o : %.cc %.h
	$(CXX) -c $(CXXFLAGS) -o $@ $<
