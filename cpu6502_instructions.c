#include "cpu6502_instructions.h"

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

uint16_t PC(CPU *ctx) {
  return (uint16_t)ctx->regPCL | ((uint16_t)ctx->regPCH << 8);
}

void incrementPC(CPU *ctx) {
  uint16_t pc = PC(ctx) + 1;
  ctx->regPCL = (uint8_t)pc;
  ctx->regPCH = (uint8_t)(pc >> 8);
}

uint16_t get_address_imm(CPU *ctx) {
  uint16_t addr = PC(ctx);
  incrementPC(ctx);
  return addr;
}
uint16_t get_address_zp0(CPU *ctx) {
  uint8_t addr = read_cpu(ctx, PC(ctx));
  incrementPC(ctx);
  return addr;
}

uint16_t get_address_zpx(CPU *ctx) {
  uint8_t addr = read_cpu(ctx, PC(ctx));
  incrementPC(ctx);
  read_cpu(ctx, addr);
  return (uint8_t)(addr + ctx->regX);
}

uint16_t get_address_zpy(CPU *ctx) {
  uint8_t addr = read_cpu(ctx, PC(ctx));
  incrementPC(ctx);
  read_cpu(ctx, addr);
  return (uint8_t)(addr + ctx->regY);
}

uint16_t get_address_abs(CPU *ctx) {
  uint8_t lo = read_cpu(ctx, PC(ctx));
  incrementPC(ctx);
  uint8_t hi = read_cpu(ctx, PC(ctx));
  incrementPC(ctx);
  return ((uint16_t)hi << 8) | lo;
}

uint16_t get_address_abx(CPU *ctx, bool force_dummy) {
  uint8_t lo = read_cpu(ctx, PC(ctx));
  incrementPC(ctx);
  uint8_t hi = read_cpu(ctx, PC(ctx));
  incrementPC(ctx);
  uint16_t base = ((uint16_t)hi << 8) | lo;
  uint16_t addr = base + ctx->regX;
  if (force_dummy || (base & 0xFF00) != (addr & 0xFF00))
    read_cpu(ctx, (base & 0xFF00) | (addr & 0x00FF));
  return addr;
}

uint16_t get_address_aby(CPU *ctx, bool force_dummy) {
  uint8_t lo = read_cpu(ctx, PC(ctx));
  incrementPC(ctx);
  uint8_t hi = read_cpu(ctx, PC(ctx));
  incrementPC(ctx);
  uint16_t base = ((uint16_t)hi << 8) | lo;
  uint16_t addr = base + ctx->regY;
  if (force_dummy || (base & 0xFF00) != (addr & 0xFF00))
    read_cpu(ctx, (base & 0xFF00) | (addr & 0x00FF));
  return addr;
}

uint16_t get_address_rel(CPU *ctx) {
  uint8_t offset = read_cpu(ctx, PC(ctx));
  incrementPC(ctx);
  return PC(ctx) + (int8_t)offset;
}

uint16_t get_address_ind(CPU *ctx) {
  uint8_t lo = read_cpu(ctx, PC(ctx));
  incrementPC(ctx);
  uint8_t hi = read_cpu(ctx, PC(ctx));
  incrementPC(ctx);
  uint16_t ptr = ((uint16_t)hi << 8) | lo;
  uint8_t targetLo = read_cpu(ctx, ptr);
  uint16_t nextPtr = (ptr & 0xFF00) | ((ptr + 1) & 0x00FF);
  uint8_t targetHi = read_cpu(ctx, nextPtr);
  return ((uint16_t)targetHi << 8) | targetLo;
}

uint16_t get_address_izx(CPU *ctx) {
  uint8_t zp = read_cpu(ctx, PC(ctx));
  incrementPC(ctx);
  uint8_t ptr = (uint8_t)(zp + ctx->regX);
  read_cpu(ctx, ptr);
  uint8_t lo = read_cpu(ctx, ptr);
  uint8_t hi = read_cpu(ctx, (uint8_t)(ptr + 1));
  return ((uint16_t)hi << 8) | lo;
}

uint16_t get_address_izy(CPU *ctx, bool force_dummy) {
  uint8_t zp = read_cpu(ctx, PC(ctx));
  incrementPC(ctx);
  uint8_t lo = read_cpu(ctx, zp);
  uint8_t hi = read_cpu(ctx, (uint8_t)(zp + 1));
  uint16_t base = ((uint16_t)hi << 8) | lo;
  uint16_t addr = base + ctx->regY;
  if (force_dummy || (base & 0xFF00) != (addr & 0xFF00))
    read_cpu(ctx, (base & 0xFF00) | (addr & 0x00FF));
  return addr;
}

void execute_slo(CPU *ctx, uint16_t addr) {
  uint8_t val = read_cpu(ctx, addr);
  write_cpu(ctx, addr, val);
  ctx->regP &= ~CPU_FLAG_C;
  if (val & 0x80)
    ctx->regP |= CPU_FLAG_C;
  val <<= 1;
  write_cpu(ctx, addr, val);
  ctx->regA |= val;
  ctx->regP &= ~(CPU_FLAG_N | CPU_FLAG_Z);
  if (ctx->regA == 0)
    ctx->regP |= CPU_FLAG_Z;
  if (ctx->regA & 0x80)
    ctx->regP |= CPU_FLAG_N;
}

void execute_ora(CPU *ctx, uint16_t addr) {
  uint8_t val = read_cpu(ctx, addr);
  ctx->regA |= val;
  ctx->regP &= ~(CPU_FLAG_N | CPU_FLAG_Z);
  if (ctx->regA == 0)
    ctx->regP |= CPU_FLAG_Z;
  if (ctx->regA & 0x80)
    ctx->regP |= CPU_FLAG_N;
}

void execute_asl(CPU *ctx, uint16_t addr) {
  uint8_t val = read_cpu(ctx, addr);
  write_cpu(ctx, addr, val);
  ctx->regP &= ~CPU_FLAG_C;
  if (val & 0x80)
    ctx->regP |= CPU_FLAG_C;
  val <<= 1;
  write_cpu(ctx, addr, val);
  ctx->regP &= ~(CPU_FLAG_Z | CPU_FLAG_N);
  if (val == 0)
    ctx->regP |= CPU_FLAG_Z;
  if (val & 0x80)
    ctx->regP |= CPU_FLAG_N;
}

void execute_asl_acc(CPU *ctx) {
  uint8_t val = ctx->regA;
  ctx->regP &= ~CPU_FLAG_C;
  if (val & 0x80)
    ctx->regP |= CPU_FLAG_C;
  val <<= 1;
  ctx->regA = val;
  ctx->regP &= ~(CPU_FLAG_Z | CPU_FLAG_N);
  if (val == 0)
    ctx->regP |= CPU_FLAG_Z;
  if (val & 0x80)
    ctx->regP |= CPU_FLAG_N;
}
