static void unix_get_secdata(struct scm_cookie *scm, struct sk_buff *skb)
{
	UNIXCB(skb).secid = scm->secid;
}