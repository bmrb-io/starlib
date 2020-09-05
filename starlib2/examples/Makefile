
# set up compiler and options

# TO PICK A PLATFORM: Uncomment the lines in the Irix section
# or the Linux section. Note that the sections are delimited
# by "[ BEGIN" and "] END" so you can use parenthesis-matching
# on the square braces to see the section if your editor supports that.

# Currently, those are the only two platforms supported, however
# the Linux version is designed for egcs, so it should port to
# other platforms using the egcs compiler with only a minimal
# amount of work.


MEM_TRIM_FLAGS = -DNO_POSBITS_DATAVALUENODE

#    ----- [ BEGIN settings for Irix: --------
#    ---- Comment out to use another OS: -------
#  C-PLUS-PLUS compiler:
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
#    ---- ] END settings for Irix: -----

# ------- [ Begin settings for Sun/Solairs with egcs++/bison/flex (not CC) -----
#CPP = g++
#YACC = bison
#LEX = flex -8
#LEXLINK = -lfl
#CPPFLAGS = -g -w -I. -L. -DIS_SOLARIS -DIS_GCC $(MEM_TRIM_FLAGS)


# ------- ] END settings for Linux: --------
#     ---- [ BEGIN Settings for Linux: --------------
#     (Warning, by default this creates Pentium binaries
#     that won't run on a 386 or a 486.)
#
CPP = g++
#CPP = g++-3.0
#CPPFLAGS = -g -w -I. -L. -DIS_LINUX -DIS_GCC $(MEM_TRIM_FLAGS)
CPPFLAGS = -ggdb -w -Wall -I. -L. -DIS_LINUX -DIS_GCC $(MEM_TRIM_FLAGS)
TEMPLATE_CC =
LEXLINK = -lfl
#     ---- ] END Settings for Linux: --------------

#purify tools
PCOV = /usr/local/pure/purify/purecov
#PUR  = /usr/local/pure/purify/purify -m71-engine -g++
PUR  = /usr/local/pure/purify/purify 

#
#  CHANGE THIS IF COMPILING SOMEWHERE OTHER THAN THE INSTALL
#  DIRECTORY SO IT POINTS AT THE RIGHT PLACE FOR STARLIB:
STARLIBDIR = ..

# For the starlib header files, and any other extra headers:
INCLUDEDIRS = $(STARLIBDIR)

# For the starlib library:
LINKDIRS = $(STARLIBDIR)
LINKLIBS = ../template.o -lstar $(LEXLINK)
LINKLIBSPURE = ../template.pure.o -lstarpure $(LEXLINK)

all : fix_nmrstar3_entry_IDs syntax_check search passthru hash_tags copy star_syntax formatNMRSTAR formatNMRSTAR3 simple_string stardiff star_grep_helper star_dump_values remove_loop_column

clean : 
	- @echo "rm -rf ii_files" | sh ; \
	- @echo "rm -f *.o" | sh ;
	- @rm fix_nmrstar3_entry_IDs syntax_check search passthru copy star_syntax formatNMRSTAR formatNMRSTAR3 simple_string stardiff find_experiment_cases star_grep_helper star_dump_values hash_tags

# -----------------
star3_to_tab: star3_to_tab.o $(STARLIBDIR)/libstar.a
	$(CPP) $(CPPFLAGS) star3_to_tab.o -o star3_to_tab -L$(LINKDIRS) $(LINKLIBS)

star3_to_tab.o: star3_to_tab.cc
	$(CPP) $(CPPFLAGS) -I$(INCLUDEDIRS) -c $(TEMPLATE_CC) star3_to_tab.cc -o star3_to_tab.o

star_dump_values: star_dump_values.o $(STARLIBDIR)/libstar.a
	$(CPP) $(CPPFLAGS) star_dump_values.o -o star_dump_values -L$(LINKDIRS) $(LINKLIBS)

star_dump_values.o: star_dump_values.cc $(STARLIBDIR)/libstar.a
	$(CPP) $(CPPFLAGS) -I$(INCLUDEDIRS) -c $(TEMPLATE_CC) star_dump_values.cc -o star_dump_values.o

# -----------------
star_grep_helper: star_grep_helper.o $(STARLIBDIR)/libstar.a
	$(CPP) $(CPPFLAGS) star_grep_helper.o -o star_grep_helper -L$(LINKDIRS) $(LINKLIBS)

