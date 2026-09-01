#include "mmu.h"
#include "controller.h"
#include "cpu6502.h"
#include "ppu.h"

uint16_t mirror_ppu_address(uint16_t addr, uint8_t mirror);

uint16_t mirror_ppu_address(uint16_t addr, uint8_t mirror) {
  switch (mirror) {
  case MIRRORING_VERTICAL:
    addr &= 0x7FF;
    break;
  case MIRRORING_HORIZONTAL:
    addr = mirror_ppu_address(addr, MIRRORING_SINGLE0) +
           (addr >= 0x800 ? 0x400 : 0);
    break;
  case MIRRORING_SINGLE0:
    addr &= 0x3FF;
    break;
  case MIRRORING_SINGLE1:
    addr = mirror_ppu_address(addr, MIRRORING_SINGLE0) + 0x400;
    break;
  }

  return addr;
}

Bus *bus_init_bus(Cartridge *cart) {
  Bus *bus = malloc(sizeof(Bus));

  if (!bus)
    return NULL;

  bus->player_1 = controller_build_controller();
  bus->player_2 = controller_build_controller();

  bus->mapper = mapper_build_mapper(cart->mapper, cart->submapper, cart);

  bus->ppu_accum = 0.0;
  bus->ppu_cpu_ratio = cart->region == REGION_PAL ? 3.2 : 3.0;

  bus->cpu = cpu_init_cpu(bus);
  bus->ppu = ppu_init_ppu(bus);

  bus->mirror = &cart->mirroring;
  return bus;
}

void bus_write_cpu(Bus *bus, uint16_t addr, uint8_t val) {
  if (!bus)
    return;

  if (addr <= 0x1FFF)
    bus->cpu_ram[addr & 0x7FF] = val;
  else if (addr <= 0x3FFF) {
    addr = (addr - 0x2000) & 7;
    ppu_write_cpu(bus->ppu, addr, val);
  } else if (addr <= 0x4013) {
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

  bus->ppu->real_oam[index] = val;
}

uint8_t bus_read_cpu(Bus *bus, uint16_t addr) {
  if (!bus)
    return 0;

  int16_t val = -1;

  if (addr <= 0x1FFF)
    val = bus->cpu_ram[addr & 0x7FF];
  else if (addr <= 0x3FFF) {
    addr = (addr - 0x2000) & 7;
    val = ppu_read_cpu(bus->ppu, addr);
  } else if (addr == 0x4015) {
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

void bus_write_ppu(Bus *bus, uint16_t addr, uint8_t val) {
  addr &= 0x3FFF;

  if (addr <= 0x1FFF)
    mapper_ppu_write(bus->mapper, addr, val);
  else if (addr <= 0x3EFF) {
    addr = (addr - 0x2000) & 0xFFF;
    addr = mirror_ppu_address(addr, *bus->mirror);
    bus->ppu_ram[addr] = val;
  } else {
    addr = (addr - 0x3F00) & 0x1F;
    if ((addr & 0x13) == 0x10)
      addr -= 0x10;
    bus->ppu_pal[addr] = val & 0x3F;
  }
}

uint8_t bus_read_ppu(Bus *bus, uint16_t addr) {
  addr &= 0x3FFF;
  int16_t val = -1;

  if (addr <= 0x1FFF)
    val = mapper_ppu_read(bus->mapper, addr);
  else if (addr <= 0x3EFF) {
    addr = (addr - 0x2000) & 0xFFF;
    addr = mirror_ppu_address(addr, *bus->mirror);
    val = bus->ppu_ram[addr];
  } else {
    addr = (addr - 0x3F00) & 0x1F;
    if ((addr & 0x13) == 0x10)
      addr -= 0x10;
    return bus->ppu_pal[addr];
  }

  bus->floatingBusValue = val >= 0 ? val : bus->floatingBusValue;
  return bus->floatingBusValue;
}

void bus_increment_master_clock(Bus *bus) {
  if (!bus)
    return;

  // apu_tick_apu(bus->apu);

  while (bus->ppu_accum < bus->ppu_cpu_ratio) {
    ppu_clock_ppu(bus->ppu);
    bus->ppu_accum += 1.0;
  }

  bus->ppu_accum -= bus->ppu_cpu_ratio;
  bus->cpu->irqLineMapper = mapper_isIrq(bus->mapper);
}
