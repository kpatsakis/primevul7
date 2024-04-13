PHP_METHOD(PharFileInfo, __destruct)
{
	zval *zobj = getThis();
	phar_entry_object *entry_obj = (phar_entry_object*)((char*)Z_OBJ_P(zobj) - Z_OBJ_P(zobj)->handlers->offset);

	if (entry_obj->entry && entry_obj->entry->is_temp_dir) {
		if (entry_obj->entry->filename) {
			efree(entry_obj->entry->filename);
			entry_obj->entry->filename = NULL;
		}

		efree(entry_obj->entry);
		entry_obj->entry = NULL;
	}
}