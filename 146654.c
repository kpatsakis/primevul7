int dane_query_data(dane_query_t q, unsigned int idx,
			unsigned int *usage, unsigned int *type,
			unsigned int *match, gnutls_datum_t * data)
{
	if (idx >= q->data_entries)
		return gnutls_assert_val(DANE_E_REQUESTED_DATA_NOT_AVAILABLE);

	if (usage)
		*usage = q->usage[idx];
	if (type)
		*type = q->type[idx];
	if (match)
		*match = q->match[idx];
	if (data) {
		data->data = q->data[idx].data;
		data->size = q->data[idx].size;
	}

	return DANE_E_SUCCESS;
}