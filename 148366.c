redirect_tg4(struct sk_buff *skb, const struct xt_action_param *par)
{
	return nf_nat_redirect_ipv4(skb, par->targinfo, par->hooknum);
}