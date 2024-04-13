load_attachment_idle (EAttachment *attachment)
{
	e_attachment_load_async (
		attachment,
		(GAsyncReadyCallback) attachment_loaded, NULL);

	return FALSE;
}