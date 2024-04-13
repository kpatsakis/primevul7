static int read_emulate(struct kvm_vcpu *vcpu, gpa_t gpa,
			void *val, int bytes)
{
	return !kvm_read_guest(vcpu->kvm, gpa, val, bytes);
}