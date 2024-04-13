handle_uretprobe_chain(struct return_instance *ri, struct pt_regs *regs)
{
	struct uprobe *uprobe = ri->uprobe;
	struct uprobe_consumer *uc;

	down_read(&uprobe->register_rwsem);
	for (uc = uprobe->consumers; uc; uc = uc->next) {
		if (uc->ret_handler)
			uc->ret_handler(uc, ri->func, regs);
	}
	up_read(&uprobe->register_rwsem);
}