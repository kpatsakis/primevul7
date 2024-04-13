zsetpagedevice(i_ctx_t *i_ctx_p)
{
    os_ptr op = osp;
    int code;

/******
    if ( igs->in_cachedevice )
        return_error(gs_error_undefined);
 ******/
    if (r_has_type(op, t_dictionary)) {
        check_dict_read(*op);
#if 0	/****************/
        /*
         * In order to avoid invalidaccess errors on setpagedevice,
         * the dictionary must be allocated in local VM.
         */
        if (!(r_is_local(op)))
            return_error(gs_error_invalidaccess);
#endif	/****************/
        /* Make the dictionary read-only. */
        code = zreadonly(i_ctx_p);
        if (code < 0)
            return code;
    } else {
        check_type(*op, t_null);
    }
    istate->pagedevice = *op;
    pop(1);
    return 0;
}