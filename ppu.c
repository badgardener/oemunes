#include "ppu.h"

void ppu_write(PPU *ctx, uint16_t addr, uint8_t val);
uint8_t ppu_read(PPU *ctx, uint16_t addr);

PPU *ppu_init_ppu(Bus *bus) {
  PPU *ppu = malloc(sizeof(PPU));

  if (!ppu)
    return NULL;

  ppu->bus = bus;
  return ppu;
}

void ppu_write(PPU *ctx, uint16_t addr, uint8_t val) {
  bus_write_ppu(ctx->bus, addr, val);
}

uint8_t ppu_read(PPU *ctx, uint16_t addr) {
  return bus_read_ppu(ctx->bus, addr);
}

void ppu_write_cpu(PPU *ctx, uint8_t reg, uint8_t val) {
} // NEEDS IMPLEMENTATION

uint8_t ppu_read_cpu(PPU *ctx, uint8_t reg) {
  return -1;
} // NEEDS IMPLEMENTATION

void ppu_clock_ppu(PPU *ctx) {} // NEEDS IMPLEMENTATION
