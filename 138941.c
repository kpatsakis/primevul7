ins_char_typebuf(int c)
{
    char_u	buf[MB_MAXBYTES + 1];
    if (IS_SPECIAL(c))
    {
	buf[0] = K_SPECIAL;
	buf[1] = K_SECOND(c);
	buf[2] = K_THIRD(c);
	buf[3] = NUL;
    }
    else
	buf[(*mb_char2bytes)(c, buf)] = NUL;
    (void)ins_typebuf(buf, KeyNoremap, 0, !KeyTyped, cmd_silent);
}