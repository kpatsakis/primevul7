int process_peer_commit (pwd_session_t *session, uint8_t *in, size_t in_len, BN_CTX *bnctx)
{
	uint8_t *ptr;
	size_t data_len;
	BIGNUM *x = NULL, *y = NULL, *cofactor = NULL;
	EC_POINT *K = NULL, *point = NULL;
	int res = 1;

	if (((session->peer_scalar = BN_new()) == NULL) ||
	    ((session->k = BN_new()) == NULL) ||
	    ((cofactor = BN_new()) == NULL) ||
	    ((x = BN_new()) == NULL) ||
	    ((y = BN_new()) == NULL) ||
	    ((point = EC_POINT_new(session->group)) == NULL) ||
	    ((K = EC_POINT_new(session->group)) == NULL) ||
	    ((session->peer_element = EC_POINT_new(session->group)) == NULL)) {
		DEBUG2("pwd: failed to allocate room to process peer's commit");
		goto finish;
	}

	if (!EC_GROUP_get_cofactor(session->group, cofactor, NULL)) {
		DEBUG2("pwd: unable to get group co-factor");
		goto finish;
	}

	/* element, x then y, followed by scalar */
	ptr = (uint8_t *)in;
	data_len = BN_num_bytes(session->prime);

	/*
	 *	Did the peer send enough data?
	 */
	if (in_len < (2 * data_len + BN_num_bytes(session->order))) {
		DEBUG("pwd: Invalid commit packet");
		goto finish;
	}

	BN_bin2bn(ptr, data_len, x);
	ptr += data_len;
	BN_bin2bn(ptr, data_len, y);
	ptr += data_len;

	data_len = BN_num_bytes(session->order);
	BN_bin2bn(ptr, data_len, session->peer_scalar);

	/* validate received scalar */
	if (BN_is_zero(session->peer_scalar) ||
	    BN_is_one(session->peer_scalar) ||
	    BN_cmp(session->peer_scalar, session->order) >= 0) {
		ERROR("Peer's scalar is not within the allowed range");
		goto finish;
	}

	if (!EC_POINT_set_affine_coordinates_GFp(session->group, session->peer_element, x, y, bnctx)) {
		DEBUG2("pwd: unable to get coordinates of peer's element");
		goto finish;
	}

	/* validate received element */
	if (!EC_POINT_is_on_curve(session->group, session->peer_element, bnctx) ||
	    EC_POINT_is_at_infinity(session->group, session->peer_element)) {
		ERROR("Peer's element is not a point on the elliptic curve");
		goto finish;
	}

	/* check to ensure peer's element is not in a small sub-group */
	if (BN_cmp(cofactor, BN_value_one())) {
		if (!EC_POINT_mul(session->group, point, NULL, session->peer_element, cofactor, NULL)) {
			DEBUG2("pwd: unable to multiply element by co-factor");
			goto finish;
		}

		if (EC_POINT_is_at_infinity(session->group, point)) {
			DEBUG2("pwd: peer's element is in small sub-group");
			goto finish;
		}
	}

	/* detect reflection attacks */
	if (BN_cmp(session->peer_scalar, session->my_scalar) == 0 ||
	    EC_POINT_cmp(session->group, session->peer_element, session->my_element, bnctx) == 0) {
		ERROR("Reflection attack detected");
		goto finish;
	}

	/* compute the shared key, k */
	if ((!EC_POINT_mul(session->group, K, NULL, session->pwe, session->peer_scalar, bnctx)) ||
	    (!EC_POINT_add(session->group, K, K, session->peer_element, bnctx)) ||
	    (!EC_POINT_mul(session->group, K, NULL, K, session->private_value, bnctx))) {
		DEBUG2("pwd: unable to compute shared key, k");
		goto finish;
	}

	/* ensure that the shared key isn't in a small sub-group */
	if (BN_cmp(cofactor, BN_value_one())) {
		if (!EC_POINT_mul(session->group, K, NULL, K, cofactor, NULL)) {
			DEBUG2("pwd: unable to multiply k by co-factor");
			goto finish;
		}
	}

	/*
	 * This check is strictly speaking just for the case above where
	 * co-factor > 1 but it was suggested that even though this is probably
	 * never going to happen it is a simple and safe check "just to be
	 * sure" so let's be safe.
	 */
	if (EC_POINT_is_at_infinity(session->group, K)) {
		DEBUG2("pwd: k is point-at-infinity!");
		goto finish;
	}

	if (!EC_POINT_get_affine_coordinates_GFp(session->group, K, session->k, NULL, bnctx)) {
		DEBUG2("pwd: unable to get shared secret from K");
		goto finish;
	}
	res = 0;

finish:
	EC_POINT_clear_free(K);
	EC_POINT_clear_free(point);
	BN_clear_free(cofactor);
	BN_clear_free(x);
	BN_clear_free(y);

	return res;
}