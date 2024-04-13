pad_heap (MonoDynamicStream *sh)
{
	if (sh->index & 3) {
		int sz = 4 - (sh->index & 3);
		memset (sh->data + sh->index, 0, sz);
		sh->index += sz;
	}
}