static int handle_mwait(struct kvm_vcpu *vcpu)
{
	printk_once(KERN_WARNING "kvm: MWAIT instruction emulated as NOP!\n");
	return handle_nop(vcpu);
}