static int copy_sec_ctx(struct xfrm_sec_ctx *s, struct sk_buff *skb)
{
	struct xfrm_user_sec_ctx *uctx;
	struct nlattr *attr;
	int ctx_size = sizeof(*uctx) + s->ctx_len;

	attr = nla_reserve(skb, XFRMA_SEC_CTX, ctx_size);
	if (attr == NULL)
		return -EMSGSIZE;

	uctx = nla_data(attr);
	uctx->exttype = XFRMA_SEC_CTX;
	uctx->len = ctx_size;
	uctx->ctx_doi = s->ctx_doi;
	uctx->ctx_alg = s->ctx_alg;
	uctx->ctx_len = s->ctx_len;
	memcpy(uctx + 1, s->ctx_str, s->ctx_len);

	return 0;
}