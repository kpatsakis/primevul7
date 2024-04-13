
zend_object_iterator *date_object_period_get_iterator(zend_class_entry *ce, zval *object, int by_ref TSRMLS_DC)
{
	date_period_it  *iterator = emalloc(sizeof(date_period_it));
	php_period_obj  *dpobj    = (php_period_obj *)zend_object_store_get_object(object TSRMLS_CC);

	if (by_ref) {
		zend_error(E_ERROR, "An iterator cannot be used with foreach by reference");
	}

	Z_ADDREF_P(object);
	iterator->intern.data = (void*) dpobj;
	iterator->intern.funcs = &date_period_it_funcs;
	iterator->date_period_zval = object;
	iterator->object = dpobj;
	iterator->current = NULL;

	return (zend_object_iterator*)iterator;