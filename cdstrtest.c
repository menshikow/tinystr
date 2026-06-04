#include "cdstr.h"

Err test_init(String *s_out, const char *s_in) {}

Err test_append(String *s, const char *slice) {}

Err test_start() {
  if (test_init == SUCCESS) {
    return SUCCESS;
  } else {
    return Err;
  }

  return SUCCESS;
}

int main(void) {

  if (test_start() == SUCCESS) {
    print("All tests are passed");
    return 1;
  } else {
    return 0;
  }
}
