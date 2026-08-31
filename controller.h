#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

enum {
  CONTROLLER_BUTTON_A = 0b10000000,
  CONTROLLER_BUTTON_B = 0b01000000,
  CONTROLLER_BUTTON_SELECT = 0b00100000,
  CONTROLLER_BUTTON_START = 0b00010000,
  CONTROLLER_BUTTON_UP = 0b00001000,
  CONTROLLER_BUTTON_DOWN = 0b00000100,
  CONTROLLER_BUTTON_LEFT = 0b00000010,
  CONTROLLER_BUTTON_RIGHT = 0b00000001,
};

typedef struct Controller {
  uint8_t buttons;
  uint8_t shift_register;
  bool strobe;
} Controller;

Controller *controller_build_controller(void);

void set_state(Controller *ctx, uint8_t btn, bool pressed);
bool get_state(Controller *ctx, uint8_t btn);

void controller_write_bus(Controller *ctx, uint8_t data);
uint8_t controller_read_bus(Controller *ctx);

static inline void controller_destroy_controller(Controller *ctx) {
  if (ctx)
    free(ctx);
}

#endif
