#include "cartridge.h"

#include <string.h>

size_t nes2_ram_size(uint8_t value);
bool valid_range(size_t offset, size_t size, size_t romSize);
size_t nes2_rom_size(uint8_t lsb, uint8_t msb);

size_t nes2_ram_size(uint8_t value) {
  if (value == 0)
    return 0;

  return (size_t)64 << value;
}

bool valid_range(size_t offset, size_t size, size_t romSize) {
  return offset <= romSize && size <= romSize - offset;
}

size_t nes2_rom_size(uint8_t lsb, uint8_t msb) {
  if (msb != 0x0F)
    return ((size_t)msb << 8 | lsb) * 16 * 1024;

  uint8_t exponent = lsb >> 2;
  uint8_t multiplier = (lsb & 0x03) * 2 + 1;

  if (exponent >= sizeof(size_t) * 8)
    return 0;

  return ((size_t)multiplier << exponent);
}

Cartridge *cartridge_build_cartridge(uint8_t *rom, size_t romSize,
                                     CartridgeError *error) {
  if (!rom || romSize < 16) {
    if (error)
      *error = CARTRIDGE_SMALL_ROM;
    return NULL;
  }

  if (rom[0] != 'N' || rom[1] != 'E' || rom[2] != 'S' || rom[3] != 0x1A) {
    if (error)
      *error = CARTRIDGE_INVALID_ROM;
    return NULL;
  }

  Cartridge *cart = calloc(1, sizeof(Cartridge));

  if (!cart) {
    if (error)
      *error = CARTRIDGE_MEMORY_ERROR;
    return NULL;
  }

  cart->is_nes2 = (rom[7] & 0x0C) == 0x08;

  if (rom[6] & 0x08)
    cart->mirroring = MIRRORING_FOURSCREEN;
  else if (rom[6] & 0x01)
    cart->mirroring = MIRRORING_VERTICAL;
  else
    cart->mirroring = MIRRORING_HORIZONTAL;

  cart->battery = (rom[6] & 0x02) != 0;
  cart->trainer = (rom[6] & 0x04) != 0;

  cart->mapper = (uint16_t)((rom[6] >> 4) | (rom[7] & 0xF0));

  if (cart->is_nes2) {
    cart->mapper |= (uint16_t)((rom[8] & 0x0F) << 8);
    cart->submapper = rom[8] >> 4;
  }

  if (cart->is_nes2) {
    switch (rom[9] & 0x03) {
    case 0:
      cart->region = REGION_NTSC;
      break;
    case 1:
      cart->region = REGION_PAL;
      break;
    case 2:
      cart->region = REGION_NTSC;
      break;
    case 3:
      cart->region = REGION_DENDY;
      break;
    }
  } else {
    cart->region = (rom[9] & 0x01) ? REGION_PAL : REGION_NTSC;
  }

  if (cart->is_nes2) {
    cart->prgRomSize = nes2_rom_size(rom[4], rom[9] & 0x0F);
    cart->chrRomSize = nes2_rom_size(rom[5], rom[9] >> 4);

    cart->prgRamSize = nes2_ram_size(rom[10] & 0x0F);
    cart->prgNvRamSize = nes2_ram_size(rom[10] >> 4);

    cart->chrRamSize = nes2_ram_size(rom[11] & 0x0F);
    cart->chrNvRamSize = nes2_ram_size(rom[11] >> 4);
  } else {
    cart->prgRomSize = (size_t)rom[4] * 16 * 1024;
    cart->chrRomSize = (size_t)rom[5] * 8 * 1024;
    cart->prgRamSize = rom[8] ? (size_t)rom[8] * 8 * 1024 : 8 * 1024;
    cart->prgNvRamSize = 0;
    cart->chrRamSize = cart->chrRomSize ? 0 : 8 * 1024;
    cart->chrNvRamSize = 0;
  }

  size_t offset = 16;

  if (cart->trainer) {
    if (!valid_range(offset, 512, romSize))
      goto fail;

    cart->trainerData = malloc(512);

    if (!cart->trainerData)
      goto fail;

    memcpy(cart->trainerData, rom + offset, 512);
    offset += 512;
  }

  if (!valid_range(offset, cart->prgRomSize, romSize))
    goto fail;

  if (cart->prgRomSize) {
    cart->prgRom = malloc(cart->prgRomSize);

    if (!cart->prgRom)
      goto fail;

    memcpy(cart->prgRom, rom + offset, cart->prgRomSize);
    offset += cart->prgRomSize;
  }

  if (!valid_range(offset, cart->chrRomSize, romSize))
    goto fail;

  if (cart->chrRomSize) {
    cart->chrRom = malloc(cart->chrRomSize);

    if (!cart->chrRom)
      goto fail;

    memcpy(cart->chrRom, rom + offset, cart->chrRomSize);
    offset += cart->chrRomSize;
  }

  if (cart->prgRamSize) {
    cart->prgRam = calloc(1, cart->prgRamSize);

    if (!cart->prgRam)
      goto fail;
  }

  if (cart->prgNvRamSize) {
    cart->prgNvRam = calloc(1, cart->prgNvRamSize);

    if (!cart->prgNvRam)
      goto fail;
  }

  if (cart->chrRamSize) {
    cart->chrRam = calloc(1, cart->chrRamSize);

    if (!cart->chrRam)
      goto fail;
  }

  if (cart->chrNvRamSize) {
    cart->chrNvRam = calloc(1, cart->chrNvRamSize);

    if (!cart->chrNvRam)
      goto fail;
  }

  if (error)
    *error = CARTRIDGE_OK;
  return cart;

fail:
  free(cart->prgRom);
  free(cart->prgRam);
  free(cart->prgNvRam);
  free(cart->chrRom);
  free(cart->chrRam);
  free(cart->chrNvRam);
  free(cart->trainerData);
  free(cart);

  if (error)
    *error = CARTRIDGE_INVALID_DATA;
  return NULL;
}
