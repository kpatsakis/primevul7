AppendCharToRedobuff(int c)
{
    if (!block_redo)
	add_char_buff(&redobuff, c);
}