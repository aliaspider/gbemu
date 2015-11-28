#ifndef CPUDISASM_H
#define CPUDISASM_H

#include "cpu.h"

int gbemu_disasm_current(gbemu_cpu_t* CPU);
void gbemu_dump_state(gbemu_cpu_t* CPU);

#endif // CPUDISASM_H
