/* Advanced Interface */
PHPAPI zval *php_date_instantiate(zend_class_entry *pce, zval *object TSRMLS_DC)
{
	object_init_ex(object, pce);
	return object;