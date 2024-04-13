PHP_METHOD(Phar, __destruct)
{
	zval *zobj = getThis();
	phar_archive_object *phar_obj = (phar_archive_object*)((char*)Z_OBJ_P(zobj) - Z_OBJ_P(zobj)->handlers->offset);

	if (phar_obj->archive && phar_obj->archive->is_persistent) {
		zend_hash_str_del(&PHAR_G(phar_persist_map), (const char *) phar_obj->archive, sizeof(phar_obj->archive));
	}
}