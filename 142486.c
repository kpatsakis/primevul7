static void sapi_read_post_data(TSRMLS_D)
{
	sapi_post_entry *post_entry;
	uint content_type_length = strlen(SG(request_info).content_type);
	char *content_type = estrndup(SG(request_info).content_type, content_type_length);
	char *p;
	char oldchar=0;
	void (*post_reader_func)(TSRMLS_D) = NULL;


	/* dedicated implementation for increased performance:
	 * - Make the content type lowercase
	 * - Trim descriptive data, stay with the content-type only
	 */
	for (p=content_type; p<content_type+content_type_length; p++) {
		switch (*p) {
			case ';':
			case ',':
			case ' ':
				content_type_length = p-content_type;
				oldchar = *p;
				*p = 0;
				break;
			default:
				*p = tolower(*p);
				break;
		}
	}

	/* now try to find an appropriate POST content handler */
	if (zend_hash_find(&SG(known_post_content_types), content_type,
			content_type_length+1, (void **) &post_entry) == SUCCESS) {
		/* found one, register it for use */
		SG(request_info).post_entry = post_entry;
		post_reader_func = post_entry->post_reader;
	} else {
		/* fallback */
		SG(request_info).post_entry = NULL;
		if (!sapi_module.default_post_reader) {
			/* no default reader ? */
			SG(request_info).content_type_dup = NULL;
			sapi_module.sapi_error(E_WARNING, "Unsupported content type:  '%s'", content_type);
			return;
		}
	}
	if (oldchar) {
		*(p-1) = oldchar;
	}

	SG(request_info).content_type_dup = content_type;

	if(post_reader_func) {
		post_reader_func(TSRMLS_C);
	}

	if(sapi_module.default_post_reader) {
		sapi_module.default_post_reader(TSRMLS_C);
	}
}