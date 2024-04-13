PHPAPI int php_lint_script(zend_file_handle *file)
{
	zend_op_array *op_array;
	int retval = FAILURE;

	zend_try {
		op_array = zend_compile_file(file, ZEND_INCLUDE);
		zend_destroy_file_handle(file);

		if (op_array) {
			destroy_op_array(op_array);
			efree(op_array);
			retval = SUCCESS;
		}
	} zend_end_try();
	if (EG(exception)) {
		zend_exception_error(EG(exception), E_ERROR);
	}

	return retval;
}