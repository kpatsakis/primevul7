static int backtrack_insn(struct bpf_verifier_env *env, int idx,
			  u32 *reg_mask, u64 *stack_mask)
{
	const struct bpf_insn_cbs cbs = {
		.cb_call	= disasm_kfunc_name,
		.cb_print	= verbose,
		.private_data	= env,
	};
	struct bpf_insn *insn = env->prog->insnsi + idx;
	u8 class = BPF_CLASS(insn->code);
	u8 opcode = BPF_OP(insn->code);
	u8 mode = BPF_MODE(insn->code);
	u32 dreg = 1u << insn->dst_reg;
	u32 sreg = 1u << insn->src_reg;
	u32 spi;

	if (insn->code == 0)
		return 0;
	if (env->log.level & BPF_LOG_LEVEL) {
		verbose(env, "regs=%x stack=%llx before ", *reg_mask, *stack_mask);
		verbose(env, "%d: ", idx);
		print_bpf_insn(&cbs, insn, env->allow_ptr_leaks);
	}

	if (class == BPF_ALU || class == BPF_ALU64) {
		if (!(*reg_mask & dreg))
			return 0;
		if (opcode == BPF_MOV) {
			if (BPF_SRC(insn->code) == BPF_X) {
				/* dreg = sreg
				 * dreg needs precision after this insn
				 * sreg needs precision before this insn
				 */
				*reg_mask &= ~dreg;
				*reg_mask |= sreg;
			} else {
				/* dreg = K
				 * dreg needs precision after this insn.
				 * Corresponding register is already marked
				 * as precise=true in this verifier state.
				 * No further markings in parent are necessary
				 */
				*reg_mask &= ~dreg;
			}
		} else {
			if (BPF_SRC(insn->code) == BPF_X) {
				/* dreg += sreg
				 * both dreg and sreg need precision
				 * before this insn
				 */
				*reg_mask |= sreg;
			} /* else dreg += K
			   * dreg still needs precision before this insn
			   */
		}
	} else if (class == BPF_LDX) {
		if (!(*reg_mask & dreg))
			return 0;
		*reg_mask &= ~dreg;

		/* scalars can only be spilled into stack w/o losing precision.
		 * Load from any other memory can be zero extended.
		 * The desire to keep that precision is already indicated
		 * by 'precise' mark in corresponding register of this state.
		 * No further tracking necessary.
		 */
		if (insn->src_reg != BPF_REG_FP)
			return 0;

		/* dreg = *(u64 *)[fp - off] was a fill from the stack.
		 * that [fp - off] slot contains scalar that needs to be
		 * tracked with precision
		 */
		spi = (-insn->off - 1) / BPF_REG_SIZE;
		if (spi >= 64) {
			verbose(env, "BUG spi %d\n", spi);
			WARN_ONCE(1, "verifier backtracking bug");
			return -EFAULT;
		}
		*stack_mask |= 1ull << spi;
	} else if (class == BPF_STX || class == BPF_ST) {
		if (*reg_mask & dreg)
			/* stx & st shouldn't be using _scalar_ dst_reg
			 * to access memory. It means backtracking
			 * encountered a case of pointer subtraction.
			 */
			return -ENOTSUPP;
		/* scalars can only be spilled into stack */
		if (insn->dst_reg != BPF_REG_FP)
			return 0;
		spi = (-insn->off - 1) / BPF_REG_SIZE;
		if (spi >= 64) {
			verbose(env, "BUG spi %d\n", spi);
			WARN_ONCE(1, "verifier backtracking bug");
			return -EFAULT;
		}
		if (!(*stack_mask & (1ull << spi)))
			return 0;
		*stack_mask &= ~(1ull << spi);
		if (class == BPF_STX)
			*reg_mask |= sreg;
	} else if (class == BPF_JMP || class == BPF_JMP32) {
		if (opcode == BPF_CALL) {
			if (insn->src_reg == BPF_PSEUDO_CALL)
				return -ENOTSUPP;
			/* regular helper call sets R0 */
			*reg_mask &= ~1;
			if (*reg_mask & 0x3f) {
				/* if backtracing was looking for registers R1-R5
				 * they should have been found already.
				 */
				verbose(env, "BUG regs %x\n", *reg_mask);
				WARN_ONCE(1, "verifier backtracking bug");
				return -EFAULT;
			}
		} else if (opcode == BPF_EXIT) {
			return -ENOTSUPP;
		}
	} else if (class == BPF_LD) {
		if (!(*reg_mask & dreg))
			return 0;
		*reg_mask &= ~dreg;
		/* It's ld_imm64 or ld_abs or ld_ind.
		 * For ld_imm64 no further tracking of precision
		 * into parent is necessary
		 */
		if (mode == BPF_IND || mode == BPF_ABS)
			/* to be analyzed */
			return -ENOTSUPP;
	}
	return 0;
}