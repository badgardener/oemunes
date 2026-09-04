#ifndef CPU6502_INSTRUCTIONS_H
#define CPU6502_INSTRUCTIONS_H

#include "cpu6502.h"

uint8_t read_cpu(CPU *ctx, uint16_t addr);
void write_cpu(CPU *ctx, uint16_t addr, uint8_t val);

void push_cpu(CPU *ctx, uint8_t val);
uint8_t pop_cpu(CPU *ctx);

uint16_t PC(CPU *ctx);
void incrementPC(CPU *ctx);

uint16_t get_address_imm(CPU *ctx);
uint16_t get_address_zp0(CPU *ctx);
uint16_t get_address_zpx(CPU *ctx);
uint16_t get_address_zpy(CPU *ctx);
uint16_t get_address_abs(CPU *ctx);
uint16_t get_address_abx(CPU *ctx, bool force_dummy);
uint16_t get_address_aby(CPU *ctx, bool force_dummy);
uint16_t get_address_rel(CPU *ctx);
uint16_t get_address_ind(CPU *ctx);
uint16_t get_address_izx(CPU *ctx);
uint16_t get_address_izy(CPU *ctx, bool force_dummy);

void execute_slo(CPU *ctx, uint16_t addr);
void execute_ora(CPU *ctx, uint16_t addr);
void execute_asl(CPU *ctx, uint16_t addr);

void execute_asl_acc(CPU *ctx);

#endif // CPU6502_INSTRUCTIONS_H
