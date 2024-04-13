int get_vmx_mem_address(struct kvm_vcpu *vcpu, unsigned long exit_qualification,
			u32 vmx_instruction_info, bool wr, gva_t *ret)
{
	gva_t off;
	bool exn;
	struct kvm_segment s;

	/*
	 * According to Vol. 3B, "Information for VM Exits Due to Instruction
	 * Execution", on an exit, vmx_instruction_info holds most of the
	 * addressing components of the operand. Only the displacement part
	 * is put in exit_qualification (see 3B, "Basic VM-Exit Information").
	 * For how an actual address is calculated from all these components,
	 * refer to Vol. 1, "Operand Addressing".
	 */
	int  scaling = vmx_instruction_info & 3;
	int  addr_size = (vmx_instruction_info >> 7) & 7;
	bool is_reg = vmx_instruction_info & (1u << 10);
	int  seg_reg = (vmx_instruction_info >> 15) & 7;
	int  index_reg = (vmx_instruction_info >> 18) & 0xf;
	bool index_is_valid = !(vmx_instruction_info & (1u << 22));
	int  base_reg       = (vmx_instruction_info >> 23) & 0xf;
	bool base_is_valid  = !(vmx_instruction_info & (1u << 27));

	if (is_reg) {
		kvm_queue_exception(vcpu, UD_VECTOR);
		return 1;
	}

	/* Addr = segment_base + offset */
	/* offset = base + [index * scale] + displacement */
	off = exit_qualification; /* holds the displacement */
	if (addr_size == 1)
		off = (gva_t)sign_extend64(off, 31);
	else if (addr_size == 0)
		off = (gva_t)sign_extend64(off, 15);
	if (base_is_valid)
		off += kvm_register_read(vcpu, base_reg);
	if (index_is_valid)
		off += kvm_register_read(vcpu, index_reg)<<scaling;
	vmx_get_segment(vcpu, &s, seg_reg);

	/*
	 * The effective address, i.e. @off, of a memory operand is truncated
	 * based on the address size of the instruction.  Note that this is
	 * the *effective address*, i.e. the address prior to accounting for
	 * the segment's base.
	 */
	if (addr_size == 1) /* 32 bit */
		off &= 0xffffffff;
	else if (addr_size == 0) /* 16 bit */
		off &= 0xffff;

	/* Checks for #GP/#SS exceptions. */
	exn = false;
	if (is_long_mode(vcpu)) {
		/*
		 * The virtual/linear address is never truncated in 64-bit
		 * mode, e.g. a 32-bit address size can yield a 64-bit virtual
		 * address when using FS/GS with a non-zero base.
		 */
		*ret = s.base + off;

		/* Long mode: #GP(0)/#SS(0) if the memory address is in a
		 * non-canonical form. This is the only check on the memory
		 * destination for long mode!
		 */
		exn = is_noncanonical_address(*ret, vcpu);
	} else {
		/*
		 * When not in long mode, the virtual/linear address is
		 * unconditionally truncated to 32 bits regardless of the
		 * address size.
		 */
		*ret = (s.base + off) & 0xffffffff;

		/* Protected mode: apply checks for segment validity in the
		 * following order:
		 * - segment type check (#GP(0) may be thrown)
		 * - usability check (#GP(0)/#SS(0))
		 * - limit check (#GP(0)/#SS(0))
		 */
		if (wr)
			/* #GP(0) if the destination operand is located in a
			 * read-only data segment or any code segment.
			 */
			exn = ((s.type & 0xa) == 0 || (s.type & 8));
		else
			/* #GP(0) if the source operand is located in an
			 * execute-only code segment
			 */
			exn = ((s.type & 0xa) == 8);
		if (exn) {
			kvm_queue_exception_e(vcpu, GP_VECTOR, 0);
			return 1;
		}
		/* Protected mode: #GP(0)/#SS(0) if the segment is unusable.
		 */
		exn = (s.unusable != 0);

		/*
		 * Protected mode: #GP(0)/#SS(0) if the memory operand is
		 * outside the segment limit.  All CPUs that support VMX ignore
		 * limit checks for flat segments, i.e. segments with base==0,
		 * limit==0xffffffff and of type expand-up data or code.
		 */
		if (!(s.base == 0 && s.limit == 0xffffffff &&
		     ((s.type & 8) || !(s.type & 4))))
			exn = exn || (off + sizeof(u64) > s.limit);
	}
	if (exn) {
		kvm_queue_exception_e(vcpu,
				      seg_reg == VCPU_SREG_SS ?
						SS_VECTOR : GP_VECTOR,
				      0);
		return 1;
	}

	return 0;
}