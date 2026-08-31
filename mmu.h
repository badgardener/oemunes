#ifndef MMU_H
#define MMU_H

#include <stdint.h>

typedef struct CPU CPU;

typedef struct Bus {
  uint8_t cpu_ram[0x800];

  CPU *cpu;
} Bus;

Bus *init_bus();

void bus_write_cpu(Bus *bus, uint16_t address, uint8_t val);
void bus_write_oam(Bus *bus, uint8_t index, uint8_t val);
uint8_t bus_read_cpu(Bus *bus, uint16_t address);

void bus_increment_master_clock(Bus *bus);

#endif // MMU_H
