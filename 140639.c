static inline void vm_entry_controls_init(struct vcpu_vmx *vmx, u32 val)
{
	vmcs_write32(VM_ENTRY_CONTROLS, val);
	vmx->vm_entry_controls_shadow = val;
}