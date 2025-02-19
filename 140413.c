static int kvm_is_user_mode(void)
{
	int user_mode = 3;

	if (__this_cpu_read(current_vcpu))
		user_mode = kvm_x86_ops->get_cpl(__this_cpu_read(current_vcpu));

	return user_mode != 0;
}