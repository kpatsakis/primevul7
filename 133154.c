static void pirlvl_destroy(jpc_pirlvl_t *rlvl)
{
	if (rlvl->prclyrnos) {
		jas_free(rlvl->prclyrnos);
	}
}