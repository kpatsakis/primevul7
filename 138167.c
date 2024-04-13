
static zend_object_value date_object_clone_period(zval *this_ptr TSRMLS_DC)
{
	php_period_obj *new_obj = NULL;
	php_period_obj *old_obj = (php_period_obj *) zend_object_store_get_object(this_ptr TSRMLS_CC);
	zend_object_value new_ov = date_object_new_period_ex(old_obj->std.ce, &new_obj TSRMLS_CC);
	
	zend_objects_clone_members(&new_obj->std, new_ov, &old_obj->std, Z_OBJ_HANDLE_P(this_ptr) TSRMLS_CC);

	/** FIX ME ADD CLONE STUFF **/
	return new_ov;