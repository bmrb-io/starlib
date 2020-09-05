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

# set up compiler and options


# Define this to turn off position bits (line/col numbers)
# in DataValueNodes (removes some data and saves memory)
# Comment this off to re-enable line/col numbers on each
# DataValueNode at the expense of some memory:
#
# You need to define this in all the code that
# includes this libraries header files if you
# want to have this effect.  That includes
# code that uses the library too.
MEM_TRIM_FLAGS =  -DNO_POSBITS_DATAVALUENODE

#-----------------------------------------------
#-----------------------------------------------
#            OS-specific settings:
#     Instructions:
#         Choose a platform - either Irix or Linux
#         by uncommenting the appropriate section.
#         If you switch from Irix to Linux or visa
#         versa you should do a 'make clean' to
#         clear out the old object files.  Otherwise
#         you end up trying to link Linux .o's with
#         Irix .o's and that won't work.
#-----------------------------------------------
#-----------------------------------------------


# ----- [ BEGIN Settings for Irix: --------
# ---- Comment out to use another OS: -------
# -- C-PLUS-PLUS compiler:
#CPP = CC
#YACC = /usr/freeware/bin/bison
#LEX = /usr/freeware/bin/flex -8
#LEXLIB = -ll
#   BITNESS - The bittitude of the target binary.  Must match the
#   same bitness as the application using this library file:
#   ----------------------------------------------------------------------
#   For newer SGI 64-bit machines (o2, o200 and up) : BITNESS = -mips4 -64
#   For older SGI 32-bit machines (Indigo, Indy): BITNESS = -mips3 -n32
#   For a generic machine, to ignore it, set it null: BITNESS =
#
#    Pick one of the BITNESSes to uncomment:
#BITNESS = -mips4 -64
#BITNESS = -mips3 -n32
#BITNESS = -mips2 -32
#CPPFLAGS = -g -ptnone -I. -L. -w $(BITNESS) -DIS_IRIX $(MEM_TRIM_FLAGS)
#INSTDIR=/bmrb/irix/bin
# ----- ] END Settings for Irix: --------



# ------- [ Begin settings for Sun/Solairs with egcs++/bison/flex (not CC) -----
#CPP = g++
#YACC = bison
#LEX = flex -8
#LEXLIB = -lfl
#CPPFLAGS = -g -w -I. -L. -DIS_SOLARIS -DIS_GCC $(MEM_TRIM_FLAGS)
#INSTDIR=../../localbin
# ------- ] END settings for Sun/Solaris: --------



# ------- [ Begin settings for RedHat 5.2 Linux: --------------
# -- Might work also on other Linuxen, but I do not have the
# -- resources to test this to find out.  Please e-mail
# -- me with updates if something needs tweaking for another
# -- platform.
CPP = g++
#CPP = g++-3.0
CPPFLAGS = -ggdb -w -Wall -I. -L.  -DIS_LINUX -DIS_GCC $(MEM_TRIM_FLAGS)
#CPPFLAGS = -g -w -I. -L.  -DIS_LINUX -DIS_GCC $(MEM_TRIM_FLAGS)
YACC = bison
LEX = flex -8
LEXLIB = -lfl
INSTDIR=../../localbin
# ------- ] END Settings for Linux: --------

# ------- Flags for other OS's to be added here: -------------
# ...
# ...
# ...



# --------------------------------------------------
# -- Start of non OS-specific settings: ------------
# --------------------------------------------------

#purify tools
PCOV = /usr/local/pure/purify/purecov
#PUR  = /usr/local/pure/purify/purify -m71-engine -g++
PUR  = /usr/local/pure/purify/purify

LINKLIBS = -lstar -$(LEXLIB)

# PARSEVERBOSITY = -v
PARSEVERBOSITY =


# set to "-DDEBUG" to activate debugging lex/yacc in detail:
# PARSEDEBUG =  -DDEBUG
PARSEDEBUG =

# set to "-DDEBUG" to activate debugging of the Unparse output:
# (Prints pointers' addresses and datatypes as it unparses.)
# UNPARSEDEBUG = -DDEBUG
# UNPARSEDEBUG = -DDEBUGSIZEOF
UNPARSEDEBUG = 

