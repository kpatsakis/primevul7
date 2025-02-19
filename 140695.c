static void kvm_vcpu_check_singlestep(struct kvm_vcpu *vcpu, unsigned long rflags, int *r)
{
	struct kvm_run *kvm_run = vcpu->run;

	/*
	 * rflags is the old, "raw" value of the flags.  The new value has
	 * not been saved yet.
	 *
	 * This is correct even for TF set by the guest, because "the
	 * processor will not generate this exception after the instruction
	 * that sets the TF flag".
	 */
	if (unlikely(rflags & X86_EFLAGS_TF)) {
		if (vcpu->guest_debug & KVM_GUESTDBG_SINGLESTEP) {
			kvm_run->debug.arch.dr6 = DR6_BS | DR6_FIXED_1 |
						  DR6_RTM;
			kvm_run->debug.arch.pc = vcpu->arch.singlestep_rip;
			kvm_run->debug.arch.exception = DB_VECTOR;
			kvm_run->exit_reason = KVM_EXIT_DEBUG;
			*r = EMULATE_USER_EXIT;
		} else {
			vcpu->arch.emulate_ctxt.eflags &= ~X86_EFLAGS_TF;
			/*
			 * "Certain debug exceptions may clear bit 0-3.  The
			 * remaining contents of the DR6 register are never
			 * cleared by the processor".
			 */
			vcpu->arch.dr6 &= ~15;
			vcpu->arch.dr6 |= DR6_BS | DR6_RTM;
			kvm_queue_exception(vcpu, DB_VECTOR);
		}
	}
}