input_available(void)
{
    return (!vim_is_input_buf_empty()
# if defined(FEAT_CLIENTSERVER) || defined(FEAT_EVAL)
	    || typebuf_was_filled
# endif
	    );
}