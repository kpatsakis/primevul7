int gnutls_x509_policies_get(gnutls_x509_policies_t policies,
			     unsigned int seq,
			     struct gnutls_x509_policy_st *policy)
{
	if (seq >= policies->size)
		return gnutls_assert_val(GNUTLS_E_REQUESTED_DATA_NOT_AVAILABLE);

	if (policy) {
		memcpy(policy, &policies->policy[seq],
		       sizeof(struct gnutls_x509_policy_st));
	}

	return 0;
}