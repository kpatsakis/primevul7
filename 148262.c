static zend_always_inline void zend_free_obj_get_result(zval *op) /* {{{ */
{
	ZEND_ASSERT(!Z_REFCOUNTED_P(op) || Z_REFCOUNT_P(op) != 0);
	zval_ptr_dtor(op);
}