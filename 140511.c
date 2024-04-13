static inline u32 vm_entry_controls_get(struct vcpu_vmx *vmx)
{
	return vmx->vm_entry_controls_shadow;
}