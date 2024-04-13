before_blocking(void)
{
    updatescript(0);
#ifdef FEAT_EVAL
    if (may_garbage_collect)
	garbage_collect(FALSE);
#endif
}