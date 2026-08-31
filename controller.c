#include "controller.h"

Controller *controller_build_controller(void) {
  Controller *ctx = (Controller *)malloc(sizeof(Controller));
  if (ctx) {
    ctx->buttons = 0;
    ctx->shift_register = 0;
    ctx->strobe = false;
  }

  return ctx;
}

void set_state(Controller *ctx, uint8_t btn, bool pressed) {
  if (!ctx)
    return;

  if (pressed)
    ctx->buttons |= btn;
  else
    ctx->buttons &= ~btn;
}

bool get_state(Controller *ctx, uint8_t btn) {
  if (!ctx)
    return false;

  return (ctx->buttons & btn) != 0;
}

void controller_write_bus(Controller *ctx, uint8_t data) {
  if (!ctx)
    return;

  ctx->strobe = (data & 0x01);
  if (ctx->strobe)
    ctx->shift_register = ctx->buttons;
}

uint8_t controller_read_bus(Controller *ctx) {
  if (!ctx)
    return 0x40;

  uint8_t value;
  if (ctx->strobe)
    value = (ctx->buttons & CONTROLLER_BUTTON_A) ? 1 : 0;
  else {
    value = (ctx->shift_register & 0x80) ? 1 : 0;
    ctx->shift_register = (ctx->shift_register << 1) | 0x01;
  }

  return value | 0x40;
}
