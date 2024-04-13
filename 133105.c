void license_write_encrypted_premaster_secret_blob(wStream* s, LICENSE_BLOB* blob, UINT32 ModulusLength)
{
	UINT32 length;

	length = ModulusLength + 8;

	if (blob->length > ModulusLength)
	{
		fprintf(stderr, "license_write_encrypted_premaster_secret_blob: invalid blob\n");
		return;
	}

	Stream_EnsureRemainingCapacity(s, length + 4);

	Stream_Write_UINT16(s, blob->type); /* wBlobType (2 bytes) */
	Stream_Write_UINT16(s, length); /* wBlobLen (2 bytes) */

	if (blob->length > 0)
		Stream_Write(s, blob->data, blob->length); /* blobData */

	Stream_Zero(s, length - blob->length);
}