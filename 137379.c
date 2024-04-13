void _gnutls_x509_policies_erase(gnutls_x509_policies_t policies,
				 unsigned int seq)
{
	if (seq >= policies->size)
		return;

	memset(&policies->policy[seq], 0, sizeof(struct gnutls_x509_policy_st));
}