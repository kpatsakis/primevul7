ResetRedobuff(void)
{
    if (!block_redo)
    {
	free_buff(&old_redobuff);
	old_redobuff = redobuff;
	redobuff.bh_first.b_next = NULL;
    }
}