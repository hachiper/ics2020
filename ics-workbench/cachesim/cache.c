#include "common.h"
#include <inttypes.h>
#include <string.h>

typedef struct
{
  bool dirty;
  bool valid;
  uint32_t tag;
  uint8_t data[64];
} cache_block;
//cash 16kb=4*64*64
cache_block cache[64][4];

void mem_read(uintptr_t block_num, uint8_t *buf);
void mem_write(uintptr_t block_num, const uint8_t *buf);

void write(uintptr_t group_id, int i, uintptr_t block_address, uint32_t data, uint32_t wmask)
{
  uint32_t *a = (void *)(cache[group_id][i].data) + (block_address & ~0x3);
  *a = (*a & ~wmask) | (data & wmask);
}

static uint64_t cycle_cnt = 0;

void cycle_increase(int n) { cycle_cnt += n; }

// TODO: implement the following functions

uint32_t cache_read(uintptr_t addr)
{
  uintptr_t block_address = addr & 63,
            group_id = (addr >> 6) & 63,
            ad_tag = (addr >> 12);
  bool hit = false;
  uint32_t *ret;
  for (int i = 0; i < 4; i++)
  {
    if (cache[group_id][i].tag == ad_tag && cache[group_id][i].valid == true)
    {
      hit = true;
      ret = (void *)(cache[group_id][i].data) + (block_address & ~0x3);
      break;
    }
  }

  if (!hit)
  {
    bool empty2 = false;
    for (int i = 0; i < 4; i++)
    {
      if (cache[group_id][i].valid == false)
      {
        empty2 = true;
        mem_read(addr >> 6, cache[group_id][i].data);
        ret = (void *)(cache[group_id][i].data) + (block_address & ~0x3);
        cache[group_id][i].tag = ad_tag;
        cache[group_id][i].valid = true;
        cache[group_id][i].dirty = false;
        break;
      }
    }

    if (!empty2)
    {
      int sra = rand() % 4;
      if (cache[group_id][sra].dirty)
      {
        mem_write((cache[group_id][sra].tag << 6) | group_id, cache[group_id][sra].data);
      }

      mem_read(addr >> 6, cache[group_id][sra].data);
      ret = (void *)(cache[group_id][sra].data) + (block_address & ~0x3);
      cache[group_id][sra].tag = ad_tag;
      cache[group_id][sra].valid = true;
      cache[group_id][sra].dirty = false;
    }
  }

  return *ret;
}

void cache_write(uintptr_t addr, uint32_t data, uint32_t wmask)
{
  uintptr_t block_address = addr & 63,
            group_id = (addr >> 6) & 63,
            ad_tag = (addr >> 12);
  bool hit = false;
  for (int i = 0; i < 4; i++)
  {
    if (ad_tag == cache[group_id][i].tag && cache[group_id][i].valid == true)
    {
      hit = true;
      // memset(cache[group_id][i].data, data, sizeof(data));
      write(group_id, i, block_address, data, wmask);
      cache[group_id][i].dirty = true;
      break;
    }
  }
  if (!hit)
  {
    bool empty = false;
    for (int i = 0; i < 4; i++)
    {
      if (cache[group_id][i].valid == false)
      {
        empty = true;
        mem_read(addr >> 6, cache[group_id][i].data);
        write(group_id, i, block_address, data, wmask);
        cache[group_id][i].dirty = false;
        cache[group_id][i].valid = true;
        cache[group_id][i].tag = ad_tag;
        mem_write(addr >> 6, cache[group_id][i].data);
        break;
      }
    }

    if (!empty)
    {
      int num = rand() % 4;
      if (cache[group_id][num].dirty)
      {
        mem_write((cache[group_id][num].tag << 6) | (group_id), cache[group_id][num].data);
      }
      mem_read(addr >> 6, cache[group_id][num].data);
      write(group_id, num, block_address, data, wmask);
      cache[group_id][num].dirty = false;
      cache[group_id][num].valid = true;
      cache[group_id][num].tag = ad_tag;
      // memset(cache[group_id][num].data, data, sizeof(data));
      mem_write(addr >> 6, cache[group_id][num].data);
    }
  }
  // printf("ok!\n");
}

void init_cache(int total_size_width, int associativity_width)
{
  int group_num = (1 << associativity_width);
  int uintsize = (1 << total_size_width) / group_num / BLOCK_SIZE;
  for (int i = 0; i < uintsize; i++)
  {
    for (int j = 0; j < group_num; j++)
    {
      cache[i][j].valid = false;
      cache[i][j].dirty = false;
    }
  }
}

void display_statistic(void)
{
}
