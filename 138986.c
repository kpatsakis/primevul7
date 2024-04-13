CancelRedo(void)
{
    if (!block_redo)
    {
	free_buff(&redobuff);
	redobuff = old_redobuff;
	old_redobuff.bh_first.b_next = NULL;
	start_stuff();
	while (read_readbuffers(TRUE) != NUL)
	    ;
    }
}