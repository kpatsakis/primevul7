static int vmx_vm_has_apicv(struct kvm *kvm)
{
	return enable_apicv && irqchip_in_kernel(kvm);
}