# set to "-d" to activate lex dumping of info for excessive
# debugging info.
# LEXDUMP = -d
LEXDUMP = 


all :	showCompilerVersion libstar.a 

showCompilerVersion:
	- @echo "Showing compiler version on this machine:"
	- $(CPP) -v
	- @echo "(Useful for diagnosing problems with NMI builds)."
	- @echo ""
	- @echo ""

libstarpure.a: parser.pure.o \
	    scanner.pure.o \
	    parser.pure.o \
	    ast.pure.o \
	    message.pure.o \
	    scopeTagList.pure.o \
	    list.pure.o \
	    formatNMRSTAR.pure.o \
	    astlist.pure.o \
	    unparse.pure.o \
	    template.pure.o \
	    starErrReport.pure.o 
	rm -f libstarpure.a
	ar -rcsv libstarpure.a \
	    scanner.pure.o \
	    parser.pure.o \
	    ast.pure.o \
	    message.pure.o \
	    scopeTagList.pure.o \
	    list.pure.o \
	    astlist.pure.o \
	    unparse.pure.o \
	    starErrReport.pure.o

libstar.a: parser.o \
	    scanner.o \
	    parser.o \
	    ast.o \
	    message.o \
	    scopeTagList.o \
	    list.o \
	    formatNMRSTAR.o \
	    astlist.o \
	    unparse.o \
	    template.o \
	    starErrReport.o 
	rm -f libstar.a
	ar -rcsv libstar.a \
	    scanner.o \
	    parser.o \
	    ast.o \
	    message.o \
	    scopeTagList.o \
	    list.o \
	    formatNMRSTAR.o \
	    astlist.o \
	    unparse.o \
	    starErrReport.o

#----------------------- Non-purified versions ----------------------

examples: libstar.a
	echo "Entering examples directory, making:" ; cd examples ; make

main.o:	main.cc scanner.h parser.h gram.tab.h ast.h scopeTagList.h transforms.h
	$(CPP) $(CPPFLAGS) -c main.cc -o main.o

parser.o: parser.cc parser.h ast.h gram.tab.h gram.tab.c message.h lex.yy.c
	$(CPP) $(PARSEDEBUG) $(CPPFLAGS) -c parser.cc -o parser.o

scanner.o: lex.yy.c scanner.h ast.h gram.tab.h message.h 
	$(CPP) $(PARSEDEBUG) $(CPPFLAGS) -c lex.yy.c -o scanner.o

starErrReport.o: starErrReport.cc scanner.h
	$(CPP) $(CPPFLAGS) -c starErrReport.cc -o starErrReport.o

lex.yy.c: star.l list.h astlist.h
	  $(LEX) $(LEXDUMP) star.l ; 
	  @echo  "Warning: This is an ugly hack to get around a bug "
	  @echo  "         in flex that won't let you define YYLMAX until "
	  @echo  "         after it has already been used to declare yytext's "
	  @echo  "         size.  Here I force the define to come at the front "
	  @echo  "         no matter what flex puts out. "
	  echo "#define YYLMAX 4194304" | cat - lex.yy.c > lex.yy.c.tmp 
	  mv lex.yy.c.tmp lex.yy.c

formatNMRSTAR.o: formatNMRSTAR.cc astnode.h ast.h
	$(CPP) $(CPPFLAGS) -c formatNMRSTAR.cc -o formatNMRSTAR.o
	
list.o: list.h list.cc astnode.h
	$(CPP) $(CPPFLAGS) -c list.cc -o list.o

astlist.o: astlist.h astlist.cc astnode.h list.o
	$(CPP) $(CPPFLAGS) -c astlist.cc -o astlist.o

unparse.o:unparse.cc list.h astlist.h ast.h
	$(CPP) $(CPPFLAGS) $(UNPARSEDEBUG) -c unparse.cc -o unparse.o

template.o: template.h template.cc ast.h list.h astlist.h astlist.cc list.cc
	$(CPP) $(CPPFLAGS) -c template.cc -o template.o

message.o: message.cc message.h scanner.h ast.h gram.tab.h
	$(CPP) $(CPPFLAGS) -c message.cc -o message.o

parser.h: scanner.h ast.h
	touch parser.h

ast.o: ast.h ast.cc
	$(CPP) $(CPPFLAGS) $(UNPARSEDEBUG) -c ast.cc -o ast.o

