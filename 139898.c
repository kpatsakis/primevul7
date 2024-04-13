static void cleanup_return_instances(struct uprobe_task *utask, bool chained,
					struct pt_regs *regs)
{
	struct return_instance *ri = utask->return_instances;
	enum rp_check ctx = chained ? RP_CHECK_CHAIN_CALL : RP_CHECK_CALL;

	while (ri && !arch_uretprobe_is_alive(ri, ctx, regs)) {
		ri = free_ret_instance(ri);
		utask->depth--;
	}
	utask->return_instances = ri;
}