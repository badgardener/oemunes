#include "cpu6502_instructions.h"

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

void cpu_execute_cpu(CPU *ctx) {
  if (!ctx)
    return;

  if (ctx->jammed) {
    read_cpu(ctx, PC(ctx));
    return;
  }

  ctx->opcode = (OPCode)read_cpu(ctx, PC(ctx));
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
    uint16_t addr = get_address_izx(ctx);
    execute_ora(ctx, addr);
    break;
  }

  case OPCODE_ORA_ZP0: {
    uint16_t addr = get_address_zp0(ctx);
    execute_ora(ctx, addr);
    break;
  }

  case OPCODE_ORA_IMM: {
    uint16_t addr = get_address_imm(ctx);
    execute_ora(ctx, addr);
    break;
  }

  case OPCODE_ORA_ABS: {
    uint16_t addr = get_address_abs(ctx);
    execute_ora(ctx, addr);
    break;
  }

  case OPCODE_ORA_IZY: {
    uint16_t addr = get_address_izy(ctx, false);
    execute_ora(ctx, addr);
    break;
  }

  case OPCODE_ORA_ZPX: {
    uint16_t addr = get_address_zpx(ctx);
    execute_ora(ctx, addr);
    break;
  }

  case OPCODE_ORA_ABY: {
    uint16_t addr = get_address_aby(ctx, false);
    execute_ora(ctx, addr);
    break;
  }

  case OPCODE_ORA_ABX: {
    uint16_t addr = get_address_abx(ctx, false);
    execute_ora(ctx, addr);
    break;
  }

  case OPCODE_SLO_IZX: {
    uint16_t addr = get_address_izx(ctx);
    execute_slo(ctx, addr);
    break;
  }

  case OPCODE_SLO_IZY: {
    uint16_t addr = get_address_izy(ctx, true);
    execute_slo(ctx, addr);
    break;
  }

  case OPCODE_SLO_ZP0: {
    uint16_t addr = get_address_zp0(ctx);
    execute_slo(ctx, addr);
    break;
  }

  case OPCODE_SLO_ZPX: {
    uint16_t addr = get_address_zpx(ctx);
    execute_slo(ctx, addr);
    break;
  }

  case OPCODE_SLO_ABS: {
    uint16_t addr = get_address_abs(ctx);
    execute_slo(ctx, addr);
    break;
  }

  case OPCODE_SLO_ABX: {
    uint16_t addr = get_address_abx(ctx, true);
    execute_slo(ctx, addr);
    break;
  }

  case OPCODE_SLO_ABY: {
    uint16_t addr = get_address_aby(ctx, true);
    execute_slo(ctx, addr);
    break;
  }

  case OPCODE_ASL_ABS: {
    uint16_t addr = get_address_abs(ctx);
    execute_asl(ctx, addr);
    break;
  }

  case OPCODE_ASL_ABX: {
    uint16_t addr = get_address_abx(ctx, false);
    execute_asl(ctx, addr);
    break;
  }

  case OPCODE_ASL_ACC: {
    read_cpu(ctx, PC(ctx));
    execute_asl_acc(ctx);
    break;
  }

  case OPCODE_ASL_ZP0: {
    uint16_t addr = get_address_zp0(ctx);
    execute_asl(ctx, addr);
    break;
  }

  case OPCODE_ASL_ZPX: {
    uint16_t addr = get_address_zpx(ctx);
    execute_asl(ctx, addr);
    break;
  }

  case OPCODE_PHP_IMP: {
    read_cpu(ctx, PC(ctx));
    push_cpu(ctx, ctx->regP | CPU_FLAG_B | CPU_FLAG_U);
    break;
  }

    /*
     Other 177 more OPCODE needed to be implemented.
    */

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
    get_address_imm(ctx);
    break;
  }

  case OPCODE_NOP_ZP0:
  case OPCODE_NOP_ZP0_2:
  case OPCODE_NOP_ZP0_3: {
    uint16_t addr = get_address_zp0(ctx);
    read_cpu(ctx, addr);
    break;
  }

  case OPCODE_NOP_ZPX:
  case OPCODE_NOP_ZPX_2:
  case OPCODE_NOP_ZPX_3:
  case OPCODE_NOP_ZPX_4:
  case OPCODE_NOP_ZPX_5:
  case OPCODE_NOP_ZPX_6: {
    uint16_t addr = get_address_zpx(ctx);
    read_cpu(ctx, addr);
    break;
  }

  case OPCODE_NOP_ABS: {
    uint16_t addr = get_address_abs(ctx);
    read_cpu(ctx, addr);
    break;
  }

  case OPCODE_NOP_ABX:
  case OPCODE_NOP_ABX_2:
  case OPCODE_NOP_ABX_3:
  case OPCODE_NOP_ABX_4:
  case OPCODE_NOP_ABX_5:
  case OPCODE_NOP_ABX_6: {
    uint16_t addr = get_address_abx(ctx, false);
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

  case OPCODE_CLC_IMP: {
    read_cpu(ctx, PC(ctx));
    ctx->regP &= ~CPU_FLAG_C;
    break;
  }

  case OPCODE_SEC_IMP: {
    read_cpu(ctx, PC(ctx));
    ctx->regP |= CPU_FLAG_C;
    break;
  }

  case OPCODE_CLI_IMP: {
    read_cpu(ctx, PC(ctx));
    ctx->regP &= ~CPU_FLAG_I;
    break;
  }

  case OPCODE_SEI_IMP: {
    read_cpu(ctx, PC(ctx));
    ctx->regP |= CPU_FLAG_I;
    break;
  }

  case OPCODE_CLV_IMP: {
    read_cpu(ctx, PC(ctx));
    ctx->regP &= ~CPU_FLAG_V;
    break;
  }

  case OPCODE_CLD_IMP: {
    read_cpu(ctx, PC(ctx));
    ctx->regP &= ~CPU_FLAG_D;
    break;
  }

  case OPCODE_SED_IMP: {
    read_cpu(ctx, PC(ctx));
    ctx->regP |= CPU_FLAG_D;
    break;
  }

  case OPCODE_TAX_IMP: {
    read_cpu(ctx, PC(ctx));
    ctx->regX = ctx->regA;
    ctx->regP &= ~(CPU_FLAG_Z | CPU_FLAG_N);
    if (ctx->regX == 0)
      ctx->regP |= CPU_FLAG_Z;
    if (ctx->regX & 0x80)
      ctx->regP |= CPU_FLAG_N;
    break;
  }

  case OPCODE_TAY_IMP: {
    read_cpu(ctx, PC(ctx));
    ctx->regY = ctx->regA;
    ctx->regP &= ~(CPU_FLAG_Z | CPU_FLAG_N);
    if (ctx->regY == 0)
      ctx->regP |= CPU_FLAG_Z;
    if (ctx->regY & 0x80)
      ctx->regP |= CPU_FLAG_N;
    break;
  }

  case OPCODE_TXA_IMP: {
    read_cpu(ctx, PC(ctx));
    ctx->regA = ctx->regX;
    ctx->regP &= ~(CPU_FLAG_Z | CPU_FLAG_N);
    if (ctx->regA == 0)
      ctx->regP |= CPU_FLAG_Z;
    if (ctx->regA & 0x80)
      ctx->regP |= CPU_FLAG_N;
    break;
  }

  case OPCODE_TYA_IMP: {
    read_cpu(ctx, PC(ctx));
    ctx->regA = ctx->regY;
    ctx->regP &= ~(CPU_FLAG_Z | CPU_FLAG_N);
    if (ctx->regA == 0)
      ctx->regP |= CPU_FLAG_Z;
    if (ctx->regA & 0x80)
      ctx->regP |= CPU_FLAG_N;
    break;
  }

  case OPCODE_TSX_IMP: {
    read_cpu(ctx, PC(ctx));
    ctx->regX = ctx->regS;
    ctx->regP &= ~(CPU_FLAG_Z | CPU_FLAG_N);
    if (ctx->regX == 0)
      ctx->regP |= CPU_FLAG_Z;
    if (ctx->regX & 0x80)
      ctx->regP |= CPU_FLAG_N;
    break;
  }

  case OPCODE_TXS_IMP: {
    read_cpu(ctx, PC(ctx));
    ctx->regS = ctx->regX;
    break;
  }

  case OPCODE_INX_IMP: {
    read_cpu(ctx, PC(ctx));
    ctx->regX++;
    ctx->regP &= ~(CPU_FLAG_Z | CPU_FLAG_N);
    if (ctx->regX == 0)
      ctx->regP |= CPU_FLAG_Z;
    if (ctx->regX & 0x80)
      ctx->regP |= CPU_FLAG_N;
    break;
  }

  case OPCODE_INY_IMP: {
    read_cpu(ctx, PC(ctx));
    ctx->regY++;
    ctx->regP &= ~(CPU_FLAG_Z | CPU_FLAG_N);
    if (ctx->regY == 0)
      ctx->regP |= CPU_FLAG_Z;
    if (ctx->regY & 0x80)
      ctx->regP |= CPU_FLAG_N;
    break;
  }

  case OPCODE_DEX_IMP: {
    read_cpu(ctx, PC(ctx));
    ctx->regX--;
    ctx->regP &= ~(CPU_FLAG_Z | CPU_FLAG_N);
    if (ctx->regX == 0)
      ctx->regP |= CPU_FLAG_Z;
    if (ctx->regX & 0x80)
      ctx->regP |= CPU_FLAG_N;
    break;
  }

  case OPCODE_DEY_IMP: {
    read_cpu(ctx, PC(ctx));
    ctx->regY--;
    ctx->regP &= ~(CPU_FLAG_Z | CPU_FLAG_N);
    if (ctx->regY == 0)
      ctx->regP |= CPU_FLAG_Z;
    if (ctx->regY & 0x80)
      ctx->regP |= CPU_FLAG_N;
    break;
  }
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
