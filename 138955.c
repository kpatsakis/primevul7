init_typebuf(void)
{
    if (typebuf.tb_buf == NULL)
    {
	typebuf.tb_buf = typebuf_init;
	typebuf.tb_noremap = noremapbuf_init;
	typebuf.tb_buflen = TYPELEN_INIT;
	typebuf.tb_len = 0;
	typebuf.tb_off = MAXMAPLEN + 4;
	typebuf.tb_change_cnt = 1;
    }
}