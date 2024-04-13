PHP_METHOD(Phar, hasMetadata)
{
	PHAR_ARCHIVE_OBJECT();

	RETURN_BOOL(Z_TYPE(phar_obj->archive->metadata) != IS_UNDEF);
}