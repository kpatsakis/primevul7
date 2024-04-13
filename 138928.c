free_typebuf(void)
{
    if (typebuf.tb_buf == typebuf_init)
	internal_error("Free typebuf 1");
    else
	vim_free(typebuf.tb_buf);
    if (typebuf.tb_noremap == noremapbuf_init)
	internal_error("Free typebuf 2");
    else
	vim_free(typebuf.tb_noremap);
}