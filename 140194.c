static void kvm_update_dr7(struct kvm_vcpu *vcpu)
{
	unsigned long dr7;

	if (vcpu->guest_debug & KVM_GUESTDBG_USE_HW_BP)
		dr7 = vcpu->arch.guest_debug_dr7;
	else
		dr7 = vcpu->arch.dr7;
	kvm_x86_ops->set_dr7(vcpu, dr7);
	vcpu->arch.switch_db_regs &= ~KVM_DEBUGREG_BP_ENABLED;
	if (dr7 & DR7_BP_EN_MASK)
		vcpu->arch.switch_db_regs |= KVM_DEBUGREG_BP_ENABLED;
}