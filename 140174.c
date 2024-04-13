static inline void vm_exit_controls_set(struct vcpu_vmx *vmx, u32 val)
{
	if (vmx->vm_exit_controls_shadow != val)
		vm_exit_controls_init(vmx, val);
}