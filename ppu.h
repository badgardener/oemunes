#ifndef PPU_H
#define PPU_H

#include "mmu.h"

typedef struct PPU {
  uint8_t real_oam[256];
  Bus *bus;
} PPU;

PPU *ppu_init_ppu(Bus *bus);

void ppu_write_cpu(PPU *ctx, uint8_t reg, uint8_t val);
uint8_t ppu_read_cpu(PPU *ctx, uint8_t reg);

void ppu_clock_ppu(PPU *ctx);

static inline void ppu_deinit_ppu(PPU *ppu) {
  if (ppu)
    free(ppu);
}

#endif // PPU_H
