int kvm_arch_hardware_setup(void)
{
	return kvm_x86_ops->hardware_setup();
}