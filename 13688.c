static int skcipher_accept_parent(void *private, struct sock *sk)
{
	struct skcipher_tfm *tfm = private;

	if (!tfm->has_key)
		return -ENOKEY;

	return skcipher_accept_parent_nokey(private, sk);
}