keytype(
	struct parse *pcmd,
	FILE *fp
	)
{
	const char *	digest_name;
	size_t		digest_len;
	int		key_type;

	if (!pcmd->nargs) {
		fprintf(fp, "keytype is %s with %lu octet digests\n",
			keytype_name(info_auth_keytype),
			(u_long)info_auth_hashlen);
		return;
	}

	digest_name = pcmd->argval[0].string;
	digest_len = 0;
	key_type = keytype_from_text(digest_name, &digest_len);

	if (!key_type) {
		fprintf(fp, "keytype is not valid. "
#ifdef OPENSSL
			"Type \"help keytype\" for the available digest types.\n");
#else
			"Only \"md5\" is available.\n");
#endif
		return;
	}

	info_auth_keytype = key_type;
	info_auth_hashlen = digest_len;
}