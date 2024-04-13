deinit_usm_post_config(int majorid, int minorid, void *serverarg,
		       void *clientarg)
{
    if (usm_free_user(noNameUser) != NULL) {
	DEBUGMSGTL(("deinit_usm_post_config", "could not free initial user\n"));
	return SNMPERR_GENERR;
    }
    noNameUser = NULL;

    DEBUGMSGTL(("deinit_usm_post_config", "initial user removed\n"));
    return SNMPERR_SUCCESS;
}                               /* end deinit_usm_post_config() */