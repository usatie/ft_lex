#include <stdbool.h>
#include <stdio.h> // dprintf
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // STDERR_FILENO

enum { Match = 256, Split = 257 };
typedef struct State State;
struct State {
  int c;
  State *out;
  State *out1;
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

int main(void) {}
