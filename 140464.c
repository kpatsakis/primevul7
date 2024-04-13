static int nested_vmx_check_permission(struct kvm_vcpu *vcpu)
{
	struct kvm_segment cs;
	struct vcpu_vmx *vmx = to_vmx(vcpu);

	if (!vmx->nested.vmxon) {
		kvm_queue_exception(vcpu, UD_VECTOR);
		return 0;
	}

	vmx_get_segment(vcpu, &cs, VCPU_SREG_CS);
	if ((vmx_get_rflags(vcpu) & X86_EFLAGS_VM) ||
	    (is_long_mode(vcpu) && !cs.l)) {
		kvm_queue_exception(vcpu, UD_VECTOR);
		return 0;
	}

	if (vmx_get_cpl(vcpu)) {
		kvm_inject_gp(vcpu, 0);
		return 0;
	}

	return 1;
}