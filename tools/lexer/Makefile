##===- tools/lexer/Makefile --------------------------------*- Makefile -*-===##

LEVEL = ../..
TOOLNAME = lexer
BUILT_SOURCES = lex.yy.cpp

include $(LEVEL)/Makefile.common

lex.yy.cpp: fun.l tokens.h
	flex -o $@ $<

clean::
	rm -rf lex.yy.cpp
