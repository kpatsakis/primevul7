hashIter(
	HASH_CONTEXT *HASHcontext,
	unsigned char *HASHdigest,
	unsigned char *value,
	int len)
{
	HASH_CONTEXT ctx = *HASHcontext;
	HASH_Update( &ctx, value, len );
	HASH_Final( HASHdigest, &ctx );
}