static int handle_monitor(struct kvm_vcpu *vcpu)
{
	printk_once(KERN_WARNING "kvm: MONITOR instruction emulated as NOP!\n");
	return handle_nop(vcpu);
}