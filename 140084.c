static int kvm_read_guest_virt_system(struct x86_emulate_ctxt *ctxt,
				      gva_t addr, void *val, unsigned int bytes,
				      struct x86_exception *exception)
{
	struct kvm_vcpu *vcpu = emul_to_vcpu(ctxt);
	return kvm_read_guest_virt_helper(addr, val, bytes, vcpu, 0, exception);
}