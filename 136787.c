PHP_METHOD(PharFileInfo, hasMetadata)
{
	PHAR_ENTRY_OBJECT();

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	RETURN_BOOL(Z_TYPE(entry_obj->entry->metadata) != IS_UNDEF);
}