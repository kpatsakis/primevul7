static void release_ipmi_user(struct ipmi_user *user, int index)
{
	srcu_read_unlock(&user->release_barrier, index);
}