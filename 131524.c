static void dnsserver_reload_zones(struct dnsserver_state *dsstate)
{
	struct dnsserver_partition *p;
	struct dnsserver_zone *zones, *z, *znext, *zmatch;
	struct dnsserver_zone *old_list, *new_list;

	old_list = dsstate->zones;
	new_list = NULL;

	for (p = dsstate->partitions; p; p = p->next) {
		zones = dnsserver_db_enumerate_zones(dsstate, dsstate->samdb, p);
		if (zones == NULL) {
			continue;
		}
		for (z = zones; z; ) {
			znext = z->next;
			zmatch = dnsserver_find_zone(old_list, z->name);
			if (zmatch == NULL) {
				/* Missing zone */
				z->zoneinfo = dnsserver_init_zoneinfo(z, dsstate->serverinfo);
				if (z->zoneinfo == NULL) {
					continue;
				}
				DLIST_ADD_END(new_list, z);
				p->zones_count++;
				dsstate->zones_count++;
			} else {
				/* Existing zone */
				talloc_free(z);
				DLIST_REMOVE(old_list, zmatch);
				DLIST_ADD_END(new_list, zmatch);
			}
			z = znext;
		}
	}

	if (new_list == NULL) {
		return;
	}

	/* Deleted zones */
	for (z = old_list; z; ) {
		znext = z->next;
		z->partition->zones_count--;
		dsstate->zones_count--;
		talloc_free(z);
		z = znext;
	}

	dsstate->zones = new_list;
}