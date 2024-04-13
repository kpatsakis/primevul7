realloc_post_list(void)
{
    int   nstate_max = (int)(post_end - post_start);
    int   new_max;
    int   *new_start;
    int	  *old_start;

    // For weird patterns the number of states can be very high. Increasing by
    // 50% seems a reasonable compromise between memory use and speed.
    new_max = nstate_max * 3 / 2;
    new_start = ALLOC_MULT(int, new_max);
    if (new_start == NULL)
	return FAIL;
    mch_memmove(new_start, post_start, nstate_max * sizeof(int));
    old_start = post_start;
    post_start = new_start;
    post_ptr = new_start + (post_ptr - old_start);
    post_end = post_start + new_max;
    vim_free(old_start);
    return OK;
}