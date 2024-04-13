dsa_verify_sig(const gnutls_datum_t * text,
	       const gnutls_datum_t * signature, mpi_t * params,
	       int params_len)
{
    int ret;
    opaque _digest[MAX_HASH_SIZE];
    gnutls_datum_t digest;
    GNUTLS_HASH_HANDLE hd;

    hd = _gnutls_hash_init(GNUTLS_MAC_SHA1);
    if (hd == NULL) {
	gnutls_assert();
	return GNUTLS_E_HASH_FAILED;
    }

    _gnutls_hash(hd, text->data, text->size);
    _gnutls_hash_deinit(hd, _digest);

    digest.data = _digest;
    digest.size = 20;

    ret = _gnutls_dsa_verify(&digest, signature, params, params_len);

    return ret;
}