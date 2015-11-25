#ifndef GBEMU_H
#define GBEMU_H

#define DEBUG_HOLD() do{printf("%s@%s:%d.\n",__FUNCTION__, __FILE__, __LINE__);fflush(stdout);gbemu_wait_for_input();}while(0)
void gbemu_wait_for_input(void);

#endif // GBEMU_H
