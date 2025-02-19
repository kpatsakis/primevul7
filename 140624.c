void kvm_arch_async_page_not_present(struct kvm_vcpu *vcpu,
				     struct kvm_async_pf *work)
{
	struct x86_exception fault;

	trace_kvm_async_pf_not_present(work->arch.token, work->gva);
	kvm_add_async_pf_gfn(vcpu, work->arch.gfn);

	if (!(vcpu->arch.apf.msr_val & KVM_ASYNC_PF_ENABLED) ||
	    (vcpu->arch.apf.send_user_only &&
	     kvm_x86_ops->get_cpl(vcpu) == 0))
		kvm_make_request(KVM_REQ_APF_HALT, vcpu);
	else if (!apf_put_user(vcpu, KVM_PV_REASON_PAGE_NOT_PRESENT)) {
		fault.vector = PF_VECTOR;
		fault.error_code_valid = true;
		fault.error_code = 0;
		fault.nested_page_fault = false;
		fault.address = work->arch.token;
		kvm_inject_page_fault(vcpu, &fault);
	}
}