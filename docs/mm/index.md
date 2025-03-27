# LoOS 中的内存管理

## 直接映射的内存管理

```c
void dwm_init() {
    w_csr_dwm0(get_dmw_data(0x8000, 0, DMW_PLV0));
    w_csr_dwm1(get_dmw_data(0x9000, 1, DMW_PLV0));
    w_csr_dwm2(get_dmw_data(0x0000, 1, DMW_PLV0 | DMW_PLV3));
}
```

## 物理内存分配

主要在 src/core/kalloc.c 里面有。

## 虚拟内存分配

## 内存相关异常处理
