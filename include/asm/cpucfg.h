#ifndef CPUCFG_H
#define CPUCFG_H

static inline uint64_t read_cpucfg(uint64_t x) {
    uint64_t cfg;
    asm volatile("cpucfg %1, %0" : "=r"(cfg) : "r" (x) );
    return cfg;
}

struct cpucfg0 {
   uint32_t PRID; 
};

extern struct cpucfg0 CPUCFG0;

struct cpucfg1 {
    unsigned ARCH : 2;
    unsigned PGMMU : 1;
    unsigned IOCSR: 1;
    unsigned PALEN: 8;
    unsigned VALEN: 8;
    unsigned UAL: 1;
    unsigned RI: 1;
    unsigned EP: 1;
    unsigned RPLV: 1;
    unsigned HP: 1;
    unsigned CRC: 1;
    unsigned MSG_INT: 1;
};

extern struct cpucfg1 CPUCFG1;

// void init_cpucfg(){
    // CPUCFG0 =

// struct cpucfg2 {
//

#endif /* !CPUCFG_H */
