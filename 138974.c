AppendNumberToRedobuff(long n)
{
    if (!block_redo)
	add_num_buff(&redobuff, n);
}