static struct loaded_vmcs *nested_get_current_vmcs02(struct vcpu_vmx *vmx)
{
	struct vmcs02_list *item;
	list_for_each_entry(item, &vmx->nested.vmcs02_pool, list)
		if (item->vmptr == vmx->nested.current_vmptr) {
			list_move(&item->list, &vmx->nested.vmcs02_pool);
			return &item->vmcs02;
		}

	if (vmx->nested.vmcs02_num >= max(VMCS02_POOL_SIZE, 1)) {
		/* Recycle the least recently used VMCS. */
		item = list_entry(vmx->nested.vmcs02_pool.prev,
			struct vmcs02_list, list);
		item->vmptr = vmx->nested.current_vmptr;
		list_move(&item->list, &vmx->nested.vmcs02_pool);
		return &item->vmcs02;
	}

	/* Create a new VMCS */
	item = kmalloc(sizeof(struct vmcs02_list), GFP_KERNEL);
	if (!item)
		return NULL;
	item->vmcs02.vmcs = alloc_vmcs();
	if (!item->vmcs02.vmcs) {
		kfree(item);
		return NULL;
	}
	loaded_vmcs_init(&item->vmcs02);
	item->vmptr = vmx->nested.current_vmptr;
	list_add(&(item->list), &(vmx->nested.vmcs02_pool));
	vmx->nested.vmcs02_num++;
	return &item->vmcs02;
}