void license_free_binary_blob(LICENSE_BLOB* blob)
{
	if (blob->data != NULL)
		free(blob->data);

	free(blob);
}