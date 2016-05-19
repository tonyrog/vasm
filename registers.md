# Registers used by RV32I

	RV32I   FP     ABI1  | ABIC | 
	===================RVE registers=====================
	+-----+------+
	| x0  | f0   | zero  | 
	| x1  | f1   | ra    |
	| x2  | f2   | s0/fp |
	| x3  | f3   | s1    | called-saved register s1
	| x4  | f4   | s2    | called-saved register s2
	| x5  | f5   | s3    |
	| x6  | f6   | s4    |
	| x7  | f7   | s5    |
	------+------+------- RVC registers ----------
	| x8  | f8   | s6    | s0 fs0 
	| x9  | f9   | s7    | s1 fs1
	| x10 | f10  | s8    | a0 fa0
	| x11 | f11  | s9    | a1 fa1
    | x12 | f12  | s10   | a2 fa2
    | x13 | f13  | s11   | a3 fa3  called-saved register s11
	| x14 | f14  | sp    | a4 fa4  stack pointer
    | x15 | f15  | tp    | a5 fa5  thread local data
    -----------------------------------------
    ====================================================
    | x16 | f16  | v0    | return value1
    | x17 | f17  | v1    | return value2
    | x18 | f18  | a0    | function arg0
    | x19 | f19  | a1    | function arg1
    | x20 | f20  | a2    |
    | x21 | f21  | a3    |
    | x22 | f22  | a4    |
    | x23 | f23  | a7    |
    | x24 | f24  | a6    |
    | x25 | f25  | a7    | function arg7
    | x26 | f26  | t0    | temorary0 ( caller saved )
    | x27 | f27  | t1    | 
    | x28 | f28  | t2    |
    | x29 | f29  | t3    |
    | x30 | f30  | t4    | temorary4
    | x31 | f31  | gp    | global data
    +-----+------+-------+
