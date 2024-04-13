static int vmx_check_nested_events(struct kvm_vcpu *vcpu, bool external_intr)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);

	if (nested_cpu_has_preemption_timer(get_vmcs12(vcpu)) &&
	    vmx->nested.preemption_timer_expired) {
		if (vmx->nested.nested_run_pending)
			return -EBUSY;
		nested_vmx_vmexit(vcpu, EXIT_REASON_PREEMPTION_TIMER, 0, 0);
		return 0;
	}

	if (vcpu->arch.nmi_pending && nested_exit_on_nmi(vcpu)) {
		if (vmx->nested.nested_run_pending ||
		    vcpu->arch.interrupt.pending)
			return -EBUSY;
		nested_vmx_vmexit(vcpu, EXIT_REASON_EXCEPTION_NMI,
				  NMI_VECTOR | INTR_TYPE_NMI_INTR |
				  INTR_INFO_VALID_MASK, 0);
		/*
		 * The NMI-triggered VM exit counts as injection:
		 * clear this one and block further NMIs.
		 */
		vcpu->arch.nmi_pending = 0;
		vmx_set_nmi_mask(vcpu, true);
		return 0;
	}

	if ((kvm_cpu_has_interrupt(vcpu) || external_intr) &&
	    nested_exit_on_intr(vcpu)) {
		if (vmx->nested.nested_run_pending)
			return -EBUSY;
		nested_vmx_vmexit(vcpu, EXIT_REASON_EXTERNAL_INTERRUPT, 0, 0);
	}

	return 0;
}