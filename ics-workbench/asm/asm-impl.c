#include "asm.h"
#include <string.h>

int64_t asm_add(int64_t a, int64_t b)
{
  asm("add %1, %0;"
      : "+a"(a)
      : "b"(b));
  return a;
}

int asm_popcnt(uint64_t x)
{
  int s;
  asm (
        "mov $0,%%eax;"
        "mov $0,%%edi;"
        "st:cmp $0x40,%%edi;"
        "jge end;"
        "mov %%rbx,%%rcx;"
        "and $1,%%rcx;"
        "add %%ecx,%%eax;"
        "shr $1,%%rbx;"
        "add $1,%%edi;"
        "jmp st;"
        "end:"
        :"=a"(s)
        :"b"(x)
        :"%edi","%rcx","cc"
      );
  return s;
}

void *asm_memcpy(void *dest, const void *src, size_t n)
{
  asm volatile (
    "mov $0,%%rdi;"
    "flag1:cmp %%rdx,%%rdi;"
    "jge end1;"
    "movzbl (%%rcx),%%rax;"
    "mov %%rax,(%%rbx);"
    "add $1,%%rbx;"
    "add $1,%%rcx;"
    "add $1,%%rdi;"
    "jmp flag1;"
    "end1:"
    :
    :"b"(dest),"c"(src),"d"(n)
    :"%rdi","%rax","memory","cc"
  );
  return dest;

}

int asm_setjmp(asm_jmp_buf env)
{
  asm volatile(
      "push %%rbp;"
      "mov %%rsp,%%rbp;"
      "mov %%rdi,%%rax;"
      "mov %%rbx,(%%rax);"
      "mov %%rcx,8(%%rax);"
      "mov %%rdx,16(%%rax);"
      "mov %%rsi,24(%%rax);"
      "mov %%rbp,32(%%rax);"
      "lea 16(%%rbp),%%rdi;"
      "mov %%rdi,40(%%rax);"
      "mov 8(%%rbp),%%rdi;"
      "mov %%rdi,48(%%rax);"
      "xor %%rax,%%rax;"
      "pop %%rbp;"
      :
      :"d"(env)
      :
  );
  return 0;
}

void asm_longjmp(asm_jmp_buf env, int val)
{
  asm volatile(
    "mov %%rsi,%%rax;"
    "test %%rax,%%rax;"
    "cmpl $0,%%eax;"
    "jne vp;"
    "inc %%eax;"
    "vp: mov (%%rdi),%%rbx;"
    "mov 8(%%rdi),%%rcx;"
    "mov 16(%%rdi),%%rdx;"
    "mov 24(%%rdi),%%rsi;"
    "mov 32(%%rdi),%%rbp;"
    "mov 40(%%rdi),%%rsp;"
    "jmp *48(%%rdi);"
    :
    :"d"(env),"a"(val)
    :
  );
}