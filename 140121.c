static inline void vm_entry_controls_setbit(struct vcpu_vmx *vmx, u32 val)
{
	vm_entry_controls_set(vmx, vm_entry_controls_get(vmx) | val);
}