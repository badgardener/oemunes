#ifndef MAPPER_BASE_CPP
#define MAPPER_BASE_CPP

#include "../cartridge.h"

#include <cstdint>

class Mapper {
public:
  Cartridge *cart;

  inline Mapper(Cartridge *cart) : cart(cart) {}
  virtual inline ~Mapper() { free(cart); }

  virtual std::uint8_t cpu_read(std::uint16_t addr) = 0;
  virtual void cpu_write(std::uint16_t addr, std::uint8_t val) = 0;

  virtual std::uint8_t ppu_read(std::uint16_t addr) = 0;
  virtual void ppu_write(std::uint16_t addr, std::uint8_t val) = 0;

  virtual bool isIrq() = 0;
  virtual void latch(std::uint8_t val) = 0;
  virtual void notifyScanline() = 0;
};

#endif // MAPPER_BASE_CPP
