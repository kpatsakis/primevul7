static canon_ace *dup_canon_ace( canon_ace *src_ace)
{
	canon_ace *dst_ace = SMB_MALLOC_P(canon_ace);

	if (dst_ace == NULL)
		return NULL;

	*dst_ace = *src_ace;
	dst_ace->prev = dst_ace->next = NULL;
	return dst_ace;
}