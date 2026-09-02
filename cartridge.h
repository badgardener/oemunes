#ifndef CARTRIDGE_H
#define CARTRIDGE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

enum {
  MIRRORING_VERTICAL,
  MIRRORING_HORIZONTAL,
  MIRRORING_FOURSCREEN,
  MIRRORING_SINGLE0,
  MIRRORING_SINGLE1,
};

enum {
  REGION_NTSC, // Equivalent to DUAL also.
  REGION_PAL,
  REGION_DENDY,
};

typedef struct Cartridge {
  uint8_t *prgRom;
  uint8_t *prgRam;
  uint8_t *prgNvRam;

  uint8_t *chrRom;
  uint8_t *chrRam;
  uint8_t *chrNvRam;

  size_t prgRomSize;
  size_t prgRamSize;
  size_t prgNvRamSize;

  size_t chrRomSize;
  size_t chrRamSize;
  size_t chrNvRamSize;

  uint16_t mapper;
  uint8_t submapper;

  uint8_t mirroring;
  uint8_t region;

  bool battery;
  bool trainer;
  bool is_nes2;

  uint8_t *trainerData;
} Cartridge;

typedef enum CartridgeError {
  CARTRIDGE_OK = 0,
  CARTRIDGE_SMALL_ROM,
  CARTRIDGE_INVALID_ROM,
  CARTRIDGE_MEMORY_ERROR,
  CARTRIDGE_INVALID_DATA
} CartridgeError;

Cartridge *cartridge_build_cartridge(uint8_t *rom, size_t romSize,
                                     CartridgeError *error);

static inline void cartridge_drop_cartridge(Cartridge *cart) {
  if (cart) {
    if (cart->prgRom)
      free(cart->prgRom);
    if (cart->prgRam)
      free(cart->prgRam);
    if (cart->prgNvRam)
      free(cart->prgNvRam);
    if (cart->chrRom)
      free(cart->chrRom);
    if (cart->chrRam)
      free(cart->chrRam);
    if (cart->chrNvRam)
      free(cart->chrNvRam);

    if (cart->trainerData)
      free(cart->trainerData);

    free(cart);
  }
}

#endif // CARTRIDGE_H
