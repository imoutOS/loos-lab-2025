# Context swtch
#
#   void swtch(struct context *old, struct context *new);
# # Save current registers in old. Load from new.	

.globl what
.globl swtch
#swtch:
#        st.d $ra, $a0, 0
#        st.d $sp, $a0, 8
#        st.d $a0, $a0, 16
#        st.d $a1, $a0, 24
#        st.d $a2, $a0, 32
#        st.d $a3, $a0, 40
#        st.d $a4, $a0, 48
#        st.d $a5, $a0, 56
#        st.d $a6, $a0, 64
#        st.d $a7, $a0, 72
#        st.d $tp, $a0, 80
#        st.d $fp, $a0, 88
#
#        ld.d $ra, $a1, 0
#        ld.d $sp, $a1, 8
#        ld.d $a0, $a1, 16
#        ld.d $a2, $a1, 32
#        ld.d $a3, $a1, 40
#        ld.d $a4, $a1, 48
#        ld.d $a5, $a1, 56
#        ld.d $a6, $a1, 64
#        ld.d $a7, $a1, 72
#        ld.d $tp, $a1, 80
#        ld.d $fp, $a1, 88
#        # load a1 at last
#        ld.d $a1, $a1, 24
#        
#        jirl $zero, $ra, 0

swtch:
    # save the registers.
    st.d $ra, $a0, 0
    st.d $tp, $a0, 8
    st.d $sp, $a0, 16
    st.d $a0, $a0, 24
    st.d $a1, $a0, 32
    st.d $a2, $a0, 40
    st.d $a3, $a0, 48
    st.d $a4, $a0, 56
    st.d $a5, $a0, 64
    st.d $a6, $a0, 72
    st.d $a7, $a0, 80
    st.d $t0, $a0, 88
    st.d $t1, $a0, 96
    st.d $t2, $a0, 104
    st.d $t3, $a0, 112
    st.d $t4, $a0, 120
    st.d $t5, $a0, 128
    st.d $t6, $a0, 136
    st.d $t7, $a0, 144
    st.d $t8, $a0, 152
    st.d $r21, $a0,160
    st.d $fp, $a0, 168
    st.d $s0, $a0, 176
    st.d $s1, $a0, 184
    st.d $s2, $a0, 192
    st.d $s3, $a0, 200
    st.d $s4, $a0, 208
    st.d $s5, $a0, 216
    st.d $s6, $a0, 224
    st.d $s7, $a0, 232
    st.d $s8, $a0, 240

    # restore register
    ld.d $ra, $a1, 0
    ld.d $tp, $a1, 8
    ld.d $sp, $a1, 16
    ld.d $a0, $a1, 24
    ld.d $a2, $a1, 40
    ld.d $a3, $a1, 48
    ld.d $a4, $a1, 56
    ld.d $a5, $a1, 64
    ld.d $a6, $a1, 72
    ld.d $a7, $a1, 80
    ld.d $t0, $a1, 88
    ld.d $t1, $a1, 96
    ld.d $t2, $a1, 104
    ld.d $t3, $a1, 112
    ld.d $t4, $a1, 120
    ld.d $t5, $a1, 128
    ld.d $t6, $a1, 136
    ld.d $t7, $a1, 144
    ld.d $t8, $a1, 152
    ld.d $r21, $a1,160
    ld.d $fp, $a1, 168
    ld.d $s0, $a1, 176
    ld.d $s1, $a1, 184
    ld.d $s2, $a1, 192
    ld.d $s3, $a1, 200
    ld.d $s4, $a1, 208
    ld.d $s5, $a1, 216
    ld.d $s6, $a1, 224
    ld.d $s7, $a1, 232
    ld.d $s8, $a1, 240
    ld.d $a1, $a1, 32

    jr $ra
	

.globl trap_exit
.globl exec_helper
exec_helper:
    b trap_exit
exec_exit_fail:
    b exec_exit_fail

