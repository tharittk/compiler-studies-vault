#ifndef FUN_AST_SRCLOC_H
#define FUN_AST_SRCLOC_H

namespace fun {

// Source location information
struct SrcLoc {
  // line is source line number, col is source column number
  SrcLoc(int line, int col) : line(line), col(col) {}
  int line;
  int col;
};

}

#endif
