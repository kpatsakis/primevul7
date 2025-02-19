static int _kvm_get_dr(struct kvm_vcpu *vcpu, int dr, unsigned long *val)
{
	switch (dr) {
	case 0 ... 3:
		*val = vcpu->arch.db[dr];
		break;
	case 4:
		if (kvm_read_cr4_bits(vcpu, X86_CR4_DE))
			return 1;
		/* fall through */
	case 6:
		*val = vcpu->arch.dr6;
		break;
	case 5:
		if (kvm_read_cr4_bits(vcpu, X86_CR4_DE))
			return 1;
		/* fall through */
	default: /* 7 */
		*val = vcpu->arch.dr7;
		break;
	}

	return 0;
}