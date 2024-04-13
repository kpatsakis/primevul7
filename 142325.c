cdf_dump_catalog(const cdf_header_t *h, const cdf_stream_t *sst)
{
	cdf_catalog_t *cat;
	cdf_unpack_catalog(h, sst, &cat);
	const cdf_catalog_entry_t *ce = cat->cat_e;
	struct timespec ts;
	char tbuf[64], sbuf[256];
	size_t i;

	printf("Catalog:\n");
	for (i = 0; i < cat->cat_num; i++) {
		cdf_timestamp_to_timespec(&ts, ce[i].ce_timestamp);
		printf("\t%d %s %s", ce[i].ce_num,
		    cdf_u16tos8(sbuf, ce[i].ce_namlen, ce[i].ce_name),
		    cdf_ctime(&ts.tv_sec, tbuf));
	}
	free(cat);
}