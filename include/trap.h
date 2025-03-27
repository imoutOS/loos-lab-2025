#ifndef TRAP_H
#define TRAP_H

void trap_entry();
void trap_handler();
void trap_init();

uint64_t r_trap_era() ;
void  w_trap_era(uint64_t era) ;

#endif /* !TRAP_H */