ast.h:	scanner.h list.h astlist.h
	touch ast.h

scopeTagList.o:scopeTagList.cc scopeTagList.h
	$(CPP) $(CPPFLAGS) -c scopeTagList.cc -o scopeTagList.o

convert.o: ast.h convert.cc interface.h
	$(CPP) $(CPPFLAGS) -c convert.cc -o convert.o

#----------------------- Purified versions ----------------------

main.pure.o:	main.cc scanner.h parser.h gram.tab.h ast.h scopeTagList.h transforms.h
	$(PUR) $(CPP) $(CPPFLAGS) -c main.cc -o main.pure.o

parser.pure.o: parser.cc parser.h ast.h gram.tab.h gram.tab.c message.h lex.yy.c
	$(PUR) $(CPP) $(PARSEDEBUG) $(CPPFLAGS) -c parser.cc -o parser.pure.o

scanner.pure.o: lex.yy.c scanner.h ast.h gram.tab.h message.h 
	$(PUR) $(CPP) $(PARSEDEBUG) $(CPPFLAGS) -c lex.yy.c -o scanner.pure.o

starErrReport.pure.o: starErrReport.cc scanner.h
	$(PUR) $(CPP) $(CPPFLAGS) -c starErrReport.cc -o starErrReport.pure.o

lex.yy.c.pure: star.l list.h astlist.h
	  $(LEX) $(LEXDUMP) star.l 
	  @echo  "Warning: This is an ugly hack to get around a bug "
	  @echo  "       in flex that won't let you define YYLMAX until "
	  @echo  "       after it has already been used to declare yytext's "
	  @echo  "       size.  Here I force the define to come at the front "
	  @echo  "       no matter what flex puts out. "
	  echo "#define YYLMAX 261420" | cat - lex.yy.c > lex.yy.c.tmp 
	  mv lex.yy.c.tmp lex.yy.c

list.pure.o: list.h list.cc astnode.h
	$(PUR) $(CPP) $(CPPFLAGS) -c list.cc -o list.pure.o

astlist.pure.o: astlist.h astlist.cc astnode.h list.pure.o
	$(PUR) $(CPP) $(CPPFLAGS) -c astlist.cc -o astlist.pure.o

unparse.pure.o:unparse.cc list.h astlist.h ast.h
	$(PUR) $(CPP) $(CPPFLAGS) $(UNPARSEDEBUG) -c unparse.cc -o unparse.pure.o

template.pure.o: template.h template.cc ast.h list.h astlist.h astlist.cc list.cc
	$(PUR) $(CPP) $(CPPFLAGS) -c template.cc -o template.pure.o

message.pure.o: message.cc message.h scanner.h ast.h gram.tab.h
	$(PUR) $(CPP) $(CPPFLAGS) -c message.cc -o message.pure.o

ast.pure.o: ast.h ast.cc
	$(PUR) $(CPP) $(CPPFLAGS) $(UNPARSEDEBUG) -c ast.cc -o ast.pure.o

scopeTagList.pure.o:scopeTagList.cc scopeTagList.h
	$(PUR) $(CPP) $(CPPFLAGS) -c scopeTagList.cc -o scopeTagList.pure.o

convert.pure.o: ast.h convert.cc interface.h
	$(PUR) $(CPP) $(CPPFLAGS) -c convert.cc -o convert.pure.o

###
# the -b flag causes bison to give its output the given name
# the -d flag causes bison to output gram.tab.h
###
gram.tab.c:	star.y star.l scanner.h
		$(YACC) $(PARSEVERBOSITY) -b gram -d star.y

gram.tab.h:	star.y star.l scanner.h
		$(YACC) $(PARSEVERBOSITY) -b gram -d star.y

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

clean: clean_examples
	- /bin/rm -r -f *.o star star.pure star.purecov \
	      gram.tab.h gram.tab.c lex.yy.c *.pcv *.output \
	      *.a *~ s2nmr ii_files

clean_examples:
	- echo "cleaning examples directory..." ; cd examples ; make clean ; cd ..

doc++:
	doc++ -u -H -b -S -B doc/html/starlib_banner.html -d doc/html *.h

###
# install - dummy do nothing
###

install: libstar.a
