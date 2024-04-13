static inline void vm_entry_controls_set(struct vcpu_vmx *vmx, u32 val)
{
	if (vmx->vm_entry_controls_shadow != val)
		vm_entry_controls_init(vmx, val);
}