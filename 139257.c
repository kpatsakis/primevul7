static inline unsigned int unix_hash_fold(__wsum n)
{
	unsigned int hash = (__force unsigned int)csum_fold(n);

	hash ^= hash>>8;
	return hash&(UNIX_HASH_SIZE-1);
}