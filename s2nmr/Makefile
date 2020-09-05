####
# This Makefile can be used to make the following:
#	star.purecov (default)	This is the "pure coverage" version of the
#				example program that parses a STAR file
#
#	star.pure		Purified version of the same program.
#
#	star			Plain version of the same program (will
#				run faster than the previous two versions).
#
#	test			Run the program with sample input files.
#
#	view			View pure coverage results.
#
#	clean			Remove generated files.
#
###

MEM_TRIM_FLAGS = -DNO_POSBITS_DATAVALUENODE

#    ----- [ BEGIN settings for Irix: --------
#    ---- Comment out to use another OS: -------
# C-PLUS-PLUS compiler:
#CPP = CC
#LEXLINK = -ll
#TEMPLATE_CC =
#
#    BITNESS - The bittitude of the target binary.  Must match the
#    same bitness as the application using this library file:
#    ----------------------------------------------------------------------
#    For newer SGI 64-bit machines (o2, o200 and up) : BITNESS = -mips4 -64
#    For older SGI 32-bit machines (Indigo, Indy): BITNESS = -mips3 -n32
#    For a generic machine, to ignore it, set it null: BITNESS =
#
#BITNESS = -mips4 -64
#BITNESS = -mips3 -n32
#BITNESS = -mips2 -32
#CPPFLAGS = -ptnone -I. -L. -w $(BITNESS) -DIS_IRIX $(MEM_TRIM_FLAGS)
#INSTBINDIR=/bmrb/irix/bin
#    ---- ] END settings for Irix: -----

# ------- [ Begin settings for Sun/Solairs with egcs++/bison/flex (not CC) -----
#CPP = g++
#CPPFLAGS = -g -w -I. -L. -DIS_SOLARIS -DIS_GCC $(MEM_TRIM_FLAGS)
#TEMPLATE_CC =
#LEXLINK = -lfl
#INSTBINDIR=../../localbin
#    ---- ] END settings for Solaris: -----

#     ---- [ BEGIN Settings for Linux: --------------
#     (Warning, by default this creates Pentium binaries
#     that won't run on a 386 or a 486.)
#
CC = gcc
CPP = g++
CPPFLAGS = -g -w -I. -L. -DIS_LINUX -DIS_GCC $(MEM_TRIM_FLAGS) 
TEMPLATE_CC =
LEXLINK = -lfl
REGEXLINK = 
INSTBINDIR=/bmrb/linux/bin
#     ---- ] END Settings for Linux: --------------

STARLIBDIR = ../starlib2

#purify tools
PCOV = /usr/local/pure/purify/purecov
PUR  = /usr/local/pure/purify/purify -m71-engine -g++

#Bruce Parens' "electric fence" malloc debugger link in
#  WARNING: DO NOT USE THIS IN PRODUCTION CODE.  IT IS A
# HORRIBLE MEMORY HOG!!!!!  FOR DEBUGGING ONLY!!!  USING
# THIS ADDS A PAGE OF MEMORY AFTER EACH MALLOC OR NEW'ED
# CHUNK.
#  Set to -lefence to activate electric fence, set to
#  a nullstring to leave it out.
#EFLIB = -lefence
EFLIB =


# ------------------------------------------------------
# Shouldn't have to modify below this line for porting.
# ------------------------------------------------------

LIBS = -static $(STARLIBDIR)/template.o -L$(STARLIBDIR) -lstar $(LEXLINK) $(EFLIB) $(REGEXLINK)
INCS = -I$(STARLIBDIR)

all: s2nmr  make_order_from_template annotation_export

annotation_export:	s2nmr postprocess_adit.str 
	tar cvf annotation_export.tar s2nmr postprocess_adit.str

make_order_from_template:	make_order_from_template.o
	$(CPP) $(CPPFLAGS) \
	make_order_from_template.o \
	$(LIBS) \
	-o make_order_from_template

make_order_from_template.o:	make_order_from_template.cc
	$(CPP) $(CPPFLAGS) $(INCS) \
	-c make_order_from_template.cc \
	-o make_order_from_template.o
	
