#include <stdbool.h>
#include <stdio.h> // dprintf
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // STDERR_FILENO

#define STATE_EOF 256
#define STATE_WILD 257
#define STATE_SPLIT 258
typedef struct state {
  int c;
  struct state *out;
  struct state *out1;
} state;
int regex_match(state *s, const char *text);
int regex_matchhere(state *s, const char *text);

state *state_new(int c, state *out, state *out1) {
  state *s = malloc(sizeof(state));
  s->c = c;
  s->out = out;
  s->out1 = out1;
  return s;
}

// ab*c*d
// a ->  b*    +--eps-->  c*    +--eps-->  d
//       ^--b--|          ^--c--|

// abc -> abc..
// ab*c -> ab*c..
// a|bcde -> ab|cde...

state *regex_compile(const char *regexp) {
  state head = {};
  state *sp = &head;
  while (*regexp) {
    int c = *regexp;
    if (c == '.') {
      c = STATE_WILD;
    }
    state *new_s;
    if (regexp[1] == '*') {
      new_s = state_new(c, NULL /* to be filled */, NULL /* self */);
      new_s->out = new_s;
      regexp += 2;
    } else {
      new_s = state_new(c, NULL, NULL);
      ++regexp;
    }
    if (sp->out == NULL) {
      sp = sp->out = new_s;
    } else {
      sp = sp->out1 = new_s;
    }
  }
  if (sp->out == NULL) {
    sp = sp->out = state_new(STATE_EOF, NULL, NULL);
  } else {
    sp = sp->out1 = state_new(STATE_EOF, NULL, NULL);
  }
  return head.out;
}

int regex_match(state *s, const char *text) {
  do {
    // check if regexp matches text
    if (regex_matchhere(s, text)) {
      return 1;
    }
  } while (*(text++) != '\0');
  return 0;
}

void append(state **state_list, state *next) {
  // Append state to the list
  int i;
  for (i = 0; state_list[i]; ++i) {
    if (state_list[i] == next) {
      return;
    }
  }
  state_list[i] = next;
  state_list[i + 1] = NULL;
}

state *list1[128] = {}, *list2[128] = {};

void swap(void *a, void *b) {
  void *c = *(void **)a;
  *(void **)a = *(void **)b;
  *(void **)b = c;
}

int regex_matchhere(state *s, const char *text) {
  state **clist = list1, **nlist = list2;
  clist[0] = s;
  while (clist[0]) {
    for (int i = 0; clist[i]; ++i) {
      s = clist[i];
      if (s->c == STATE_EOF) {
        return 1;
      }
      if (s->c == *text || (*text && s->c == STATE_WILD)) {
        // If match, append the next state to the nlist
        if (s->out1 == NULL) {
          append(nlist, s->out);
        } else {
          append(nlist, s->out1);
        }
      }
      if (s->out1) {
        // If there is another branch, try it
        append(nlist, s->out);
      }
    }
    swap(&clist, &nlist);
    nlist[0] = NULL;
    ++text;
  }
  return 0;
}

int main(int argc, const char *argv[]) {
  if (argc != 3) {
    dprintf(STDERR_FILENO, "Usage: %s <pattern> <string>\n", argv[0]);
    return 1;
  }
  const char *regexp = argv[1];
  const char *text = argv[2];

  // First compile the state machine
  state *s = regex_compile(regexp);
  // Second, run the state machine against the input text
  if (regex_match(s, text)) {
    return 0;
  } else {
    return 1;
  }
}
