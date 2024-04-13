static int unix_socketpair(struct socket *socka, struct socket *sockb)
{
	struct sock *ska = socka->sk, *skb = sockb->sk;

	/* Join our sockets back to back */
	sock_hold(ska);
	sock_hold(skb);
	unix_peer(ska) = skb;
	unix_peer(skb) = ska;
	init_peercred(ska);
	init_peercred(skb);

	if (ska->sk_type != SOCK_DGRAM) {
		ska->sk_state = TCP_ESTABLISHED;
		skb->sk_state = TCP_ESTABLISHED;
		socka->state  = SS_CONNECTED;
		sockb->state  = SS_CONNECTED;
	}
	return 0;
}