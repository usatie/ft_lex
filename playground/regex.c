#include <stdio.h>  // dprintf
#include <unistd.h> // STDERR_FILENO

struct machine {};

struct machine *regex_compile(const char *regexp) { return NULL; }

int regex_match(struct machine *m, const char *text) { return 0; }

int main(int argc, const char *argv[]) {
  if (argc != 3) {
    dprintf(STDERR_FILENO, "Usage: %s <pattern> <string>\n", argv[0]);
    return 1;
  }
  const char *regexp = argv[1];
  const char *text = argv[2];

  // First compile the state machine
  struct machine *m = regex_compile(regexp);
  // Second, run the state machine against the input text
  if (regex_match(m, text)) {
    return 0;
  } else {
    return 1;
  }
}
