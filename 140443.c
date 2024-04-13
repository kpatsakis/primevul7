static int get_vmx_mem_address(struct kvm_vcpu *vcpu,
				 unsigned long exit_qualification,
				 u32 vmx_instruction_info, gva_t *ret)
{
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
	*ret = vmx_get_segment_base(vcpu, seg_reg);
	if (base_is_valid)
		*ret += kvm_register_read(vcpu, base_reg);
	if (index_is_valid)
		*ret += kvm_register_read(vcpu, index_reg)<<scaling;
	*ret += exit_qualification; /* holds the displacement */

	if (addr_size == 1) /* 32 bit */
		*ret &= 0xffffffff;

	/*
	 * TODO: throw #GP (and return 1) in various cases that the VM*
	 * instructions require it - e.g., offset beyond segment limit,
	 * unusable or unreadable/unwritable segment, non-canonical 64-bit
	 * address, and so on. Currently these are not checked.
	 */
	return 0;
}