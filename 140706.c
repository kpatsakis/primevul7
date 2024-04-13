static struct page *nested_get_page(struct kvm_vcpu *vcpu, gpa_t addr)
{
	struct page *page = gfn_to_page(vcpu->kvm, addr >> PAGE_SHIFT);
	if (is_error_page(page))
		return NULL;

	return page;
}