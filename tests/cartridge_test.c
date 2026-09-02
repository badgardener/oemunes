#include "../cartridge.c"
#include <stdio.h>

int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("Usage : %s <nes-file>\n", argv[0]);
    return 0;
  }

  FILE *file = fopen(argv[1], "rb");
  if (file == NULL) {
    perror("Error opening file");
    return 1;
  }

  fseek(file, 0, SEEK_END);
  long file_size = ftell(file);
  if (file_size < 0) {
    perror("Error determining file size");
    fclose(file);
    return 1;
  }

  rewind(file);

  uint8_t *ROM_DATA = (uint8_t *)malloc(file_size);
  if (ROM_DATA == NULL) {
    perror("Memory allocation failed");
    fclose(file);
    return 1;
  }

  size_t bytes_read = fread(ROM_DATA, 1, file_size, file);
  if (bytes_read < (size_t)file_size) {
    if (ferror(file)) {
      perror("Error reading file");
    } else if (feof(file)) {
      printf("EOF reached unexpectedly. Read %zu bytes.\n", bytes_read);
    }
  } else {
    printf("Successfully loaded %zu bytes into ROM_DATA.\n", bytes_read);
  }

  fclose(file);

  CartridgeError error;
  Cartridge *cart =
      cartridge_build_cartridge(ROM_DATA, (size_t)file_size, &error);

  if (!cart) {
    printf("Cannot build cartridge.\n");
    char *msg;

    switch (error) {
    case CARTRIDGE_OK:
      msg = "No Error Occurred. But still error?";
      break;
    case CARTRIDGE_SMALL_ROM:
      msg = "ROM too small.";
      break;
    case CARTRIDGE_INVALID_ROM:
      msg = "Invalid ROM header.";
      break;
    case CARTRIDGE_MEMORY_ERROR:
      msg = "Cannot allocate memory for Cartridge.";
      break;
    case CARTRIDGE_INVALID_DATA:
      msg = "ROM is truncated and its data do not match the header.";
      break;
    }

    printf("%s\n", msg);

    free(ROM_DATA);
    return 1;
  }

  printf("TYPE           : %s\n", cart->is_nes2 ? "NES2.0" : "iNES1.0");
  printf("PRG SIZE       : 0x%zx\n", cart->prgRomSize);
  printf("PRG RAM SIZE   : 0x%zx\n", cart->prgRamSize);
  printf("PRG NVRAM SIZE : 0x%zx\n", cart->prgNvRamSize);
  printf("CHR SIZE       : 0x%zx\n", cart->chrRomSize);
  printf("CHR RAM SIZE   : 0x%zx\n", cart->chrRamSize);
  printf("CHR NVRAM SIZE : 0x%zx\n", cart->chrNvRamSize);
  printf("MAPPER         : %u\n", cart->mapper);
  printf("SUBMAPPER      : %u\n", cart->submapper);
  printf("MIRRORING      : %s\n", cart->mirroring == 0   ? "VERTICAL"
                                  : cart->mirroring == 1 ? "HORIZONTAL"
                                  : cart->mirroring == 2 ? "FOURSCREEN"
                                  : cart->mirroring == 3 ? "SINGLE0"
                                                         : "SINGLE1");
  printf("REGION         : %s\n", cart->region == 0   ? "NTSC"
                                  : cart->region == 1 ? "PAL"
                                                      : "DENDY");
  printf("BATTERY        : %s\n", cart->battery ? "TRUE" : "FALSE");
  printf("TRAINER        : %s\n", cart->trainer ? "TRUE" : "FALSE");

  cartridge_drop_cartridge(cart);
  free(ROM_DATA);
  return 0;
}