star_grep_helper.o: star_grep_helper.cc $(STARLIBDIR)/libstar.a
	$(CPP) $(CPPFLAGS) -I$(INCLUDEDIRS) -c $(TEMPLATE_CC) star_grep_helper.cc -o star_grep_helper.o

# -----------------
syntax_check: syntax_check.o $(STARLIBDIR)/libstar.a
	$(CPP) $(CPPFLAGS) syntax_check.o -o syntax_check -L$(LINKDIRS) $(LINKLIBS)

syntax_check.o: syntax_check.cc $(STARLIBDIR)/libstar.a
	$(CPP) $(CPPFLAGS) -I$(INCLUDEDIRS) -c $(TEMPLATE_CC) syntax_check.cc -o syntax_check.o

# -----------------
search:   search.o $(STARLIBDIR)/libstar.a
	$(CPP) $(CPPFLAGS) search.o -o search -L$(LINKDIRS) $(LINKLIBS)

search.o: search.cc
	$(CPP) $(CPPFLAGS) -I$(INCLUDEDIRS) -c $(TEMPLATE_CC) search.cc -o search.o

# -----------------
passthru:   passthru.o $(STARLIBDIR)/libstar.a
	$(CPP) $(CPPFLAGS) passthru.o -o passthru -L$(LINKDIRS) $(LINKLIBS)

passthru.o: passthru.cc
	$(CPP) $(CPPFLAGS) -I$(INCLUDEDIRS) -c $(TEMPLATE_CC) passthru.cc -o passthru.o

# -----------------
experiment_grep:   experiment_grep.o $(STARLIBDIR)/libstar.a
	$(CPP) $(CPPFLAGS) experiment_grep.o -o experiment_grep -L$(LINKDIRS) $(LINKLIBS)

experiment_grep.o: experiment_grep.cc
	$(CPP) $(CPPFLAGS) -I$(INCLUDEDIRS) -c $(TEMPLATE_CC) experiment_grep.cc -o experiment_grep.o

passthru.pure:   passthru.pure.o $(STARLIBDIR)/libstarpure.a
	$(PUR) $(CPP) $(CPPFLAGS) passthru.pure.o -o passthru.pure.o -L$(LINKDIRS) $(TEMPLATE_PURE_O) $(LINKLIBSPURE)

passthru.pure.o: passthru.cc
	$(PUR) $(CPP) $(CPPFLAGS) -I$(INCLUDEDIRS) -c $(TEMPLATE_CC) passthru.cc -o passthru.pure.o

# -----------------
hash_tags:   hash_tags.o $(STARLIBDIR)/libstar.a
	$(CPP) $(CPPFLAGS) hash_tags.o -o hash_tags -L$(LINKDIRS) $(LINKLIBS)

hash_tags.o: hash_tags.cc
	$(CPP) $(CPPFLAGS) -I$(INCLUDEDIRS) -c $(TEMPLATE_CC) hash_tags.cc -o hash_tags.o


# -----------------
formatNMRSTAR:   formatNMRSTAR.o $(STARLIBDIR)/libstar.a
	$(CPP) $(CPPFLAGS) formatNMRSTAR.o -o formatNMRSTAR -L$(LINKDIRS) $(LINKLIBS)

formatNMRSTAR.o: formatNMRSTAR.cc
	$(CPP) $(CPPFLAGS) -I$(INCLUDEDIRS) -c $(TEMPLATE_CC) formatNMRSTAR.cc -o formatNMRSTAR.o

# -----------------
formatNMRSTAR3:   formatNMRSTAR3.o $(STARLIBDIR)/libstar.a
	$(CPP) $(CPPFLAGS) formatNMRSTAR3.o -o formatNMRSTAR3 -L$(LINKDIRS) $(LINKLIBS)

formatNMRSTAR3.o: formatNMRSTAR3.cc
	$(CPP) $(CPPFLAGS) -I$(INCLUDEDIRS) -c $(TEMPLATE_CC) formatNMRSTAR3.cc -o formatNMRSTAR3.o

# -----------------
stardiff:   stardiff.o $(STARLIBDIR)/libstar.a
	$(CPP) $(CPPFLAGS) stardiff.o -o stardiff -L$(LINKDIRS) $(LINKLIBS)

stardiff.o: stardiff.cc
	$(CPP) $(CPPFLAGS) -I$(INCLUDEDIRS) -c $(TEMPLATE_CC) stardiff.cc -o stardiff.o

