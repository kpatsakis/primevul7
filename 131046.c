static void spl_dllist_it_rewind(zend_object_iterator *iter TSRMLS_DC) /* {{{ */
{
	spl_dllist_it     *iterator = (spl_dllist_it *)iter;
	spl_dllist_object *object   = iterator->object;
	spl_ptr_llist     *llist    = object->llist;

	spl_dllist_it_helper_rewind(&iterator->traverse_pointer, &iterator->traverse_position, llist, object->flags TSRMLS_CC);
}