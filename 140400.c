static int xen_hvm_config(struct kvm_vcpu *vcpu, u64 data)
{
	struct kvm *kvm = vcpu->kvm;
	int lm = is_long_mode(vcpu);
	u8 *blob_addr = lm ? (u8 *)(long)kvm->arch.xen_hvm_config.blob_addr_64
		: (u8 *)(long)kvm->arch.xen_hvm_config.blob_addr_32;
	u8 blob_size = lm ? kvm->arch.xen_hvm_config.blob_size_64
		: kvm->arch.xen_hvm_config.blob_size_32;
	u32 page_num = data & ~PAGE_MASK;
	u64 page_addr = data & PAGE_MASK;
	u8 *page;
	int r;

	r = -E2BIG;
	if (page_num >= blob_size)
		goto out;
	r = -ENOMEM;
	page = memdup_user(blob_addr + (page_num * PAGE_SIZE), PAGE_SIZE);
	if (IS_ERR(page)) {
		r = PTR_ERR(page);
		goto out;
	}
	if (kvm_write_guest(kvm, page_addr, page, PAGE_SIZE))
		goto out_free;
	r = 0;
out_free:
	kfree(page);
out:
	return r;
}