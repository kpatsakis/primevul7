/* {{{ date_period_read_property */
static zval *date_period_read_property(zval *object, zval *member, int type, const zend_literal *key TSRMLS_DC)
{
	zval *zv;
	if (type != BP_VAR_IS && type != BP_VAR_R) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Retrieval of DatePeriod properties for modification is unsupported");
	}

	Z_OBJPROP_P(object); /* build properties hash table */

	zv = std_object_handlers.read_property(object, member, type, key TSRMLS_CC);
	if (Z_TYPE_P(zv) == IS_OBJECT && Z_OBJ_HANDLER_P(zv, clone_obj)) {
		/* defensive copy */
		zend_object_value zov = Z_OBJ_HANDLER_P(zv, clone_obj)(zv TSRMLS_CC);
		MAKE_STD_ZVAL(zv);
		Z_TYPE_P(zv) = IS_OBJECT;
		Z_OBJVAL_P(zv) = zov;
	}

	return zv;