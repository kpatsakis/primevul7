static inline void vm_exit_controls_setbit(struct vcpu_vmx *vmx, u32 val)
{
	vm_exit_controls_set(vmx, vm_exit_controls_get(vmx) | val);
}