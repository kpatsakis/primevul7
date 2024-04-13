static void kvm_vcpu_ioctl_x86_get_xsave(struct kvm_vcpu *vcpu,
					 struct kvm_xsave *guest_xsave)
{
	if (cpu_has_xsave) {
		memcpy(guest_xsave->region,
			&vcpu->arch.guest_fpu.state->xsave,
			vcpu->arch.guest_xstate_size);
		*(u64 *)&guest_xsave->region[XSAVE_HDR_OFFSET / sizeof(u32)] &=
			vcpu->arch.guest_supported_xcr0 | XSTATE_FPSSE;
	} else {
		memcpy(guest_xsave->region,
			&vcpu->arch.guest_fpu.state->fxsave,
			sizeof(struct i387_fxsave_struct));
		*(u64 *)&guest_xsave->region[XSAVE_HDR_OFFSET / sizeof(u32)] =
			XSTATE_FPSSE;
	}
}