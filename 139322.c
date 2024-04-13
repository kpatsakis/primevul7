static bool unix_skb_scm_eq(struct sk_buff *skb,
			    struct scm_cookie *scm)
{
	const struct unix_skb_parms *u = &UNIXCB(skb);

	return u->pid == scm->pid &&
	       uid_eq(u->uid, scm->creds.uid) &&
	       gid_eq(u->gid, scm->creds.gid) &&
	       unix_secdata_eq(scm, skb);
}