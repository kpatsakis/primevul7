struct sock *do_msg_redirect_map(struct sk_msg_buff *msg)
{
	struct sock *sk = NULL;

	if (msg->map) {
		sk = __sock_map_lookup_elem(msg->map, msg->key);

		msg->key = 0;
		msg->map = NULL;
	}

	return sk;
}