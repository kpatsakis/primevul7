static int set_msr_hyperv(struct kvm_vcpu *vcpu, u32 msr, u64 data)
{
	switch (msr) {
	case HV_X64_MSR_APIC_ASSIST_PAGE: {
		u64 gfn;
		unsigned long addr;

		if (!(data & HV_X64_MSR_APIC_ASSIST_PAGE_ENABLE)) {
			vcpu->arch.hv_vapic = data;
			if (kvm_lapic_enable_pv_eoi(vcpu, 0))
				return 1;
			break;
		}
		gfn = data >> HV_X64_MSR_APIC_ASSIST_PAGE_ADDRESS_SHIFT;
		addr = gfn_to_hva(vcpu->kvm, gfn);
		if (kvm_is_error_hva(addr))
			return 1;
		if (__clear_user((void __user *)addr, PAGE_SIZE))
			return 1;
		vcpu->arch.hv_vapic = data;
		mark_page_dirty(vcpu->kvm, gfn);
		if (kvm_lapic_enable_pv_eoi(vcpu, gfn_to_gpa(gfn) | KVM_MSR_ENABLED))
			return 1;
		break;
	}
	case HV_X64_MSR_EOI:
		return kvm_hv_vapic_msr_write(vcpu, APIC_EOI, data);
	case HV_X64_MSR_ICR:
		return kvm_hv_vapic_msr_write(vcpu, APIC_ICR, data);
	case HV_X64_MSR_TPR:
		return kvm_hv_vapic_msr_write(vcpu, APIC_TASKPRI, data);
	default:
		vcpu_unimpl(vcpu, "HYPER-V unimplemented wrmsr: 0x%x "
			    "data 0x%llx\n", msr, data);
		return 1;
	}

	return 0;
}