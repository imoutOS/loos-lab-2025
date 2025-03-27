#ifndef ASM_TLB_H
#define ASM_TLB_H

static inline void tlbclr(void)
{
	__asm__ __volatile__("tlbclr");
}

static inline void tlbflush(void)
{
	__asm__ __volatile__("tlbflush");
}

enum invtlb_ops {
	/* Invalid all tlb */
	INVTLB_ALL = 0x0,
	/* Invalid current tlb */
	INVTLB_CURRENT_ALL = 0x1,
	/* Invalid all global=1 lines in current tlb */
	INVTLB_CURRENT_GTRUE = 0x2,
	/* Invalid all global=0 lines in current tlb */
	INVTLB_CURRENT_GFALSE = 0x3,
	/* Invalid global=0 and matched asid lines in current tlb */
	INVTLB_GFALSE_AND_ASID = 0x4,
	/* Invalid addr with global=0 and matched asid in current tlb */
	INVTLB_ADDR_GFALSE_AND_ASID = 0x5,
	/* Invalid addr with global=1 or matched asid in current tlb */
	INVTLB_ADDR_GTRUE_OR_ASID = 0x6,
	/* Invalid matched gid in guest tlb */
	INVGTLB_GID = 0x9,
	/* Invalid global=1, matched gid in guest tlb */
	INVGTLB_GID_GTRUE = 0xa,
	/* Invalid global=0, matched gid in guest tlb */
	INVGTLB_GID_GFALSE = 0xb,
	/* Invalid global=0, matched gid and asid in guest tlb */
	INVGTLB_GID_GFALSE_ASID = 0xc,
	/* Invalid global=0 , matched gid, asid and addr in guest tlb */
	INVGTLB_GID_GFALSE_ASID_ADDR = 0xd,
	/* Invalid global=1 , matched gid, asid and addr in guest tlb */
	INVGTLB_GID_GTRUE_ASID_ADDR = 0xe,
	/* Invalid all gid gva-->gpa guest tlb */
	INVGTLB_ALLGID_GVA_TO_GPA = 0x10,
	/* Invalid all gid gpa-->hpa tlb */
	INVTLB_ALLGID_GPA_TO_HPA = 0x11,
	/* Invalid all gid tlb, including  gva-->gpa and gpa-->hpa */
	INVTLB_ALLGID = 0x12,
	/* Invalid matched gid gva-->gpa guest tlb */
	INVGTLB_GID_GVA_TO_GPA = 0x13,
	/* Invalid matched gid gpa-->hpa tlb */
	INVTLB_GID_GPA_TO_HPA = 0x14,
	/* Invalid matched gid tlb,including gva-->gpa and gpa-->hpa */
	INVTLB_GID_ALL = 0x15,
	/* Invalid matched gid and addr gpa-->hpa tlb */
	INVTLB_GID_ADDR = 0x16,
};

static inline void invtlb(unsigned int op, unsigned int info, unsigned long long addr)
{
	__asm__ __volatile__(
		"invtlb %0, %1, %2\n\t"
		:
		: "i"(op), "r"(info), "r"(addr)
		: "memory"
		);
}

static inline void invtlb_addr(unsigned int op, unsigned long long addr)
{
	__asm__ __volatile__(
		"invtlb %0, $zero, %1\n\t"
		:
		: "i"(op), "r"(addr)
		: "memory"
		);
}

static inline void invtlb_info(unsigned int op, unsigned int info)
{
	__asm__ __volatile__(
		"invtlb %0, %1, $zero\n\t"
		:
		: "i"(op), "r"(info)
		: "memory"
		);
}

static inline void invtlb_all()
{
	__asm__ __volatile__(
		"invtlb 0x0, $zero, $zero\n\t"
		);
}

#endif /* !ASM_TLB_H */