s2nmr: main.o \
       transform-aux.o \
       transform1a.o \
       transform1b.o \
       transform1c.o \
       transform1d.o \
       transform2a.o \
       transform2b.o \
       transform2c.o \
       transform3a.o \
       transform3b.o \
       transform3c.o \
       transform5.o \
       transform6.o \
       transform7.o \
       transform8.o \
       transform9.o \
       transform10.o \
       transform11.o \
       transform12.o \
       transform13.o \
       transform14.o \
       transform15.o \
       transform16.o \
       transform17.o \
       transform18.o \
       transform19.o \
       transform20.o \
       transform21.o \
       transform22.o \
       transform23.o \
       transform24.o \
       transform25.o \
       transform26.o \
       transform26a.o \
       transform27.o \
       transform28.o \
       transform29.o \
       transform30.o \
       transform31.o \
       transform32.o \
       transform33.o \
       transform34.o \
       transform35.o \
       transform36.o \
       transform37.o \
       transform38.o \
       transform39.o \
       transform40.o \
       transform41.o \
       transform42.o \
       transform43.o \
       inv_transform43.o \
       transform44.o \
       transform45.o \
       transform46.o \
       transform47.o \
       transform48.o \
       transform49.o \
       transform50.o \
       transform51.o \
       transform52.o \
       transform53.o \
       transform54.o \
       transform55.o \
       transform56.o \
       transform57.o \
       transform58.o \
       transform59.o \
       transform60.o \
       transform61.o \
       transform62.o \
       transform63.o \
       transform64.o \
       transform65.o \
       transform66.o \
       transform67.o \
       transform68.o \
       transform69.o \
       transform70.o \
       transform71.o \
       transform72.o \
       transform73.o \
       transform74.o \
       transform75.o \
       transform76.o \
       transform77.o \
       transform78.o \
       transformII-1.o \
       transformII-2.o \
       transformII-1-2.o \
       transformIII-3.o \
       $(STARLIBDIR)/template.o \
       $(STARLIBDIR)/libstar.a \
       Makefile
	$(CPP)  $(CPPFLAGS) $(INCS) \
		main.o \
       		transform-aux.o \
       		transform1a.o \
       		transform1b.o \
       		transform1c.o \
       		transform1d.o \
	        transform2a.o \
	        transform2b.o \
	        transform2c.o \
	        transform3a.o \
	        transform3b.o \
        	transform3c.o \
        	transform5.o \
        	transform6.o \
        	transform7.o \
        	transform8.o \
        	transform9.o \
	        transform10.o \
	        transform11.o \
	        transform12.o \
	        transform13.o \
	        transform14.o \
	        transform15.o \
	        transform16.o \
	        transform17.o \
	        transform18.o \
		transform19.o \
		transform20.o \
		transform21.o \
		transform22.o \
		transform23.o \
		transform24.o \
		transform25.o \
		transform26.o \
		transform26a.o \
		transform27.o \
		transform28.o \
		transform29.o \
		transform30.o \
		transform31.o \
		transform32.o \
		transform33.o \
		transform34.o \
		transform35.o \
		transform36.o \
		transform37.o \
		transform38.o \
		transform39.o \
		transform40.o \
		transform41.o \
		transform42.o \
		transform43.o \
		inv_transform43.o \
		transform44.o \
		transform45.o \
		transform46.o \
		transform47.o \
		transform48.o \
		transform49.o \
		transform50.o \
	        transform51.o \
	        transform52.o \
	        transform53.o \
	        transform54.o \
	        transform55.o \
	        transform56.o \
	        transform57.o \
	        transform58.o \
	        transform59.o \
	        transform60.o \
	        transform61.o \
	        transform62.o \
	        transform63.o \
	        transform64.o \
	        transform65.o \
	        transform66.o \
	        transform67.o \
	        transform68.o \
	        transform69.o \
	        transform70.o \
	        transform71.o \
	        transform72.o \
	        transform73.o \
	        transform74.o \
	        transform75.o \
	        transform76.o \
	        transform77.o \
	        transform78.o \
 	        transformII-1.o \
        	transformII-2.o \
        	transformII-1-2.o \
        	transformIII-3.o \
		$(LIBS) \
		-o s2nmr

