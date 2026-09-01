#ifndef MMU_H
#define MMU_H

#include "cartridge.h"
#include "controller.h"
#include "mapper.h"

typedef struct CPU CPU;
typedef struct PPU PPU;

typedef struct APU {
} APU; // Later, needed to be look like
// typedef struct APU APU;

typedef struct Bus {
  uint8_t cpu_ram[0x800];
  uint8_t ppu_ram[0x1000];
  uint8_t ppu_pal[0x20];
  uint8_t floatingBusValue;

  uint8_t *mirror;
  CPU *cpu;
  PPU *ppu;
  APU *apu;

  Mapper *mapper;

  float ppu_accum;
  float ppu_cpu_ratio;

  Controller *player_1;
  Controller *player_2;
} Bus;

Bus *bus_init_bus(Cartridge *cart);

void bus_write_cpu(Bus *bus, uint16_t addr, uint8_t val);
void bus_write_oam(Bus *bus, uint8_t index, uint8_t val);
uint8_t bus_read_cpu(Bus *bus, uint16_t addr);

void bus_write_ppu(Bus *bus, uint16_t addr, uint8_t val);
uint8_t bus_read_ppu(Bus *bus, uint16_t addr);

void bus_increment_master_clock(Bus *bus);

static inline void bus_deinit_bus(Bus *bus) {
  if (bus) {
    free(bus);

    if (bus->cpu)
      free(bus->cpu);
    if (bus->ppu)
      free(bus->ppu);
    // if (bus->apu)
    //   free(bus->apu);

    if (bus->mapper)
      mapper_destroy_mapper(bus->mapper);

    if (bus->player_1)
      free(bus->player_1);
    if (bus->player_2)
      free(bus->player_2);
  }
}

#endif // MMU_H
