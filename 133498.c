static inline void dev_base_seq_inc(struct net *net)
{
	while (++net->dev_base_seq == 0)
		;
}