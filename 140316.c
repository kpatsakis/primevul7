void kvm_arch_async_page_present(struct kvm_vcpu *vcpu,
				 struct kvm_async_pf *work)
{
	struct x86_exception fault;

	trace_kvm_async_pf_ready(work->arch.token, work->gva);
	if (work->wakeup_all)
		work->arch.token = ~0; /* broadcast wakeup */
	else
		kvm_del_async_pf_gfn(vcpu, work->arch.gfn);

	if ((vcpu->arch.apf.msr_val & KVM_ASYNC_PF_ENABLED) &&
	    !apf_put_user(vcpu, KVM_PV_REASON_PAGE_READY)) {
		fault.vector = PF_VECTOR;
		fault.error_code_valid = true;
		fault.error_code = 0;
		fault.nested_page_fault = false;
		fault.address = work->arch.token;
		kvm_inject_page_fault(vcpu, &fault);
	}
	vcpu->arch.apf.halted = false;
	vcpu->arch.mp_state = KVM_MP_STATE_RUNNABLE;
}