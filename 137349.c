int subject_alt_names_set(struct name_st **names,
			  unsigned int *size,
			  unsigned int san_type,
			  const gnutls_datum_t * san, char *othername_oid)
{
	void *tmp;

	tmp = gnutls_realloc(*names, (*size + 1) * sizeof((*names)[0]));
	if (tmp == NULL) {
		return gnutls_assert_val(GNUTLS_E_MEMORY_ERROR);
	}
	*names = tmp;

	(*names)[*size].type = san_type;
	(*names)[*size].san.data = san->data;
	(*names)[*size].san.size = san->size;

	if (othername_oid) {
		(*names)[*size].othername_oid.data = (uint8_t *) othername_oid;
		(*names)[*size].othername_oid.size = strlen(othername_oid);
	} else {
		(*names)[*size].othername_oid.data = NULL;
		(*names)[*size].othername_oid.size = 0;
	}

	(*size)++;
	return 0;
}