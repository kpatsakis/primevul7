static bool fanout_has_flag(struct packet_fanout *f, u16 flag)
{
	return f->flags & (flag >> 8);
}