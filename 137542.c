int compute_server_confirm (pwd_session_t *session, uint8_t *out, BN_CTX *bnctx)
{
	BIGNUM *x = NULL, *y = NULL;
	HMAC_CTX *ctx = NULL;
	uint8_t *cruft = NULL;
	int offset, req = -1;

	ctx = HMAC_CTX_new();
	if (ctx == NULL) {
		DEBUG2("pwd: unable to allocate HMAC context!");
		goto finish;
	}

	/*
	 * Each component of the cruft will be at most as big as the prime
	 */
	if (((cruft = talloc_zero_array(session, uint8_t, BN_num_bytes(session->prime))) == NULL) ||
	    ((x = BN_new()) == NULL) || ((y = BN_new()) == NULL)) {
		DEBUG2("pwd: unable to allocate space to compute confirm!");
		goto finish;
	}

	/*
	 * commit is H(k | server_element | server_scalar | peer_element |
	 *	       peer_scalar | ciphersuite)
	 */
	H_Init(ctx);

	/*
	 * Zero the memory each time because this is mod prime math and some
	 * value may start with a few zeros and the previous one did not.
	 *
	 * First is k
	 */
	offset = BN_num_bytes(session->prime) - BN_num_bytes(session->k);
	BN_bn2bin(session->k, cruft + offset);
	H_Update(ctx, cruft, BN_num_bytes(session->prime));

	/*
	 * next is server element: x, y
	 */
	if (!EC_POINT_get_affine_coordinates_GFp(session->group, session->my_element, x, y, bnctx)) {
		DEBUG2("pwd: unable to get coordinates of server element");
		goto finish;
	}
	memset(cruft, 0, BN_num_bytes(session->prime));
	offset = BN_num_bytes(session->prime) - BN_num_bytes(x);
	BN_bn2bin(x, cruft + offset);
	H_Update(ctx, cruft, BN_num_bytes(session->prime));

	memset(cruft, 0, BN_num_bytes(session->prime));
	offset = BN_num_bytes(session->prime) - BN_num_bytes(y);
	BN_bn2bin(y, cruft + offset);
	H_Update(ctx, cruft, BN_num_bytes(session->prime));

	/*
	 * and server scalar
	 */
	memset(cruft, 0, BN_num_bytes(session->prime));
	offset = BN_num_bytes(session->order) - BN_num_bytes(session->my_scalar);
	BN_bn2bin(session->my_scalar, cruft + offset);
	H_Update(ctx, cruft, BN_num_bytes(session->order));

	/*
	 * next is peer element: x, y
	 */
	if (!EC_POINT_get_affine_coordinates_GFp(session->group, session->peer_element, x, y, bnctx)) {
		DEBUG2("pwd: unable to get coordinates of peer's element");
		goto finish;
	}

	memset(cruft, 0, BN_num_bytes(session->prime));
	offset = BN_num_bytes(session->prime) - BN_num_bytes(x);
	BN_bn2bin(x, cruft + offset);
	H_Update(ctx, cruft, BN_num_bytes(session->prime));

	memset(cruft, 0, BN_num_bytes(session->prime));
	offset = BN_num_bytes(session->prime) - BN_num_bytes(y);
	BN_bn2bin(y, cruft + offset);
	H_Update(ctx, cruft, BN_num_bytes(session->prime));

	/*
	 * and peer scalar
	 */
	memset(cruft, 0, BN_num_bytes(session->prime));
	offset = BN_num_bytes(session->order) - BN_num_bytes(session->peer_scalar);
	BN_bn2bin(session->peer_scalar, cruft + offset);
	H_Update(ctx, cruft, BN_num_bytes(session->order));

	/*
	 * finally, ciphersuite
	 */
	H_Update(ctx, (uint8_t *)&session->ciphersuite, sizeof(session->ciphersuite));

	H_Final(ctx, out);

	req = 0;
finish:
	talloc_free(cruft);
	BN_free(x);
	BN_free(y);
	HMAC_CTX_free(ctx);

	return req;
}