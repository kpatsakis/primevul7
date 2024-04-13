static void free_user(struct kref *ref)
{
	struct ipmi_user *user = container_of(ref, struct ipmi_user, refcount);

	/* SRCU cleanup must happen in task context. */
	schedule_work(&user->remove_work);
}