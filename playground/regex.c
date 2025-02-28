#include <stdio.h>

int regex_match(const char *pattern, const char *string) {
  const char *s = string;
  while (1) {
    // Pattern exauhsted, it's a match
    if (pattern[0] == '\0') {
      return 1;
    }
    // 1. Wildcard : Try to match the rest of the string
    if (pattern[0] == '.' && s[0]) {
      if (regex_match(pattern + 1, s + 1)) {
        return 1;
      }
    }
    // 2. Normal Character : The first character match, try next
    if (s[0] == pattern[0]) {
      if (regex_match(pattern + 1, s + 1)) {
        return 1;
      }
    }
    // String exhausted, no match
    if (s[0] == '\0') {
      return 0;
    }
    // If it there is no match from here. Try moving to the next
    ++s;
  }
}

int main(int argc, const char *argv[]) {
  if (argc != 3) {
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
