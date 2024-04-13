int gnutls_x509_policies_set(gnutls_x509_policies_t policies,
			     const struct gnutls_x509_policy_st *policy)
{
	unsigned i;

	if (policies->size + 1 > MAX_ENTRIES)
		return gnutls_assert_val(GNUTLS_E_INVALID_REQUEST);

	policies->policy[policies->size].oid = gnutls_strdup(policy->oid);
	if (policies->policy[policies->size].oid == NULL)
		return gnutls_assert_val(GNUTLS_E_MEMORY_ERROR);

	for (i = 0; i < policy->qualifiers; i++) {
		policies->policy[policies->size].qualifier[i].type =
		    policy->qualifier[i].type;
		policies->policy[policies->size].qualifier[i].size =
		    policy->qualifier[i].size;
		policies->policy[policies->size].qualifier[i].data =
		    gnutls_malloc(policy->qualifier[i].size + 1);
		if (policies->policy[policies->size].qualifier[i].data == NULL)
			return gnutls_assert_val(GNUTLS_E_MEMORY_ERROR);
		memcpy(policies->policy[policies->size].qualifier[i].data,
		       policy->qualifier[i].data, policy->qualifier[i].size);
		policies->policy[policies->size].qualifier[i].data[policy->
								   qualifier[i].
								   size] = 0;
	}

	policies->policy[policies->size].qualifiers = policy->qualifiers;
	policies->size++;

	return 0;
}