install: s2nmr
	cp s2nmr $(INSTBINDIR) ; cp g2nmr.str $(INSTBINDIR)
	cp postprocess_adit.str $(INSTBINDIR)
	cp 7bitcheck $(INSTBINDIR)

tarball: s2nmr
	- mkdir tar_tmp
	- mkdir tar_tmp/bin/
	- mkdir tar_tmp/etc/
	cp s2nmr tar_tmp/bin 
	cp \
	    nmrstr3.0-nmrstr2.1_devel.str \
	    nmrstr3.0-nmrstr2.1_prod.str \
	    nmrstr2.1-nmrstr3.0.str  \
	        tar_tmp/etc/
	cd tar_tmp/ && \
	    ( tar cvf - \
	        bin/s2nmr \
		etc/nmrstr3.0-nmrstr2.1_devel.str \
		etc/nmrstr3.0-nmrstr2.1_prod.str \
		etc/nmrstr2.1-nmrstr3.0.str  \
	            | gzip - > ../s2nmr.tar.gz ) && \
	    cd ..
	rm -rf tar_tmp/

s2nmr.pure: \
	main.o \
	transform-aux.o \
	transform1a.o \
	transform1b.o \
	transform1c.o \
	transform1d.o \
	transform2a.o \
	transform2b.o \
	transform2c.o \
	transform3a.o \
	transform3b.o \
	transform3c.o \
	transform5.o \
	transform6.o \
	transform7.o \
	transform8.o \
	transform9.o \
	transform10.o \
	transform11.o \
	transform12.o \
	transform13.o \
	transform14.o \
	transform15.o \
	transform16.o \
	transform17.o \
	transform18.o \
	transform19.o \
	transform20.o \
	transform21.o \
	transform22.o \
	transform23.o \
	transform24.o \
	transform25.o \
	transform26.o \
	transform26a.o \
	transform27.o \
	transform28.o \
	transform29.o \
	transform30.o \
	transform31.o \
	transform32.o \
	transform33.o \
	transform34.o \
	transform35.o \
	transform36.o \
	transform37.o \
	transform38.o \
	transform39.o \
	transform40.o \
	transform41.o \
	transform42.o \
	transform43.o \
	inv_transform43.o \
	transform44.o \
	transform45.o \
	transform46.o \
	transform47.o \
	transform48.o \
	transform49.o \
	transform50.o \
	transform51.o \
	transform52.o \
	transform53.o \
	transform54.o \
	transform55.o \
	transform56.o \
	transform57.o \
	transform58.o \
	transform59.o \
	transform60.o \
	transform61.o \
	transform6t.o \
	transformII-1.o \
	transformII-2.o \
	transformII-1-2.o \
	transformIII-3.o \
	$(STARLIBDIR)/template.o \
	$(STARLIBDIR)/libstar.a \
	Makefile
	$(PUR) $(CPP)  $(CPPFLAGS) $(INCS) \
		main.o \
       		transform-aux.o \
       		transform1a.o \
       		transform1b.o \
       		transform1c.o \
       		transform1d.o \
	        transform2a.o \
	        transform2b.o \
	        transform2c.o \
	        transform3a.o \
	        transform3b.o \
        	transform3c.o \
        	transform5.o \
        	transform6.o \
        	transform7.o \
        	transform8.o \
        	transform9.o \
	        transform10.o \
	        transform11.o \
	        transform12.o \
	        transform13.o \
	        transform14.o \
	        transform15.o \
	        transform16.o \
	        transform17.o \
	        transform18.o \
		transform19.o \
		transform20.o \
		transform21.o \
		transform22.o \
		transform23.o \
		transform24.o \
		transform25.o \
		transform26.o \
		transform26a.o \
		transform27.o \
		transform28.o \
		transform29.o \
		transform30.o \
		transform31.o \
		transform32.o \
		transform33.o \
		transform34.o \
		transform35.o \
		transform36.o \
		transform37.o \
		transform38.o \
		transform39.o \
		transform40.o \
		transform41.o \
		transform42.o \
		transform43.o \
		inv_transform43.o \
		transform44.o \
		transform45.o \
		transform46.o \
		transform47.o \
		transform48.o \
		transform49.o \
		transform50.o \
		transform51.o \
		transform52.o \
		transform53.o \
		transform54.o \
		transform55.o \
		transform56.o \
		transform57.o \
		transform58.o \
		transform59.o \
		transform60.o \
		transform61.o \
		transform62.o \
		transform63.o \
		transform64.o \
		transform65.o \
		transform66.o \
		transform67.o \
		transform68.o \
		transform69.o \
	        transform70.o \
	        transform71.o \
	        transform72.o \
	        transform73.o \
	        transform74.o \
	        transform75.o \
	        transform76.o \
	        transform77.o \
	        transform78.o \
 	        transformII-1.o \
        	transformII-2.o \
        	transformII-1-2.o \
        	transformIII-3.o \
		$(LIBS) \
		-o s2nmr.pure

