static int skcipher_setkey(void *private, const u8 *key, unsigned int keylen)
{
	struct skcipher_tfm *tfm = private;
	int err;

	err = crypto_ablkcipher_setkey(tfm->skcipher, key, keylen);
	tfm->has_key = !err;

	return err;
}