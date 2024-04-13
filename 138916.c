validate_maphash(void)
{
    if (!maphash_valid)
    {
	vim_memset(maphash, 0, sizeof(maphash));
	maphash_valid = TRUE;
    }
}