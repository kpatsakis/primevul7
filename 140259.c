static u32 vmx_pin_based_exec_ctrl(struct vcpu_vmx *vmx)
{
	u32 pin_based_exec_ctrl = vmcs_config.pin_based_exec_ctrl;

	if (!vmx_vm_has_apicv(vmx->vcpu.kvm))
		pin_based_exec_ctrl &= ~PIN_BASED_POSTED_INTR;
	return pin_based_exec_ctrl;
}