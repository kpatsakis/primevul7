int kvm_get_dr(struct kvm_vcpu *vcpu, int dr, unsigned long *val)
{
	if (_kvm_get_dr(vcpu, dr, val)) {
		kvm_queue_exception(vcpu, UD_VECTOR);
		return 1;
	}
	return 0;
}