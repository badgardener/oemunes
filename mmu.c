#include "mmu.h"
#include "cpu6502.h"
#include "mapper.h"

Bus *bus_init_bus(Cartridge *cart) {
  Bus *bus = malloc(sizeof(Bus));
  bus->mapper = mapper_build_mapper(cart->mapper, cart->submapper, cart);
  bus->cpu = cpu_init_cpu(bus);
  return bus;
}

void bus_write_cpu(Bus *bus, uint16_t addr, uint8_t val) {
  if (addr <= 0x1FFF)
    bus->cpu_ram[addr & 0x7FF] = val;
  else if (addr <= 0x3FFF) {
  } // PPU SPACE
  else if (addr <= 0x4013) {
  } // APU REG
  else if (addr == 0x4015) {
  } // APU SPECIAL REG
  else if (addr == 0x4016) {
  } // IO
  else if (addr == 0x4017) {
  } // APU SPECIAL
  else if (addr >= 0x4020)
    mapper_cpu_write(bus->mapper, addr, val);
}

void bus_write_oam(Bus *bus, uint8_t index, uint8_t val) {}

uint8_t bus_read_cpu(Bus *bus, uint16_t addr) {
  int16_t val = -1;

  if (addr <= 0x1FFF)
    val = bus->cpu_ram[addr & 0x7FF];
  else if (addr <= 0x3FFF) {
  } // PPU SPACE
  else if (addr == 0x4015) {
  } // APU SPECIAL REG
  else if (addr == 0x4016) {
  } // IO1
  else if (addr == 0x4017) {
  } // IO2
  else if (addr >= 0x4020)
    val = mapper_cpu_read(bus->mapper, addr);

  bus->floatingBusValue = val >= 0 ? val : bus->floatingBusValue;
  return bus->floatingBusValue;
}

void bus_increment_master_clock(Bus *bus) {}
