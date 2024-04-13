static inline u8 *esp_tmp_iv(struct crypto_aead *aead, void *tmp, int seqhilen)
{
	return crypto_aead_ivsize(aead) ?
	       PTR_ALIGN((u8 *)tmp + seqhilen,
			 crypto_aead_alignmask(aead) + 1) : tmp + seqhilen;
}