csnPretty(
	Syntax *syntax,
	struct berval *val,
	struct berval *out,
	void *ctx )
{
	return csnNormalize( SLAP_MR_VALUE_OF_SYNTAX, NULL, NULL, val, out, ctx );
}