#include "cpu6502.h"

#include <stdlib.h>

uint8_t read_cpu(CPU *ctx, uint16_t addr);
void write_cpu(CPU *ctx, uint16_t addr, uint8_t val);
uint16_t PC(CPU *ctx);
void push_cpu(CPU *ctx, uint8_t val);
uint8_t pop_cpu(CPU *ctx);
void incrementPC(CPU *ctx);
void execute_irq(CPU *ctx);
void execute_nmi(CPU *ctx);

CPU *cpu_init_cpu(Bus *bus) {
  CPU *cpu = malloc(sizeof(CPU));

  if (!cpu)
    return NULL;

  cpu->regA = 0;
  cpu->regX = 0;
  cpu->regY = 0;
  cpu->regS = 0xFD;
  cpu->regP = 0x24;
  cpu->bus = bus;
  cpu->regPCL = bus_read_cpu(bus, 0xFFFC);
  cpu->regPCH = bus_read_cpu(bus, 0xFFFD);
  cpu->steps = 0;
  cpu->nmiPending = false;
  cpu->irqLine = false;

  return cpu;
}

uint16_t PC(CPU *ctx) {
  return (uint16_t)ctx->regPCL | ((uint16_t)ctx->regPCH << 8);
}

uint8_t read_cpu(CPU *ctx, uint16_t addr) {
  uint8_t val = bus_read_cpu(ctx->bus, addr);
  bus_increment_master_clock(ctx->bus);
  ctx->steps++;
  return val;
}

void write_cpu(CPU *ctx, uint16_t addr, uint8_t val) {
  bus_write_cpu(ctx->bus, addr, val);
  bus_increment_master_clock(ctx->bus);
  ctx->steps++;

  if (addr != 0x4014)
    return;

  uint16_t page = (uint16_t)val << 8;

  if (ctx->steps & 1) {
    bus_increment_master_clock(ctx->bus);
    ctx->steps++;
  }

  for (uint16_t i = 0; i < 256; i++) {
    uint8_t oam_val = read_cpu(ctx, page);
    bus_write_oam(ctx->bus, i, oam_val);
    bus_increment_master_clock(ctx->bus);
    ctx->steps++;
    page++;
  }
}

void push_cpu(CPU *ctx, uint8_t val) {
  uint16_t addr = 0x0100 | ctx->regS;
  write_cpu(ctx, addr, val);
  ctx->regS--;
}

uint8_t pop_cpu(CPU *ctx) {
  ctx->regS++;
  return read_cpu(ctx, 0x0100 | ctx->regS);
}

void incrementPC(CPU *ctx) {
  uint16_t pc = PC(ctx) + 1;
  ctx->regPCL = (uint8_t)pc;
  ctx->regPCH = (uint8_t)(pc >> 8);
}

void cpu_execute_cpu(CPU *ctx) {
  ctx->opcode = read_cpu(ctx, PC(ctx));
  incrementPC(ctx);

  switch (ctx->opcode) {
  case BRK_IMP:
    read_cpu(ctx, PC(ctx));
    incrementPC(ctx);
    push_cpu(ctx, PC(ctx) >> 8);
    push_cpu(ctx, PC(ctx) & 0xFF);
    push_cpu(ctx, ctx->regP | FLAG_B | FLAG_U);
    ctx->regPCL = read_cpu(ctx, 0xFFFE);
    ctx->regPCH = read_cpu(ctx, 0xFFFF);
    break;

    /*
     TODO:
     All 255 more OPCODE needed to be implemented.
    */
  }

  if (ctx->nmiPending)
    execute_nmi(ctx);
  else if (ctx->irqLine && !(ctx->regP & FLAG_I))
    execute_irq(ctx);
}

void execute_irq(CPU *ctx) {
  read_cpu(ctx, PC(ctx));
  push_cpu(ctx, PC(ctx) >> 8);
  push_cpu(ctx, PC(ctx) & 0xFF);
  push_cpu(ctx, (ctx->regP & ~FLAG_B) | FLAG_U);
  ctx->regP |= FLAG_I;
  ctx->regPCL = read_cpu(ctx, 0xFFFE);
  ctx->regPCH = read_cpu(ctx, 0xFFFF);
}

void execute_nmi(CPU *ctx) {
  read_cpu(ctx, PC(ctx));
  push_cpu(ctx, PC(ctx) >> 8);
  push_cpu(ctx, PC(ctx) & 0xFF);
  push_cpu(ctx, (ctx->regP & ~FLAG_B) | FLAG_U);
  ctx->regP |= FLAG_I;
  ctx->regPCL = read_cpu(ctx, 0xFFFA);
  ctx->regPCH = read_cpu(ctx, 0xFFFB);
  ctx->nmiPending = false;
}

void cpu_deinit_cpu(CPU *ctx) { free(ctx); }
