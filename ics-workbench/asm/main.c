#include "asm.h"
#include <assert.h>
#include <stdio.h>

int main() {
  int res2[5]={0};
  int res3[5]={1,1,1,1,1};
  asm_jmp_buf buf;
  int r = asm_setjmp(buf);
  if (r == 0) {
    assert(asm_add(1234, 5678) == 6912);
    assert(asm_popcnt(0x0123456789abcdefULL) == 32);
    // TODO: add more tests here.
    // asm_memcpy(res2,res3,5*sizeof(int));
    // for (int i=0;i<5;i++){
    //   printf("%d ",res2[i]);
    // }
    // printf("\n");
    // for (int i=0;i<5;i++){
    //   printf("%d ",res3[i]);
    // }
    // printf("\n");

    asm_longjmp(buf, 123);
  } else {
    assert(r == 123);
    printf("PASSED.\n");
  }
}
