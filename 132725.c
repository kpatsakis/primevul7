	__acquires(user->release_barrier)
{
	struct ipmi_user *ruser;

	*index = srcu_read_lock(&user->release_barrier);
	ruser = srcu_dereference(user->self, &user->release_barrier);
	if (!ruser)
		srcu_read_unlock(&user->release_barrier, *index);
	return ruser;
}