main.o:	main.cc
	$(CPP) $(CPPFLAGS) $(INCS) -c main.cc -o main.o

transform1a.o:transform1a.cc transforms.h
	$(CPP) $(CPPFLAGS) $(INCS) -c transform1a.cc -o transform1a.o

transform1b.o:transform1b.cc transforms.h
	$(CPP) $(CPPFLAGS) $(INCS) -c transform1b.cc -o transform1b.o

transform1c.o:transform1c.cc transforms.h
	$(CPP) $(CPPFLAGS) $(INCS) -c transform1c.cc -o transform1c.o

transform1d.o:transform1d.cc transforms.h
	$(CPP) $(CPPFLAGS) $(INCS) -c transform1d.cc -o transform1d.o

transform2.o:transform2.cc transforms.h
	$(CPP) $(CPPFLAGS) $(INCS) -c transform2.cc -o transform2.o

transform2a.o:transform2a.cc transforms.h
	$(CPP) $(CPPFLAGS) $(INCS) -c transform2a.cc -o transform2a.o

transform2b.o:transform2b.cc transforms.h
	$(CPP) $(CPPFLAGS) $(INCS) -c transform2b.cc -o transform2b.o

transform2c.o:transform2c.cc transforms.h
	$(CPP) $(CPPFLAGS) $(INCS) -c transform2c.cc -o transform2c.o

transform3a.o:transform3a.cc transforms.h
	$(CPP) $(CPPFLAGS) $(INCS) -c transform3a.cc -o transform3a.o

transform3b.o:transform3b.cc transforms.h
	$(CPP) $(CPPFLAGS) $(INCS) -c transform3b.cc -o transform3b.o

transform3c.o:transform3c.cc transforms.h
	$(CPP) $(CPPFLAGS) $(INCS) -c transform3c.cc -o transform3c.o

transform5.o:transform5.cc transforms.h
	$(CPP) $(CPPFLAGS) $(INCS) -c transform5.cc -o transform5.o

transform6.o:transform6.cc transforms.h
	$(CPP) $(CPPFLAGS) $(INCS) -c transform6.cc -o transform6.o

transform7.o:transform7.cc transforms.h
	$(CPP) $(CPPFLAGS) $(INCS) -c transform7.cc -o transform7.o

transform8.o:transform8.cc transforms.h
	$(CPP) $(CPPFLAGS) $(INCS) -c transform8.cc -o transform8.o

transform9.o:transform9.cc transforms.h
	$(CPP) $(CPPFLAGS) $(INCS) -c transform9.cc -o transform9.o

transform10.o:transform10.cc transforms.h
	$(CPP) $(CPPFLAGS) $(INCS) -c transform10.cc -o transform10.o

transform11.o:transform11.cc transforms.h
	$(CPP) $(CPPFLAGS) $(INCS) -c transform11.cc -o transform11.o

