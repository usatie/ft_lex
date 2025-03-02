#include <stdbool.h>
#include <stdio.h> // dprintf
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // STDERR_FILENO

/*
 * Convert infix regexp re to postfix notation.
 * Insert . as explicit concatenation operator.
 * Cheesy parser, return static buffer.
 */
char *re2post(char *re) {
  int nalt, natom;
  static char buf[8000] = {};
  char *dst;
  // Q. what's nalt and natom?
  struct {
    int nalt;
    int natom;
  } paren[100], *p;

  p = paren;
  dst = buf;
  nalt = 0;
  natom = 0;
  // The maximum length of postfix form is (2 * len - 1),
  // when re is plain text without special characters "abc"
  // because it will converted to "abc.."
  if (strlen(re) >= sizeof(buf) / 2) {
    return NULL;
  }
  for (; *re; re++) {
    switch (*re) {
    case '(':
      if (natom > 1) {
        --natom;
        *dst++ = '.';
      }
      if (p >= paren + 100) {
        return NULL; // Q. what's this? paren buffer size is 100
      }
      // Stash nalt, natom when a parenthesis open
      p->nalt = nalt;
      p->natom = natom;
      p++;
      // Start new nalt, natom counting
      nalt = 0;
      natom = 0;
      break;
    case '|':
      if (natom == 0) {
        return NULL; // natom is ...?
      }
      while (--natom > 0) {
        *dst++ = '.';
      }
      nalt++; // number of alternate
      break;
    case ')':
      if (p == paren) {
        return NULL;
      }
      if (natom == 0) { // what is natom?
        return NULL;
      }
      while (--natom > 0) {
        *dst++ = '.';
      }
      for (; nalt > 0; nalt--) {
        *dst++ = '|';
      }
      --p;
      // Restore nalt, natom
      nalt = p->nalt;
      natom = p->natom;
      // Increment natom, because (...) of itself is natom
      natom++;
      break;
    case '*':
    case '+':
    case '?':
      if (natom == 0) { // natom is an atomic element such as normal
                        // characters and concatenated nodes
        return NULL;
      }
      *dst++ = *re;
      break;
    default:
      if (natom > 1) {
        --natom;
        *dst++ = '.';
      }
      *dst++ = *re;
      natom++;
      break;
    }
    printf("*re = %c, nalt = %d, natom = %d, buf = %s\n", *re, nalt, natom,
           buf);
  }
  // All the parenthesis must be closed
  if (p != paren) {
    return NULL;
  }
  while (--natom > 0) {
    *dst++ = '.';
  }
  for (; nalt > 0; nalt--) {
    *dst++ = '|';
  }
  *dst = 0;
  return buf;
}

enum { Match = 256, Split = 257 };
typedef struct State State;
struct State {
  int c;
  State *out;
  State *out1;
  int lastlist;
};

State matchstate = {Match}; // matching state
int nstate = 0;

/* Allocate and initialize state */
State *state(int c, State *out, State *out1) {
  State *s;

  nstate++;
  s = malloc(sizeof *s);
  s->c = c;
  s->out = out;
  s->out1 = out1;
  s->lastlist = 0;
  return s;
}

/*
 * A partially built NFA without the matching state filled in.
 * Frag.start points at the start state.
 * Frag.out is a list of places that need to be set to the next state for this
 * fragment.
 */
typedef struct Frag Frag;
typedef union Ptrlist Ptrlist;
struct Frag {
  State *start;
  Ptrlist *out;
};

/* Initialize Frag struct. */
Frag frag(State *start, Ptrlist *out) {
  Frag n = {start, out};
  return n;
}

/*
 * Since the out pointers in the list are always
 * uninitialized, we use the pointers themselves
 * as storage for the Ptrlists.
 */
union Ptrlist {
  Ptrlist *next;
  State *s;
};

/* Create singleton list containing just outp */
// これはなんだ？シングルトン？
// outpはState**として渡されているものの、作成直後のstate(c, NULL, NULL)のなかの
// NULLが入っているだけの場所なので、自由な値を一旦入れておいてOK
Ptrlist *list1(State **outp) {
  Ptrlist *l;

  l = (Ptrlist *)outp;
  l->next = NULL;
  return l;
}

/* Patch the list of states at out to point to start. */
// つまりこれはpatchするまではlistとしての体をなしているけど、
// patchした後はただのstateになっていてnextへのポインタは失われてしまうので
// listとして機能しないということ？
void patch(Ptrlist *l, State *s) {
  Ptrlist *next;

  for (; l; l = next) {
    next = l->next;
    // *((State **)l) = s; とおなじ？
    l->s = s;
  }
}

/* Join the two lists l1 and l2, returning the combination. */
Ptrlist *append(Ptrlist *l1, Ptrlist *l2) {
  Ptrlist *oldl1;

  oldl1 = l1;
  while (l1->next) {
    l1 = l1->next;
  }
  l1->next = l2;
  return oldl1;
}

/*
 * Convert postfix regular expression to NFA.
 * Return start state.
 */

