static void free_nested(struct vcpu_vmx *vmx)
{
	if (!vmx->nested.vmxon)
		return;

	vmx->nested.vmxon = false;
	nested_release_vmcs12(vmx);
	if (enable_shadow_vmcs)
		free_vmcs(vmx->nested.current_shadow_vmcs);
	/* Unpin physical memory we referred to in current vmcs02 */
	if (vmx->nested.apic_access_page) {
		nested_release_page(vmx->nested.apic_access_page);
		vmx->nested.apic_access_page = NULL;
	}
	if (vmx->nested.virtual_apic_page) {
		nested_release_page(vmx->nested.virtual_apic_page);
		vmx->nested.virtual_apic_page = NULL;
	}

	nested_free_all_saved_vmcss(vmx);
}