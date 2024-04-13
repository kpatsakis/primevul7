AppendToRedobuff(char_u *s)
{
    if (!block_redo)
	add_buff(&redobuff, s, -1L);
}