static void kvm_vcpu_ioctl_x86_get_debugregs(struct kvm_vcpu *vcpu,
					     struct kvm_debugregs *dbgregs)
{
	unsigned long val;

	memcpy(dbgregs->db, vcpu->arch.db, sizeof(vcpu->arch.db));
	_kvm_get_dr(vcpu, 6, &val);
	dbgregs->dr6 = val;
	dbgregs->dr7 = vcpu->arch.dr7;
	dbgregs->flags = 0;
	memset(&dbgregs->reserved, 0, sizeof(dbgregs->reserved));
}