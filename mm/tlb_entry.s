.section tlbrentry
.globl handle_tlbr
.align 0x4
handle_tlbr:
    csrwr	$t0, 0x8B
    csrrd	$t0, 0x1B
    lddir   $t0, $t0, 3
    lddir   $t0, $t0, 1
    ldpte	$t0, 0
    ldpte	$t0, 1
    tlbfill
    csrrd	$t0, 0x8B
    ertn