# -----------------
star_syntax:   star_syntax.o $(STARLIBDIR)/libstar.a
	$(CPP) $(CPPFLAGS) star_syntax.o -o star_syntax -L$(LINKDIRS) $(LINKLIBS)

star_syntax.o: star_syntax.cc
	$(CPP) $(CPPFLAGS) -I$(INCLUDEDIRS) -c $(TEMPLATE_CC) star_syntax.cc -o star_syntax.o

# -----------------
copy:   copy.o $(STARLIBDIR)/libstar.a
	$(CPP) $(CPPFLAGS) copy.o -o copy -L$(LINKDIRS) $(LINKLIBS)

copy.o: copy.cc
	$(CPP) $(CPPFLAGS) -I$(INCLUDEDIRS) -c $(TEMPLATE_CC) copy.cc -o copy.o


# -----------------
test1:	test1.o $(STARLIBDIR)/libstar.a
	$(CPP) $(CPPFLAGS) test1.o -o test1 -L$(LINKDIRS) $(LINKLIBS)

test1.o:	test1.cc
	$(CPP) $(CPPFLAGS) -I$(INCLUDEDIRS) -c $(TEMPLATE_CC) test1.cc -o test1.o

# -----------------
test2:	test2.o $(STARLIBDIR)/libstar.a
	$(CPP) $(CPPFLAGS) test2.o -o test2 -L$(LINKDIRS) $(LINKLIBS)

test2.o:	test2.cc
	$(CPP) $(CPPFLAGS) -I$(INCLUDEDIRS) -c $(TEMPLATE_CC) test2.cc -o test2.o

# -----------------
test3:	test3.o $(STARLIBDIR)/libstar.a
	$(CPP) $(CPPFLAGS) test3.o -o test3 -L$(LINKDIRS) $(LINKLIBS)

test3.o:	test3.cc
	$(CPP) $(CPPFLAGS) -I$(INCLUDEDIRS) -c $(TEMPLATE_CC) test3.cc -o test3.o

# -----------------
test_del: test_del.o $(STARLIBDIR)/libstar.a
	$(CPP) $(CPPFLAGS) test_del.o -o test_del -L$(LINKDIRS) $(LINKLIBS)

test_del.o: test_del.cc
	$(CPP) $(CPPFLAGS) -I$(INCLUDEDIRS) -c $(TEMPLATE_CC) test_del.cc -o test_del.o

# -----------------
simple_string:   simple_string.o $(STARLIBDIR)/libstar.a
	$(CPP) $(CPPFLAGS) simple_string.o -o simple_string -L$(LINKDIRS) $(LINKLIBS)

simple_string.o: simple_string.cc
	$(CPP) $(CPPFLAGS) -I$(INCLUDEDIRS) -c $(TEMPLATE_CC) simple_string.cc -o simple_string.o

# -----------------
find_experiment_cases:   find_experiment_cases.o $(STARLIBDIR)/libstar.a
	$(CPP) $(CPPFLAGS) find_experiment_cases.o -o find_experiment_cases -L$(LINKDIRS) $(LINKLIBS)

find_experiment_cases.o: find_experiment_cases.cc
	$(CPP) $(CPPFLAGS) -I$(INCLUDEDIRS) -c $(TEMPLATE_CC) find_experiment_cases.cc -o find_experiment_cases.o

# -----------------
fix_nmrstar3_entry_IDs: fix_nmrstar3_entry_IDs.o $(STARLIBDIR)/libstar.a
	$(CPP) $(CPPFLAGS) fix_nmrstar3_entry_IDs.o -o fix_nmrstar3_entry_IDs -L$(LINKDIRS) $(LINKLIBS)

fix_nmrstar3_entry_IDs.o: fix_nmrstar3_entry_IDs.cc $(STARLIBDIR)/libstar.a
	$(CPP) $(CPPFLAGS) -I$(INCLUDEDIRS) -c $(TEMPLATE_CC) fix_nmrstar3_entry_IDs.cc -o fix_nmrstar3_entry_IDs.o

# -----------------
remove_loop_column: remove_loop_column.o $(STARLIBDIR)/libstar.a
	$(CPP) $(CPPFLAGS) remove_loop_column.o -o remove_loop_column -L$(LINKDIRS) $(LINKLIBS)

remove_loop_column.o: remove_loop_column.cc
	$(CPP) $(CPPFLAGS) -I$(INCLUDEDIRS) -c $(TEMPLATE_CC) remove_loop_column.cc -o remove_loop_column.o
