static bool nested_get_vmcs12_pages(struct kvm_vcpu *vcpu,
					struct vmcs12 *vmcs12)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);

	if (nested_cpu_has2(vmcs12, SECONDARY_EXEC_VIRTUALIZE_APIC_ACCESSES)) {
		/* TODO: Also verify bits beyond physical address width are 0 */
		if (!PAGE_ALIGNED(vmcs12->apic_access_addr))
			return false;

		/*
		 * Translate L1 physical address to host physical
		 * address for vmcs02. Keep the page pinned, so this
		 * physical address remains valid. We keep a reference
		 * to it so we can release it later.
		 */
		if (vmx->nested.apic_access_page) /* shouldn't happen */
			nested_release_page(vmx->nested.apic_access_page);
		vmx->nested.apic_access_page =
			nested_get_page(vcpu, vmcs12->apic_access_addr);
	}

	if (nested_cpu_has(vmcs12, CPU_BASED_TPR_SHADOW)) {
		/* TODO: Also verify bits beyond physical address width are 0 */
		if (!PAGE_ALIGNED(vmcs12->virtual_apic_page_addr))
			return false;

		if (vmx->nested.virtual_apic_page) /* shouldn't happen */
			nested_release_page(vmx->nested.virtual_apic_page);
		vmx->nested.virtual_apic_page =
			nested_get_page(vcpu, vmcs12->virtual_apic_page_addr);

		/*
		 * Failing the vm entry is _not_ what the processor does
		 * but it's basically the only possibility we have.
		 * We could still enter the guest if CR8 load exits are
		 * enabled, CR8 store exits are enabled, and virtualize APIC
		 * access is disabled; in this case the processor would never
		 * use the TPR shadow and we could simply clear the bit from
		 * the execution control.  But such a configuration is useless,
		 * so let's keep the code simple.
		 */
		if (!vmx->nested.virtual_apic_page)
			return false;
	}

	return true;
}