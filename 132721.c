static void free_user_work(struct work_struct *work)
{
	struct ipmi_user *user = container_of(work, struct ipmi_user,
					      remove_work);

	cleanup_srcu_struct(&user->release_barrier);
	kfree(user);
}