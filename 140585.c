static void vcpu_scan_ioapic(struct kvm_vcpu *vcpu)
{
	u64 eoi_exit_bitmap[4];
	u32 tmr[8];

	if (!kvm_apic_hw_enabled(vcpu->arch.apic))
		return;

	memset(eoi_exit_bitmap, 0, 32);
	memset(tmr, 0, 32);

	kvm_ioapic_scan_entry(vcpu, eoi_exit_bitmap, tmr);
	kvm_x86_ops->load_eoi_exitmap(vcpu, eoi_exit_bitmap);
	kvm_apic_update_tmr(vcpu, tmr);
}