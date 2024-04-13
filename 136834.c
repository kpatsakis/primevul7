PHP_METHOD(Phar, getPath)
{
	PHAR_ARCHIVE_OBJECT();

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	RETURN_STRINGL(phar_obj->archive->fname, phar_obj->archive->fname_len);
}