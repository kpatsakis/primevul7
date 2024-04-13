void gnutls_x509_policies_deinit(gnutls_x509_policies_t policies)
{
	unsigned i;

	for (i = 0; i < policies->size; i++)
		gnutls_x509_policy_release(&policies->policy[i]);
	gnutls_free(policies);
}