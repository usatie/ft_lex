#include <stdio.h>
#include <unistd.h> // STDERR_FILENO

int regex_matchhere(const char *regexp, const char *text);
int regex_matchplus(char c, const char *regexp, const char *text);
int regex_matchstar(char c, const char *regexp, const char *text);

int regex_match(const char *regexp, const char *text) {
  do {
    // check if regexp matches text
    if (regex_matchhere(regexp, text)) {
      return 1;
    }
  } while (*(text++) != '\0');
  return 0;
}

int regex_matchhere(const char *regexp, const char *text) {
  // pattern exhausted, it's a match
  if (regexp[0] == '\0') {
    return 1;
  }
  // kleene's star
  if (regexp[1] == '*') {
    return regex_matchstar(regexp[0], regexp + 2, text);
  }
  if (regexp[1] == '+') {
    return regex_matchplus(regexp[0], regexp + 2, text);
  }
  // single character match
  if (text[0] && (regexp[0] == text[0] || regexp[0] == '.')) {
    return regex_matchhere(regexp + 1, text + 1);
  }
  return 0;
}

// p+ = pp*
int regex_matchplus(char c, const char *regexp, const char *text) {
  if (*text && (c == *text || c == '.')) {
    return regex_matchstar(c, regexp, text + 1);
  }
  return 0;
}

int regex_matchstar(char c, const char *regexp, const char *text) {
  do {
    if (regex_matchhere(regexp, text)) {
      return 1;
    }
  } while (*text != '\0' && (*(text++) == c || c == '.'));
  return 0;
}

int main(int argc, const char *argv[]) {
  if (argc != 3) {
    dprintf(STDERR_FILENO, "Usage: %s <pattern> <string>\n", argv[0]);
    return 1;
  }

  const char *regexp = argv[1];
  const char *text = argv[2];
  if (regex_match(regexp, text)) {
    return 0;
  } else {
    return 1;
  }
}