transform12.o:transform12.cc transforms.h
	$(CPP) $(CPPFLAGS) $(INCS) -c transform12.cc -o transform12.o

transform13.o:transform13.cc transforms.h
	$(CPP) $(CPPFLAGS) $(INCS) -c transform13.cc -o transform13.o

transform14.o:transform14.cc transforms.h
	$(CPP) $(CPPFLAGS) $(INCS) -c transform14.cc -o transform14.o

transform15.o:transform15.cc transforms.h
	$(CPP) $(CPPFLAGS) $(INCS) -c transform15.cc -o transform15.o

transform16.o:transform16.cc transforms.h
	$(CPP) $(CPPFLAGS) $(INCS) -c transform16.cc -o transform16.o

transform17.o:transform17.cc transforms.h
	$(CPP) $(CPPFLAGS) $(INCS) -c transform17.cc -o transform17.o

transform18.o:transform18.cc transforms.h
	$(CPP) $(CPPFLAGS) $(INCS) -c transform18.cc -o transform18.o
	
transform19.o:transform19.cc transforms.h
	$(CPP) $(CPPFLAGS) $(INCS) -c transform19.cc -o transform19.o
	
transform20.o:transform20.cc transforms.h
	$(CPP) $(CPPFLAGS) $(INCS) -c transform20.cc -o transform20.o
	
transform21.o:transform21.cc transforms.h
	$(CPP) $(CPPFLAGS) $(INCS) -c transform21.cc -o transform21.o

transform22.o:transform22.cc transforms.h
	$(CPP) $(CPPFLAGS) $(INCS) -c transform22.cc -o transform22.o
	
transform23.o:transform23.cc transforms.h
	$(CPP) $(CPPFLAGS) $(INCS) -c transform23.cc -o transform23.o	

transform24.o:transform24.cc transforms.h
	$(CPP) $(CPPFLAGS) $(INCS) -c transform24.cc -o transform24.o

transform25.o:transform25.cc transforms.h
	$(CPP) $(CPPFLAGS) $(INCS) -c transform25.cc -o transform25.o

transform26.o:transform26.cc transforms.h
	$(CPP) $(CPPFLAGS) $(INCS) -c transform26.cc -o transform26.o

transform26a.o:transform26a.cc transforms.h
	$(CPP) $(CPPFLAGS) $(INCS) -c transform26a.cc -o transform26a.o
	
transform27.o:transform27.cc transforms.h
	$(CPP) $(CPPFLAGS) $(INCS) -c transform27.cc -o transform27.o
	
transform28.o:transform28.cc transforms.h
	$(CPP) $(CPPFLAGS) $(INCS) -c transform28.cc -o transform28.o
	
transform29.o:transform29.cc transforms.h
	$(CPP) $(CPPFLAGS) $(INCS) -c transform29.cc -o transform29.o
	
transform30.o:transform30.cc transforms.h
	$(CPP) $(CPPFLAGS) $(INCS) -c transform30.cc -o transform30.o
	
transform31.o:transform31.cc transforms.h
	$(CPP) $(CPPFLAGS) $(INCS) -c transform31.cc -o transform31.o
	
transform32.o:transform32.cc transforms.h
	$(CPP) $(CPPFLAGS) $(INCS) -c transform32.cc -o transform32.o
	
transform33.o:transform33.cc transforms.h
	$(CPP) $(CPPFLAGS) $(INCS) -c transform33.cc -o transform33.o

transform34.o:transform34.cc transforms.h
	$(CPP) $(CPPFLAGS) $(INCS) -c transform34.cc -o transform34.o
	
transform35.o:transform35.cc transforms.h
	$(CPP) $(CPPFLAGS) $(INCS) -c transform35.cc -o transform35.o
	
transform36.o:transform36.cc transforms.h
	$(CPP) $(CPPFLAGS) $(INCS) -c transform36.cc -o transform36.o
	
transform37.o:transform37.cc transforms.h
	$(CPP) $(CPPFLAGS) $(INCS) -c transform37.cc -o transform37.o
	
