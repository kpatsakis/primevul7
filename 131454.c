static void spl_array_object_free_storage(void *object TSRMLS_DC)
{
	spl_array_object *intern = (spl_array_object *)object;

	zend_object_std_dtor(&intern->std TSRMLS_CC);

	zval_ptr_dtor(&intern->array);
	zval_ptr_dtor(&intern->retval);

	if (intern->debug_info != NULL) {
		zend_hash_destroy(intern->debug_info);
		efree(intern->debug_info);
	}

	efree(object);
}