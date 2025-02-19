static void spl_dllist_it_move_forward(zend_object_iterator *iter TSRMLS_DC) /* {{{ */
{
	spl_dllist_it     *iterator = (spl_dllist_it *)iter;
	spl_dllist_object *object   = iterator->object;

	zend_user_it_invalidate_current(iter TSRMLS_CC);

	spl_dllist_it_helper_move_forward(&iterator->traverse_pointer, &iterator->traverse_position, object->llist, object->flags TSRMLS_CC);
}