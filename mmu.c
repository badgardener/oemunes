#include "mmu.h"
#include "cpu6502.h"
#include "mapper.h"

Bus *bus_init_bus(Cartridge *cart, Controller *player_1, Controller *player_2) {
  Bus *bus = malloc(sizeof(Bus));

  if (!bus)
    return NULL;

  if (player_1)
    bus->player_1 = player_1;
  if (player_2)
    bus->player_2 = player_2;

  bus->mapper = mapper_build_mapper(cart->mapper, cart->submapper, cart);
  bus->cpu = cpu_init_cpu(bus);
  return bus;
}

void bus_write_cpu(Bus *bus, uint16_t addr, uint8_t val) {
  if (!bus)
    return;

  if (addr <= 0x1FFF)
    bus->cpu_ram[addr & 0x7FF] = val;
  else if (addr <= 0x3FFF) {
  } // PPU SPACE
  else if (addr <= 0x4013) {
  } // APU REG
  else if (addr == 0x4015) {
  } // APU SPECIAL REG
  else if (addr == 0x4016) {
    controller_write_bus(bus->player_1, val);
    controller_write_bus(bus->player_2, val);
  } else if (addr == 0x4017) {
  } // APU SPECIAL
  else if (addr >= 0x4020)
    mapper_cpu_write(bus->mapper, addr, val);
}

void bus_write_oam(Bus *bus, uint8_t index, uint8_t val) {
  if (!bus)
    return;
}

uint8_t bus_read_cpu(Bus *bus, uint16_t addr) {
  if (!bus)
    return 0;

  int16_t val = -1;

  if (addr <= 0x1FFF)
    val = bus->cpu_ram[addr & 0x7FF];
  else if (addr <= 0x3FFF) {
  } // PPU SPACE
  else if (addr == 0x4015) {
  } // APU SPECIAL REG
  else if (addr == 0x4016)
    val = controller_read_bus(bus->player_1);
  else if (addr == 0x4017)
    val = controller_read_bus(bus->player_2);
  else if (addr >= 0x4020)
    val = mapper_cpu_read(bus->mapper, addr);

  bus->floatingBusValue = val >= 0 ? val : bus->floatingBusValue;
  return bus->floatingBusValue;
}

void bus_increment_master_clock(Bus *bus) {
  if (!bus)
    return;
}
