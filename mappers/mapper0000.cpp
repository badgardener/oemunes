#ifndef MAPPER_0000_CPP
#define MAPPER_0000_CPP

#include "mapper_base.hpp"

class Mapper0000 : public Mapper {
public:
  Mapper0000(Cartridge *cart) : Mapper(cart) {}

  std::uint8_t cpu_read(std::uint16_t addr) override {
    if (addr >= 0x6000 && addr <= 0x7FFF) {
      if (cart->prgRamSize)
        return cart->prgRam[addr - 0x6000];

      if (cart->prgNvRamSize)
        return cart->prgNvRam[addr - 0x6000];

      return 0;
    }

    if (addr >= 0x8000) {
      std::size_t offset = addr - 0x8000;

      if (cart->prgRomSize == 0)
        return 0;

      if (cart->prgRomSize == 0x4000)
        offset &= 0x3FFF;

      return cart->prgRom[offset];
    }

    return 0;
  }

  void cpu_write(std::uint16_t addr, std::uint8_t val) override {
    if (addr >= 0x6000 && addr <= 0x7FFF) {
      if (cart->prgRamSize)
        cart->prgRam[addr - 0x6000] = val;
      else if (cart->prgNvRamSize)
        cart->prgNvRam[addr - 0x6000] = val;
    }
  }

  std::uint8_t ppu_read(std::uint16_t addr) override {
    if (addr > 0x1FFF)
      return 0;

    if (cart->chrRomSize)
      return cart->chrRom[addr];

    if (cart->chrRamSize)
      return cart->chrRam[addr];

    if (cart->chrNvRamSize)
      return cart->chrNvRam[addr];

    return 0;
  }

  void ppu_write(std::uint16_t addr, std::uint8_t val) override {
    if (addr > 0x1FFF)
      return;

    if (cart->chrRamSize)
      cart->chrRam[addr] = val;
    else if (cart->chrNvRamSize)
      cart->chrNvRam[addr] = val;
  }

  bool isIrq() override { return false; }

  void latch(std::uint8_t val) override {}

  void notifyScanline() override {}
};

#endif
