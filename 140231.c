static inline void vm_exit_controls_init(struct vcpu_vmx *vmx, u32 val)
{
	vmcs_write32(VM_EXIT_CONTROLS, val);
	vmx->vm_exit_controls_shadow = val;
}