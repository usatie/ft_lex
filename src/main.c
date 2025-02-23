#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define DEFAULT_OUTPUT_FILENAME "lex.yy.c"
#define DUMMY_LEX_YY_C                                                         \
  "#include <stdio.h>\n"                                                       \
  "#include <string.h>\n"                                                      \
  "#include <stdlib.h>\n"                                                      \
  "\n"                                                                         \
  "int main(void) {\n"                                                         \
  "\tprintf(\"Hello, world!\\n\");\n"                                          \
  "\treturn 0;\n"                                                              \
  "}\n"

int write_to_lex_yy_c(const char *src, const char *filename) {
  FILE *fp = fopen(filename, "w");
  if (fp == NULL) {
    perror("fopen");
    return -1;
  }
  size_t src_len = strlen(src);
  if (fwrite(src, sizeof(char), src_len, fp) < src_len) {
    perror("fwrite");
    fclose(fp);
    return -1;
  }
  fclose(fp);
  return 0;
}

int main(void) {
  // TODO: Read, tokenize, parse xxx.l to generate table
  // TODO: Generate NFA from table
  // TODO: Generate DFA from NFA
  // TODO: Generate code from DFA
  // TODO: Write to lex.yy.c
  if (write_to_lex_yy_c(DUMMY_LEX_YY_C, DEFAULT_OUTPUT_FILENAME) < 0) {
    return -1;
  }
  return 0;
}
