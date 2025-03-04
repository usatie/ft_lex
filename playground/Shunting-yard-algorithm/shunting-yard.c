#include <stdbool.h>
#include <stdio.h>
#include <string.h>

// Operators
// precedence | operator | associativity
// 4          | !        | right
// 3          | * / %    | left
// 2          | + -      | left
// 1          | =        | right

int op_preced(char c) {
  switch (c) {
  case '!':
    return 4;
  case '*':
  case '/':
  case '%':
    return 3;
  case '+':
  case '-':
    return 2;
  case '=':
    return 1;
  default:
    return 0;
  }
}

// Associativity:
//   e.g. - is left associative
//   `9 - 5 - 1` is `(9 - 5) - 1`, and not `9 - (5 - 1)`.
//
//   e.g. ^ is right associative
//   `2 ^ 3 ^ 2` is `2^(3^2) = 2^9`, not `(2^3)^2 = 8^2`.
//
//   e.g. = is right associative
//   `a = b = c` is `a = (b = c)`, not `(a = b) = c`
//
//   e.g. ! is right associative
//   `!!a` is `!(!a)`, not  `(!!)a`
bool op_left_assoc(char c) {
  switch (c) {
  // Left Associativity
  case '*':
  case '/':
  case '%':
  case '+':
  case '-':
    return true;
  // Right Associativity
  case '=':
  case '!':
    return false;
  default:
    return false;
  }
}

unsigned int op_arg_count(char c) {
  switch (c) {
  case '*':
  case '/':
  case '%':
  case '+':
  case '-':
  case '=':
    return 2;
  case '!':
    return 1;
  default:
    return 0;
  }
}

#define is_operator(c)                                                         \
  (c == '+' || c == '-' || c == '*' || c == '/' || c == '%' || c == '!' ||     \
   c == '=')
#define is_ident(c) ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'z'))

bool shunting_yard(const char *input, char *output) {
  const char *strpos = input, *strend = input + strlen(input);
  char c, *outpos = output;

  char stack[32];      // operator stack
  unsigned int sl = 0; // stack len (depth)
  char sc;             // stack element record

  while (strpos < strend) {
    c = *strpos++;
    if (c == ' ') {
      continue;
    } else if (is_ident(c)) {
      // If token is identifier, add to output queue
      *outpos++ = c;
    } else if (is_operator(c)) {
      while (sl > 0) {
        sc = stack[sl - 1];
        // (is_left_assic(op1) AND prec(op1) <= prec(op2))
        // OR
        // prec(op1) < prec(op2)

        // why is_operator needed? because '(' and so on is also on the stack
        if (is_operator(sc) &&
            ((op_left_assoc(c) && (op_preced(c) <= op_preced(sc)) ||
              (op_preced(c) < op_preced(sc))))) {
          *outpos++ = sc;
          sl--;
        } else {
          break;
        }
      }
      stack[sl++] = c;
    } else if (c == '(') {
      stack[sl++] = c;
    } else if (c == ')') {
      bool pe = false;
      // pop operators until the stack top becomes '('
      while (sl > 0) {
        sc = stack[--sl];
        if (sc == '(') {
          // pop from the stack, but not to output queue
          pe = true;
          break;
        } else {
          *outpos++ = sc;
        }
      }
      // If there is not opening parenthesis, this is error
      if (!pe) {
        fprintf(stderr, "Error: No matching parenthesis\n");
        return false;
      }
    } else {
      fprintf(stderr, "Error: Unknown token%c\n", c);
      return false;
    }
  }

  // Consumed all tokens, and if there are tokens left on the stack, output them
  while (sl > 0) {
    sc = stack[--sl];
    if (sc == '(' || sc == ')') {
      fprintf(stderr, "Error: No matching parenthesis\n");
      return false;
    }
    *outpos++ = sc;
  }
  *outpos = '\0'; // Null-terminate
  return true;
}

int main(int argc, const char *argv[]) {
  char output[128];
  for (int i = 1; i < argc; ++i) {
    const char *input = argv[i];
    printf("Input: '%s'\n", input);
    if (shunting_yard(input, output)) {
      printf("Output: '%s'\n", output);
    } else {
      printf("Error in shunting yard algorithm.\n");
    }
  }
  return 0;
}
