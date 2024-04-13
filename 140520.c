static void nested_free_vmcs02(struct vcpu_vmx *vmx, gpa_t vmptr)
{
	struct vmcs02_list *item;
	list_for_each_entry(item, &vmx->nested.vmcs02_pool, list)
		if (item->vmptr == vmptr) {
			free_loaded_vmcs(&item->vmcs02);
			list_del(&item->list);
			kfree(item);
			vmx->nested.vmcs02_num--;
			return;
		}
}