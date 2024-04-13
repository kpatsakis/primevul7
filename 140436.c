void kvm_inject_page_fault(struct kvm_vcpu *vcpu, struct x86_exception *fault)
{
	++vcpu->stat.pf_guest;
	vcpu->arch.cr2 = fault->address;
	kvm_queue_exception_e(vcpu, PF_VECTOR, fault->error_code);
}