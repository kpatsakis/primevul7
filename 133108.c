void license_write_binary_blob(wStream* s, LICENSE_BLOB* blob)
{
	Stream_EnsureRemainingCapacity(s, blob->length +  4);

	Stream_Write_UINT16(s, blob->type); /* wBlobType (2 bytes) */
	Stream_Write_UINT16(s, blob->length); /* wBlobLen (2 bytes) */

	if (blob->length > 0)
		Stream_Write(s, blob->data, blob->length); /* blobData */
}