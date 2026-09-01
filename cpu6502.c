#include "cpu6502.h"

uint8_t read_cpu(CPU *ctx, uint16_t addr);
void write_cpu(CPU *ctx, uint16_t addr, uint8_t val);
uint16_t PC(CPU *ctx);
void push_cpu(CPU *ctx, uint8_t val);
uint8_t pop_cpu(CPU *ctx);
void incrementPC(CPU *ctx);
void execute_irq(CPU *ctx);
void execute_nmi(CPU *ctx);

CPU *cpu_init_cpu(Bus *bus) {
  if (!bus)
    return NULL;

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
  cpu->irqLineMapper = false;
  cpu->irqLineAPU = false;
  cpu->jammed = false;

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
  if (ctx->jammed) {
    read_cpu(ctx, PC(ctx));
    return;
  }

  ctx->opcode = read_cpu(ctx, PC(ctx));
  incrementPC(ctx);

  switch (ctx->opcode) {
  case OPCODE_BRK_IMP: {
    read_cpu(ctx, PC(ctx));
    incrementPC(ctx);
    push_cpu(ctx, PC(ctx) >> 8);
    push_cpu(ctx, PC(ctx) & 0xFF);
    push_cpu(ctx, ctx->regP | CPU_FLAG_B | CPU_FLAG_U);
    ctx->regP |= CPU_FLAG_I;
    ctx->regPCL = read_cpu(ctx, 0xFFFE);
    ctx->regPCH = read_cpu(ctx, 0xFFFF);
    break;
  }

  case OPCODE_ORA_IZX: {
    uint8_t zp = read_cpu(ctx, PC(ctx));
    incrementPC(ctx);
    uint8_t ptr = (uint8_t)(zp + ctx->regX);
    read_cpu(ctx, ptr);
    uint8_t lo = read_cpu(ctx, ptr);
    uint8_t hi = read_cpu(ctx, (uint8_t)(ptr + 1));
    uint16_t addr = ((uint16_t)hi << 8) | lo;
    uint8_t val = read_cpu(ctx, addr);
    ctx->regA |= val;
    ctx->regP &= ~(CPU_FLAG_N | CPU_FLAG_Z);
    if (ctx->regA == 0)
      ctx->regP |= CPU_FLAG_Z;
    if (ctx->regA & 0x80)
      ctx->regP |= CPU_FLAG_N;
    break;
  }

  case OPCODE_NOP_IMP:
  case OPCODE_NOP_IMP_2:
  case OPCODE_NOP_IMP_3:
  case OPCODE_NOP_IMP_4:
  case OPCODE_NOP_IMP_5:
  case OPCODE_NOP_IMP_6:
  case OPCODE_NOP_IMP_7:
  case OPCODE_NOP_IMP_8:
  case OPCODE_NOP_IMP_9:
  case OPCODE_NOP_IMP_10: {
    read_cpu(ctx, PC(ctx));
    break;
  }

  case OPCODE_NOP_IMM:
  case OPCODE_NOP_IMM_2: {
    read_cpu(ctx, PC(ctx));
    incrementPC(ctx);
    break;
  }

  case OPCODE_NOP_ZP0:
  case OPCODE_NOP_ZP0_2:
  case OPCODE_NOP_ZP0_3: {
    uint8_t zp = read_cpu(ctx, PC(ctx));
    incrementPC(ctx);
    read_cpu(ctx, zp);
    break;
  }

  case OPCODE_NOP_ZPX:
  case OPCODE_NOP_ZPX_2:
  case OPCODE_NOP_ZPX_3:
  case OPCODE_NOP_ZPX_4:
  case OPCODE_NOP_ZPX_5:
  case OPCODE_NOP_ZPX_6: {
    uint8_t zp = read_cpu(ctx, PC(ctx));
    incrementPC(ctx);
    read_cpu(ctx, zp);
    uint8_t ptr = (uint8_t)(zp + ctx->regX);
    read_cpu(ctx, ptr);
    break;
  }

  case OPCODE_NOP_ABS: {
    uint8_t lo = read_cpu(ctx, PC(ctx));
    incrementPC(ctx);
    uint8_t hi = read_cpu(ctx, PC(ctx));
    incrementPC(ctx);
    read_cpu(ctx, ((uint16_t)hi << 8) | lo);
    break;
  }

  case OPCODE_NOP_ABX:
  case OPCODE_NOP_ABX_2:
  case OPCODE_NOP_ABX_3:
  case OPCODE_NOP_ABX_4:
  case OPCODE_NOP_ABX_5:
  case OPCODE_NOP_ABX_6: {
    uint8_t lo = read_cpu(ctx, PC(ctx));
    incrementPC(ctx);
    uint8_t hi = read_cpu(ctx, PC(ctx));
    incrementPC(ctx);
    uint16_t base = ((uint16_t)hi << 8) | lo;
    uint16_t addr = base + ctx->regX;
    if ((base & 0xFF00) != (addr & 0xFF00))
      read_cpu(ctx, (base & 0xFF00) | (addr & 0x00FF));
    read_cpu(ctx, addr);
    break;
  }

  case OPCODE_KIL_IMP:
  case OPCODE_KIL_IMP_2:
  case OPCODE_KIL_IMP_3:
  case OPCODE_KIL_IMP_4:
  case OPCODE_KIL_IMP_5:
  case OPCODE_KIL_IMP_6:
  case OPCODE_KIL_IMP_7:
  case OPCODE_KIL_IMP_8:
  case OPCODE_KIL_IMP_9:
  case OPCODE_KIL_IMP_10:
  case OPCODE_KIL_IMP_11:
  case OPCODE_KIL_IMP_12: {
    read_cpu(ctx, PC(ctx));
    ctx->jammed = true;
    break;
  }

    /*
     Other 214 more OPCODE needed to be implemented.
    */
  }

  if (ctx->nmiPending)
    execute_nmi(ctx);
  else if ((ctx->irqLineMapper || ctx->irqLineAPU) && !(ctx->regP & CPU_FLAG_I))
    execute_irq(ctx);
}

void execute_irq(CPU *ctx) {
  read_cpu(ctx, PC(ctx));
  push_cpu(ctx, PC(ctx) >> 8);
  push_cpu(ctx, PC(ctx) & 0xFF);
  push_cpu(ctx, (ctx->regP & ~CPU_FLAG_B) | CPU_FLAG_U);
  ctx->regP |= CPU_FLAG_I;
  ctx->regPCL = read_cpu(ctx, 0xFFFE);
  ctx->regPCH = read_cpu(ctx, 0xFFFF);
}

void execute_nmi(CPU *ctx) {
  read_cpu(ctx, PC(ctx));
  push_cpu(ctx, PC(ctx) >> 8);
  push_cpu(ctx, PC(ctx) & 0xFF);
  push_cpu(ctx, (ctx->regP & ~CPU_FLAG_B) | CPU_FLAG_U);
  ctx->regP |= CPU_FLAG_I;
  ctx->regPCL = read_cpu(ctx, 0xFFFA);
  ctx->regPCH = read_cpu(ctx, 0xFFFB);
  ctx->nmiPending = false;
}
