#include <stdio.h>

int regex_match(const char *pattern, const char *string) {
  const char *s = string, *p = pattern;
  while (1) {
    // Pattern exauhsted, it's a match
    if (p[0] == '\0') {
      return 1;
    } else if (s[0] == '\0') {
      // 1. String exhausted, no match
      return 0;
    } else if (p[0] == '.') {
      // 2. Wildcard : Try to match the rest of the string
      ++p, ++s;
    } else if (s[0] == p[0]) {
      // 3. Normal Character : The first character match, try next
      ++p, ++s;
    } else {
      // No match from the current string
      return regex_match(pattern, string + 1);
    }
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
