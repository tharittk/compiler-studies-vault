##===- tools/tchecker/Makefile -----------------------------*- Makefile -*-===##

LEVEL = ../..
TOOLNAME = tchecker
#BUILT_SOURCES = lex.yy.cpp fun.tab.cpp
USEDLIBS = Visitor.a AST.a

include $(LEVEL)/Makefile.common

#lex.yy.cpp: fun.l fun.y fun.tab.cpp
#	flex -o $@ $<

#fun.tab.cpp: fun.y
#	bison --report=all -o fun.tab.cpp -d $<

#clean::
#	rm -rf lex.yy.cpp fun.tab.cpp fun.tab.hpp fun.output
