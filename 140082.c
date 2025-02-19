static int handle_dr(struct kvm_vcpu *vcpu)
{
	unsigned long exit_qualification;
	int dr, reg;

	/* Do not handle if the CPL > 0, will trigger GP on re-entry */
	if (!kvm_require_cpl(vcpu, 0))
		return 1;
	dr = vmcs_readl(GUEST_DR7);
	if (dr & DR7_GD) {
		/*
		 * As the vm-exit takes precedence over the debug trap, we
		 * need to emulate the latter, either for the host or the
		 * guest debugging itself.
		 */
		if (vcpu->guest_debug & KVM_GUESTDBG_USE_HW_BP) {
			vcpu->run->debug.arch.dr6 = vcpu->arch.dr6;
			vcpu->run->debug.arch.dr7 = dr;
			vcpu->run->debug.arch.pc =
				vmcs_readl(GUEST_CS_BASE) +
				vmcs_readl(GUEST_RIP);
			vcpu->run->debug.arch.exception = DB_VECTOR;
			vcpu->run->exit_reason = KVM_EXIT_DEBUG;
			return 0;
		} else {
			vcpu->arch.dr7 &= ~DR7_GD;
			vcpu->arch.dr6 |= DR6_BD | DR6_RTM;
			vmcs_writel(GUEST_DR7, vcpu->arch.dr7);
			kvm_queue_exception(vcpu, DB_VECTOR);
			return 1;
		}
	}

	if (vcpu->guest_debug == 0) {
		u32 cpu_based_vm_exec_control;

		cpu_based_vm_exec_control = vmcs_read32(CPU_BASED_VM_EXEC_CONTROL);
		cpu_based_vm_exec_control &= ~CPU_BASED_MOV_DR_EXITING;
		vmcs_write32(CPU_BASED_VM_EXEC_CONTROL, cpu_based_vm_exec_control);

		/*
		 * No more DR vmexits; force a reload of the debug registers
		 * and reenter on this instruction.  The next vmexit will
		 * retrieve the full state of the debug registers.
		 */
		vcpu->arch.switch_db_regs |= KVM_DEBUGREG_WONT_EXIT;
		return 1;
	}

	exit_qualification = vmcs_readl(EXIT_QUALIFICATION);
	dr = exit_qualification & DEBUG_REG_ACCESS_NUM;
	reg = DEBUG_REG_ACCESS_REG(exit_qualification);
	if (exit_qualification & TYPE_MOV_FROM_DR) {
		unsigned long val;

		if (kvm_get_dr(vcpu, dr, &val))
			return 1;
		kvm_register_write(vcpu, reg, val);
	} else
		if (kvm_set_dr(vcpu, dr, kvm_register_readl(vcpu, reg)))
			return 1;

	skip_emulated_instruction(vcpu);
	return 1;
}