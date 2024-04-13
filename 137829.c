 Return hash id for given object */
PHP_FUNCTION(spl_object_hash)
{
	zval *obj;
	char* hash;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "o", &obj) == FAILURE) {
		return;
	}
	
	hash = emalloc(33);
	php_spl_object_hash(obj, hash TSRMLS_CC);
	
	RETVAL_STRING(hash, 0);