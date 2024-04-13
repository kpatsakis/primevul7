static int xfrm_do_migrate(struct sk_buff *skb, struct nlmsghdr *nlh,
			   struct nlattr **attrs)
{
	struct xfrm_userpolicy_id *pi = nlmsg_data(nlh);
	struct xfrm_migrate m[XFRM_MAX_DEPTH];
	struct xfrm_kmaddress km, *kmp;
	u8 type;
	int err;
	int n = 0;
	struct net *net = sock_net(skb->sk);
	struct xfrm_encap_tmpl  *encap = NULL;

	if (attrs[XFRMA_MIGRATE] == NULL)
		return -EINVAL;

	kmp = attrs[XFRMA_KMADDRESS] ? &km : NULL;

	err = copy_from_user_policy_type(&type, attrs);
	if (err)
		return err;

	err = copy_from_user_migrate((struct xfrm_migrate *)m, kmp, attrs, &n);
	if (err)
		return err;

	if (!n)
		return 0;

	if (attrs[XFRMA_ENCAP]) {
		encap = kmemdup(nla_data(attrs[XFRMA_ENCAP]),
				sizeof(*encap), GFP_KERNEL);
		if (!encap)
			return 0;
	}

	err = xfrm_migrate(&pi->sel, pi->dir, type, m, n, kmp, net, encap);

	kfree(encap);

	return err;
}