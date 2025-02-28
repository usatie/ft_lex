#include <stdio.h>

int regex_match(const char *pattern, const char *string) {
  const char *p = string;
  do {
    // Pattern exauhsted, it's a match
    if (pattern[0] == '\0') {
      return 1;
    }
    // The first character match, try next
    if (p[0] == pattern[0]) {
      if (regex_match(pattern + 1, p + 1)) {
        return 1;
      }
    }
    // If it there is no match from here. Try moving to the next
    ++p;
  } while (*p);
  return 0;
}

int main(int argc, const char *argv[]) {
  if (argc != 3) {
    printf("Usage: %s <pattern> <string>\n", argv[0]);
    return 1;
  }

  const char *pattern = argv[1];
  const char *string = argv[2];
  if (regex_match(pattern, string)) {
    printf("Match\n");
  } else {
    printf("No match\n");
  }
}