transform38.o:transform38.cc transforms.h
	$(CPP) $(CPPFLAGS) $(INCS) -c transform38.cc -o transform38.o
	
transform39.o:transform39.cc transforms.h
	$(CPP) $(CPPFLAGS) $(INCS) -c transform39.cc -o transform39.o
	
transform40.o:transform40.cc transforms.h
	$(CPP) $(CPPFLAGS) $(INCS) -c transform40.cc -o transform40.o
	
transform41.o:transform41.cc transforms.h
	$(CPP) $(CPPFLAGS) $(INCS) -c transform41.cc -o transform41.o

transform42.o:transform42.cc transforms.h
	$(CPP) $(CPPFLAGS) $(INCS) -c transform42.cc -o transform42.o

transform43.o:transform43.cc transforms.h
	$(CPP) $(CPPFLAGS) $(INCS) -c transform43.cc -o transform43.o

inv_transform43.o:inv_transform43.cc transforms.h
	$(CPP) $(CPPFLAGS) $(INCS) -c inv_transform43.cc -o inv_transform43.o

transform44.o:transform44.cc transforms.h
	$(CPP) $(CPPFLAGS) $(INCS) -c transform44.cc -o transform44.o

transform45.o:transform45.cc transforms.h
	$(CPP) $(CPPFLAGS) $(INCS) -c transform45.cc -o transform45.o

transform46.o:transform46.cc transforms.h
	$(CPP) $(CPPFLAGS) $(INCS) -c transform46.cc -o transform46.o

transform47.o:transform47.cc transforms.h
	$(CPP) $(CPPFLAGS) $(INCS) -c transform47.cc -o transform47.o

transform48.o:transform48.cc transforms.h
	$(CPP) $(CPPFLAGS) $(INCS) -c transform48.cc -o transform48.o

transform49.o:transform49.cc transforms.h
	$(CPP) $(CPPFLAGS) $(INCS) -c transform49.cc -o transform49.o

transform50.o:transform50.cc transforms.h
	$(CPP) $(CPPFLAGS) $(INCS) -c transform50.cc -o transform50.o

transform51.o:transform51.cc transforms.h
	$(CPP) $(CPPFLAGS) $(INCS) -c transform51.cc -o transform51.o

transform52.o:transform52.cc transforms.h
	$(CPP) $(CPPFLAGS) $(INCS) -c transform52.cc -o transform52.o

transform53.o:transform53.cc transforms.h
	$(CPP) $(CPPFLAGS) $(INCS) -c transform53.cc -o transform53.o

transform54.o:transform54.cc transforms.h
	$(CPP) $(CPPFLAGS) $(INCS) -c transform54.cc -o transform54.o

transform55.o:transform55.cc transforms.h
	$(CPP) $(CPPFLAGS) $(INCS) -c transform55.cc -o transform55.o

transform56.o:transform56.cc transforms.h
	$(CPP) $(CPPFLAGS) $(INCS) -c transform56.cc -o transform56.o

transform57.o:transform57.cc transforms.h
	$(CPP) $(CPPFLAGS) $(INCS) -c transform57.cc -o transform57.o

transform58.o:transform58.cc transforms.h
	$(CPP) $(CPPFLAGS) $(INCS) -c transform58.cc -o transform58.o

transform59.o:transform59.cc transforms.h
	$(CPP) $(CPPFLAGS) $(INCS) -c transform59.cc -o transform59.o

transform60.o:transform60.cc transforms.h
	$(CPP) $(CPPFLAGS) $(INCS) -c transform60.cc -o transform60.o

transform61.o:transform61.cc transforms.h
	$(CPP) $(CPPFLAGS) $(INCS) -c transform61.cc -o transform61.o

transform62.o:transform62.cc transforms.h
	$(CPP) $(CPPFLAGS) $(INCS) -c transform62.cc -o transform62.o

transform63.o:transform63.cc transforms.h
	$(CPP) $(CPPFLAGS) $(INCS) -c transform63.cc -o transform63.o

transform64.o:transform64.cc transforms.h
	$(CPP) $(CPPFLAGS) $(INCS) -c transform64.cc -o transform64.o

