int kvm_set_dr(struct kvm_vcpu *vcpu, int dr, unsigned long val)
{
	int res;

	res = __kvm_set_dr(vcpu, dr, val);
	if (res > 0)
		kvm_queue_exception(vcpu, UD_VECTOR);
	else if (res < 0)
		kvm_inject_gp(vcpu, 0);

	return res;
}