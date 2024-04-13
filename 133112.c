LICENSE_BLOB* license_new_binary_blob(UINT16 type)
{
	LICENSE_BLOB* blob;

	blob = (LICENSE_BLOB*) malloc(sizeof(LICENSE_BLOB));
	blob->type = type;
	blob->length = 0;
	blob->data = NULL;

	return blob;
}