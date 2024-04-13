static inline u32 vm_exit_controls_get(struct vcpu_vmx *vmx)
{
	return vmx->vm_exit_controls_shadow;
}