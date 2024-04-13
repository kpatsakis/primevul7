hashPreset(
	HASH_CONTEXT *HASHcontext,
	struct berval *prefix,
	char pre,
	Syntax *syntax,
	MatchingRule *mr)
{
	HASH_Init(HASHcontext);
	if(prefix && prefix->bv_len > 0) {
		HASH_Update(HASHcontext,
			(unsigned char *)prefix->bv_val, prefix->bv_len);
	}
	if(pre) HASH_Update(HASHcontext, (unsigned char*)&pre, sizeof(pre));
	HASH_Update(HASHcontext, (unsigned char*)syntax->ssyn_oid, syntax->ssyn_oidlen);
	HASH_Update(HASHcontext, (unsigned char*)mr->smr_oid, mr->smr_oidlen);
	return;
}