transform65.o:transform65.cc transforms.h
	$(CPP) $(CPPFLAGS) $(INCS) -c transform65.cc -o transform65.o

transform66.o:transform66.cc transforms.h
	$(CPP) $(CPPFLAGS) $(INCS) -c transform66.cc -o transform66.o

transform67.o:transform67.cc transforms.h
	$(CPP) $(CPPFLAGS) $(INCS) -c transform67.cc -o transform67.o

transform68.o:transform68.cc transforms.h
	$(CPP) $(CPPFLAGS) $(INCS) -c transform68.cc -o transform68.o

transform69.o:transform69.cc transforms.h
	$(CPP) $(CPPFLAGS) $(INCS) -c transform69.cc -o transform69.o

transform70.o:transform70.cc transforms.h
	$(CPP) $(CPPFLAGS) $(INCS) -c transform70.cc -o transform70.o

transform71.o:transform71.cc transforms.h
	$(CPP) $(CPPFLAGS) $(INCS) -c transform71.cc -o transform71.o

transform72.o:transform72.cc transforms.h
	$(CPP) $(CPPFLAGS) $(INCS) -c transform72.cc -o transform72.o

transform73.o:transform73.cc transforms.h
	$(CPP) $(CPPFLAGS) $(INCS) -c transform73.cc -o transform73.o

transform74.o:transform74.cc transforms.h
	$(CPP) $(CPPFLAGS) $(INCS) -c transform74.cc -o transform74.o

transform75.o:transform75.cc transforms.h
	$(CPP) $(CPPFLAGS) $(INCS) -c transform75.cc -o transform75.o

transform76.o:transform76.cc transforms.h
	$(CPP) $(CPPFLAGS) $(INCS) -c transform76.cc -o transform76.o

transform77.o:transform77.cc transforms.h
	$(CPP) $(CPPFLAGS) $(INCS) -c transform77.cc -o transform77.o

transform78.o:transform78.cc transforms.h
	$(CPP) $(CPPFLAGS) $(INCS) -c transform78.cc -o transform78.o

transformII-1.o:transformII-1.cc transforms.h
	$(CPP) $(CPPFLAGS) $(INCS) -c transformII-1.cc -o transformII-1.o

transformII-2.o:transformII-2.cc transforms.h
	$(CPP) $(CPPFLAGS) $(INCS) -c transformII-2.cc -o transformII-2.o

transformII-1-2.o:transformII-1-2.cc transforms.h
	$(CPP) $(CPPFLAGS) $(INCS) -c transformII-1-2.cc -o transformII-1-2.o

transformIII-3.o:transformIII-3.cc transforms.h
	$(CPP) $(CPPFLAGS) $(INCS) -c transformIII-3.cc -o transformIII-3.o

transform-aux.o:transform-aux.cc transforms.h
	$(CPP) $(CPPFLAGS) $(INCS) -c transform-aux.cc -o transform-aux.o

convert.o: ast.h convert.cc interface.h
	$(CPP) $(CPPFLAGS) $(INCS) -c convert.cc -o convert.o


###
# run purecov version on test input: data
###
test:	star.purecov
	-star.purecov data

###
# view pure coverage results
###
view:	star.purecov.pcv
	$(PCOV) -view star.purecov.pcv


###
# clean up
###

clean:
	/bin/rm -rf *.o star star.pure star.purecov \
	      *.pcv *.output *.a s2nmr ii_files

# For finding misspelled tags in the mapper files:
# With this and vim's quickfix, I can jump to the offending line numbers
# like I was compiling a program,
# just ":make tagspellcheck" from inside 'vim':
tagspellcheck:
	find_misspelt_tags  -tag quoted3 nmrstr3.0-nmrstr2.1_devel.str -tag bare3 /share/madings/oracle/create_schema_3/elu_tmp_development/adit_item_tbl_o.csv | grep -iv '[_\.]temp_' | grep -iv '[_\.]tmp[^_]' | grep -iv 'have_no_idea'

