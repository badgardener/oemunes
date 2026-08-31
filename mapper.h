#ifndef MAPPER_H
#define MAPPER_H

#include <stdbool.h>
#include <stdint.h>

typedef struct Mapper Mapper;
typedef struct Cartridge Cartridge;

#ifdef __cplusplus
extern "C" {
#endif

Mapper *mapper_build_mapper(uint16_t mapper_id, uint8_t submapper,
                            Cartridge *cart);

int16_t mapper_cpu_read(Mapper *ctx, uint16_t addr);
void mapper_cpu_write(Mapper *ctx, uint16_t addr, uint8_t val);

int16_t mapper_ppu_read(Mapper *ctx, uint16_t addr);
void mapper_ppu_write(Mapper *ctx, uint16_t addr, uint8_t val);

bool mapper_isIrq(Mapper *ctx);
void mapper_latch(Mapper *ctx, uint8_t val);
void mapper_notifyScanline(Mapper *ctx);

void mapper_destroy_mapper(Mapper *ctx);

#ifdef __cplusplus
}
#endif

#endif // MAPPER_H
