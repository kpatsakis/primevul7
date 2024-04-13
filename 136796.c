PHP_METHOD(Phar, getAlias)
{
	PHAR_ARCHIVE_OBJECT();

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (phar_obj->archive->alias && phar_obj->archive->alias != phar_obj->archive->fname) {
		RETURN_STRINGL(phar_obj->archive->alias, phar_obj->archive->alias_len);
	}
}