static int xfrm_alloc_userspi(struct sk_buff *skb, struct nlmsghdr *nlh,
		struct nlattr **attrs)
{
	struct net *net = sock_net(skb->sk);
	struct xfrm_state *x;
	struct xfrm_userspi_info *p;
	struct sk_buff *resp_skb;
	xfrm_address_t *daddr;
	int family;
	int err;
	u32 mark;
	struct xfrm_mark m;
	u32 if_id = 0;

	p = nlmsg_data(nlh);
	err = verify_spi_info(p->info.id.proto, p->min, p->max);
	if (err)
		goto out_noput;

	family = p->info.family;
	daddr = &p->info.id.daddr;

	x = NULL;

	mark = xfrm_mark_get(attrs, &m);

	if (attrs[XFRMA_IF_ID])
		if_id = nla_get_u32(attrs[XFRMA_IF_ID]);

	if (p->info.seq) {
		x = xfrm_find_acq_byseq(net, mark, p->info.seq);
		if (x && !xfrm_addr_equal(&x->id.daddr, daddr, family)) {
			xfrm_state_put(x);
			x = NULL;
		}
	}

	if (!x)
		x = xfrm_find_acq(net, &m, p->info.mode, p->info.reqid,
				  if_id, p->info.id.proto, daddr,
				  &p->info.saddr, 1,
				  family);
	err = -ENOENT;
	if (x == NULL)
		goto out_noput;

	err = xfrm_alloc_spi(x, p->min, p->max);
	if (err)
		goto out;

	resp_skb = xfrm_state_netlink(skb, x, nlh->nlmsg_seq);
	if (IS_ERR(resp_skb)) {
		err = PTR_ERR(resp_skb);
		goto out;
	}

	err = nlmsg_unicast(net->xfrm.nlsk, resp_skb, NETLINK_CB(skb).portid);

out:
	xfrm_state_put(x);
out_noput:
	return err;
}