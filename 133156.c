jpc_pchglist_t *jpc_pchglist_create()
{
	jpc_pchglist_t *pchglist;
	if (!(pchglist = jas_malloc(sizeof(jpc_pchglist_t)))) {
		return 0;
	}
	pchglist->numpchgs = 0;
	pchglist->maxpchgs = 0;
	pchglist->pchgs = 0;
	return pchglist;
}