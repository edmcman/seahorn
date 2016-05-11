#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

void __VERIFIER_error() {
  printf("__VERIFIER_error was executed\n");
  exit(1);
}

void __VERIFIER_assume(int x) {
  assert(x);
}

void __seahorn_extern_stub (char *fname) {
  if (fname)
    printf ("An extern stub %s was unexpectedly executed by the counterexample.\n", fname);
  else
    printf ("An unknown extern stub was unexpectedly executed by the counterexample.\n");

}

#define get_value_helper(ctype, llvmtype)                               \
  ctype __seahorn_get_value_ ## llvmtype (int ctr, ctype *g_arr, int g_arr_sz) { \
    assert (ctr < g_arr_sz && "Unexpected index");                      \
    printf("__seahorn_get_value_" #llvmtype " returning %lx.\n", (long) (g_arr[ctr])); \
    return g_arr[ctr];                                                  \
  }

#define get_value_int(bits) get_value_helper(int ## bits ## _t, i ## bits)

get_value_int(64)
get_value_int(32)
get_value_int(16)
get_value_int(8)

get_value_helper(intptr_t, ptr)
