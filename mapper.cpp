#include "mappers/mapper0000.cpp"

#include "mapper.h"

extern "C" inline Mapper *mapper_build_mapper(std::uint16_t mapper_id,
                                                  std::uint8_t submapper,
                                                  Cartridge *cart) {
  switch (mapper_id) {
  case 0x0000:
    return new Mapper0000(cart);
  default:
    return __null;
  }
}

extern "C" inline std::int16_t mapper_cpu_read(Mapper *ctx,
                                                   std::uint16_t addr) {
  return ctx->cpu_read(addr);
}

extern "C" inline void mapper_cpu_write(Mapper *ctx, std::uint16_t addr,
                                            std::uint8_t val) {
  ctx->cpu_write(addr, val);
}

extern "C" inline std::int16_t mapper_ppu_read(Mapper *ctx,
                                                   std::uint16_t addr) {
  return ctx->ppu_read(addr);
}

extern "C" inline void mapper_ppu_write(Mapper *ctx, std::uint16_t addr,
                                            std::uint8_t val) {
  ctx->ppu_write(addr, val);
}

extern "C" inline bool mapper_isIrq(Mapper *ctx) { return ctx->isIrq(); }

extern "C" inline void mapper_latch(Mapper *ctx, std::uint8_t val) {
  ctx->latch(val);
}

extern "C" inline void mapper_notifyScanline(Mapper *ctx) {
  ctx->notifyScanline();
}

extern "C" inline void mapper_destroy_mapper(Mapper *ctx) { delete ctx; }
