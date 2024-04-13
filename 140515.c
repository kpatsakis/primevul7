static inline bool vm_need_virtualize_apic_accesses(struct kvm *kvm)
{
	return flexpriority_enabled && irqchip_in_kernel(kvm);
}