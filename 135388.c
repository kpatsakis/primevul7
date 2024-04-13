static inline bool vt_in_use(unsigned int i)
{
	const struct vc_data *vc = vc_cons[i].d;

	/*
	 * console_lock must be held to prevent the vc from being deallocated
	 * while we're checking whether it's in-use.
	 */
	WARN_CONSOLE_UNLOCKED();

	return vc && kref_read(&vc->port.kref) > 1;
}