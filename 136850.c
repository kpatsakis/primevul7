static void phar_spl_foreign_dtor(spl_filesystem_object *object) /* {{{ */
{
	phar_archive_data *phar = (phar_archive_data *) object->oth;

	if (!phar->is_persistent) {
		phar_archive_delref(phar);
	}

	object->oth = NULL;
}