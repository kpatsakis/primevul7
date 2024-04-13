static void *unix_seq_next(struct seq_file *seq, void *v, loff_t *pos)
{
	++*pos;
	return unix_next_socket(seq, v, pos);
}