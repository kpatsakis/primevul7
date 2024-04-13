int kvm_set_apic_base(struct kvm_vcpu *vcpu, struct msr_data *msr_info)
{
	u64 old_state = vcpu->arch.apic_base &
		(MSR_IA32_APICBASE_ENABLE | X2APIC_ENABLE);
	u64 new_state = msr_info->data &
		(MSR_IA32_APICBASE_ENABLE | X2APIC_ENABLE);
	u64 reserved_bits = ((~0ULL) << cpuid_maxphyaddr(vcpu)) |
		0x2ff | (guest_cpuid_has_x2apic(vcpu) ? 0 : X2APIC_ENABLE);

	if (!msr_info->host_initiated &&
	    ((msr_info->data & reserved_bits) != 0 ||
	     new_state == X2APIC_ENABLE ||
	     (new_state == MSR_IA32_APICBASE_ENABLE &&
	      old_state == (MSR_IA32_APICBASE_ENABLE | X2APIC_ENABLE)) ||
	     (new_state == (MSR_IA32_APICBASE_ENABLE | X2APIC_ENABLE) &&
	      old_state == 0)))
		return 1;

	kvm_lapic_set_base(vcpu, msr_info->data);
	return 0;
}