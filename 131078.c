static zend_object_value spl_dllist_object_new(zend_class_entry *class_type TSRMLS_DC) /* {{{ */
{
	spl_dllist_object *tmp;
	return spl_dllist_object_new_ex(class_type, &tmp, NULL, 0 TSRMLS_CC);
}