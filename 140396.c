static int apf_put_user(struct kvm_vcpu *vcpu, u32 val)
{

	return kvm_write_guest_cached(vcpu->kvm, &vcpu->arch.apf.data, &val,
				      sizeof(val));
}