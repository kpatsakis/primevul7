static int build_expire(struct sk_buff *skb, struct xfrm_state *x, const struct km_event *c)
{
	struct xfrm_user_expire *ue;
	struct nlmsghdr *nlh;
	int err;

	nlh = nlmsg_put(skb, c->portid, 0, XFRM_MSG_EXPIRE, sizeof(*ue), 0);
	if (nlh == NULL)
		return -EMSGSIZE;

	ue = nlmsg_data(nlh);
	copy_to_user_state(x, &ue->state);
	ue->hard = (c->data.hard != 0) ? 1 : 0;
	/* clear the padding bytes */
	memset(&ue->hard + 1, 0, sizeof(*ue) - offsetofend(typeof(*ue), hard));

	err = xfrm_mark_put(skb, &x->mark);
	if (err)
		return err;

	err = xfrm_if_id_put(skb, x->if_id);
	if (err)
		return err;

	nlmsg_end(skb, nlh);
	return 0;
}