State *post2nfa(char *postfix) {
  char *p;
  Frag stack[1000], *stackp, e1, e2, e;
  State *s;

  fprintf(stderr, "postfix: %s\n", postfix);

  if (postfix == NULL) {
    return NULL;
  }
#define push(s) *stackp++ = s
#define pop() *--stackp
  stackp = stack;
  for (p = postfix; *p; p++) {
    switch (*p) {
    default:
      s = state(*p, NULL, NULL); // 'a' みたいなstateを作って、
      push(frag(
          s,
          list1(
              &s->out))); // state('a')のoutはあとで更新してねってことでpushしておく
      break;
    case '.': /* concatenate */
      e2 = pop();
      e1 = pop();
      // Q. e1.out = e2.startみたいにするのとはどう違う?
      patch(
          e1.out,
          e2.start); // e1が既に複合的なstateの可能性があるので、outにすぐにpatchできるといい？
      push(frag(
          e1.start,
          e2.out)); // 既に連結している複数stateも頭とお尻だけ見えるからいいのか？
      break;
    case '|': /* alternate */
      e2 = pop();
      e1 = pop();
      s = state(Split, e1.start, e2.start);
      push(frag(
          s,
          append(
              e1.out,
              e2.out))); // a|b c|d eみたいな分岐がある場合には
                         // a->c
                         // a->d
                         // b->c
                         // b->d
                         // の全ての後ろに次のeを連結したいから、こういう仕組みが必要
    case '?': /* zero or one */
      e = pop();
      s = state(Split, e.start, NULL); // (abc)?dみたいな場合,
                                       // 以下の2つの分岐を試したい s-> (abc) ->
                                       // d s-> d
      push(frag(s, append(e.out, list1(&s->out1))));
      break;
    case '*': /* zero or more */
      e = pop();
      s = state(Split, e.start, NULL); // (abc)*d
                                       // s -> (abc) -> s
                                       // s -> d
      patch(e.out, s);
      push(frag(s, list1(&s->out1)));
      break;
    case '+': /* one or more */
      e = pop();
      s = state(Split, e.start, NULL); // (abc)+dの場合
                                       // abc -> s -> abc
                                       // abc -> s -> d
      patch(e.out, s);
      push(frag(e.start, list1(&s->out1)));
      break;
    }
  }
  e = pop();
  if (stackp !=
      stack) { // stack should be empty after compilation. if not, invalid input
    return NULL;
  }
  patch(e.out, &matchstate);
  return e.start;
#undef pop
#undef push
}

typedef struct List List;
struct List {
  State **s;
  int n;
};

List l1, l2;
static int listid;

void addstate(List *, State *);
void step(List *, int, List *);

/* Compute initial state list */
List *startlist(State *start, List *l) {
  l->n = 0;
  listid++;
  addstate(l, start);
  return l;
}

/* Check whether state list contains a match. */
int ismatch(List *l) {
  int i;

  for (i = 0; i < l->n; i++) {
    if (l->s[i] == &matchstate) {
      return 1;
    }
  }
  return 0;
}

/* Add s to l, following unlabeled arrows. */
void addstate(List *l, State *s) {
  if (s == NULL || s->lastlist == listid) {
    return;
  }
  s->lastlist = listid;
  if (s->c == Split) {
    /* follow unlabled arrows */
    addstate(l, s->out);
    addstate(l, s->out1);
    return;
  }
  l->s[l->n++] = s;
}

/*
 * Step the NFA from the states in clist
 * past the character c,
 * to create next NFA state set nlist.
 */
void step(List *clist, int c, List *nlist) {
  int i;
  State *s;
  listid++;
  nlist->n = 0;
  for (i = 0; i < clist->n; i++) {
    s = clist->s[i];
    if (s->c == c) {
      // TODO: Implement wildcard
      addstate(nlist, s->out);
    }
  }
}

/* Run NFA to determine whether it matches s. */
int match(State *start, char *s) {
  int i, c;
  List *clist, *nlist, *t;

  clist = startlist(start, &l1);
  nlist = &l2;
  for (; *s; s++) {
    c = *s & 0xFF; // to prevent sign extension
    step(clist, c, nlist);
    t = clist;
    clist = nlist;
    nlist = t; /* swap clist, nlist */
  }
  // Q. There isn't early exit in the for loop?
  // Q. Match against whole string?
  return ismatch(clist);
}

int main(int argc, char **argv) {
  int i;
  char *post;
  State *start;

  if (argc < 3) {
    fprintf(stderr, "usage: nfa regexp string...\n");
    return 1;
  }

  post = re2post(argv[1]);
  if (post == NULL) {
    fprintf(stderr, "bad regexp %s\n", argv[1]);
    return 1;
  }

  start = post2nfa(post);
  if (start == NULL) {
    fprintf(stderr, "error in post2nfa %s\n", post);
    return 1;
  }

  l1.s = malloc(nstate * sizeof(l1.s[0]));
  l2.s = malloc(nstate * sizeof(l2.s[0]));
  for (i = 2; i < argc; i++) {
    if (match(start, argv[i])) {
      printf("%s\n", argv[i]);
    }
  }
  return 0;
}
