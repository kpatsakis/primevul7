static void vmx_write_tsc_offset(struct kvm_vcpu *vcpu, u64 offset)
{
	if (is_guest_mode(vcpu)) {
		/*
		 * We're here if L1 chose not to trap WRMSR to TSC. According
		 * to the spec, this should set L1's TSC; The offset that L1
		 * set for L2 remains unchanged, and still needs to be added
		 * to the newly set TSC to get L2's TSC.
		 */
		struct vmcs12 *vmcs12;
		to_vmx(vcpu)->nested.vmcs01_tsc_offset = offset;
		/* recalculate vmcs02.TSC_OFFSET: */
		vmcs12 = get_vmcs12(vcpu);
		vmcs_write64(TSC_OFFSET, offset +
			(nested_cpu_has(vmcs12, CPU_BASED_USE_TSC_OFFSETING) ?
			 vmcs12->tsc_offset : 0));
	} else {
		trace_kvm_write_tsc_offset(vcpu->vcpu_id,
					   vmcs_read64(TSC_OFFSET), offset);
		vmcs_write64(TSC_OFFSET, offset);
	}
}