#include <stdbool.h>
#include <stdio.h>
#include <string.h>

bool verbose = false;
int regex_star(char c, const char *pattern, const char *string);

int regex_match(const char *pattern, const char *string) {
  if (verbose) {
    printf("regex_match(\"%s\", \"%s\")\n", pattern, string);
  }
  const char *s = string, *p = pattern;
  while (1) {
    // Pattern exauhsted, it's a match
    if (p[0] == '\0') {
      return 1;
    } else if (p[1] == '*') {
      // 1. Star : Try all s+i where s[i] matches p[0]
      if (regex_star(p[0], p + 2, s)) {
        return 1;
      } else if (s[0] == '\0') {
        return 0;
      } else {
        p = pattern;
        s = ++string;
      }
    } else if (s[0] == '\0') {
      // 2. String exhausted, no match
      return 0;
    } else if (p[0] == '.') {
      // 3. Wildcard : Try to match the rest of the string
      ++p, ++s;
    } else if (s[0] == p[0]) {
      // 4. Normal Character : The first character match, try next
      ++p, ++s;
    } else {
      // No match from the string, advance
      p = pattern;
      s = ++string;
    }
  }
}

// ('a', "b", "c")
int regex_star(char c, const char *pattern, const char *string) {
  if (verbose) {
    printf("regex_star(%c, \"%s\", \"%s\")\n", c, pattern, string);
  }
  const char *s = string;
  do {
    if (regex_match(pattern, s)) {
      return 1;
    }
    if (c == '.' || c == *s) {
      ++s;
    } else {
      return 0;
    }
  } while (*s);
}

int main(int argc, const char *argv[]) {
  if (argc == 4 && strcmp(argv[3], "-v") == 0) {
    verbose = true;
  } else if (argc != 3) {
    printf("Usage: %s <pattern> <string>\n", argv[0]);
    return 1;
  }

  const char *pattern = argv[1];
  const char *string = argv[2];
  if (regex_match(pattern, string)) {
    return 0;
  } else {
    return 1;
  }
}
