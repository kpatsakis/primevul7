static void vmx_cpuid_update(struct kvm_vcpu *vcpu)
{
	struct kvm_cpuid_entry2 *best;
	struct vcpu_vmx *vmx = to_vmx(vcpu);
	u32 exec_control;

	vmx->rdtscp_enabled = false;
	if (vmx_rdtscp_supported()) {
		exec_control = vmcs_read32(SECONDARY_VM_EXEC_CONTROL);
		if (exec_control & SECONDARY_EXEC_RDTSCP) {
			best = kvm_find_cpuid_entry(vcpu, 0x80000001, 0);
			if (best && (best->edx & bit(X86_FEATURE_RDTSCP)))
				vmx->rdtscp_enabled = true;
			else {
				exec_control &= ~SECONDARY_EXEC_RDTSCP;
				vmcs_write32(SECONDARY_VM_EXEC_CONTROL,
						exec_control);
			}
		}
	}

	/* Exposing INVPCID only when PCID is exposed */
	best = kvm_find_cpuid_entry(vcpu, 0x7, 0);
	if (vmx_invpcid_supported() &&
	    best && (best->ebx & bit(X86_FEATURE_INVPCID)) &&
	    guest_cpuid_has_pcid(vcpu)) {
		exec_control = vmcs_read32(SECONDARY_VM_EXEC_CONTROL);
		exec_control |= SECONDARY_EXEC_ENABLE_INVPCID;
		vmcs_write32(SECONDARY_VM_EXEC_CONTROL,
			     exec_control);
	} else {
		if (cpu_has_secondary_exec_ctrls()) {
			exec_control = vmcs_read32(SECONDARY_VM_EXEC_CONTROL);
			exec_control &= ~SECONDARY_EXEC_ENABLE_INVPCID;
			vmcs_write32(SECONDARY_VM_EXEC_CONTROL,
				     exec_control);
		}
		if (best)
			best->ebx &= ~bit(X86_FEATURE_INVPCID);
	}
}