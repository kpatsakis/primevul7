static int handle_tpr_below_threshold(struct kvm_vcpu *vcpu)
{
	kvm_make_request(KVM_REQ_EVENT, vcpu);
	return 1;
}