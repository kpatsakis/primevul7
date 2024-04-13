static void ip_ra_destroy_rcu(struct rcu_head *head)
{
	struct ip_ra_chain *ra = container_of(head, struct ip_ra_chain, rcu);

	sock_put(ra->saved_sk);
	kfree(ra);
}