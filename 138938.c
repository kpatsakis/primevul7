may_sync_undo(void)
{
    if ((!(State & (INSERT + CMDLINE)) || arrow_used)
					       && scriptin[curscript] == NULL)
	u_sync(FALSE);
}