bool __weak arch_uretprobe_is_alive(struct return_instance *ret, enum rp_check ctx,
					struct pt_regs *regs)
{
	return true;
}