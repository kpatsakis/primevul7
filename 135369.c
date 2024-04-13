static inline bool vt_busy(int i)
{
	if (vt_in_use(i))
		return true;
	if (i == fg_console)
		return true;
	if (vc_is_sel(vc_cons[i].d))
		return true;

	return false;
}