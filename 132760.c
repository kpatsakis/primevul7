static int __bmc_get_device_id(struct ipmi_smi *intf, struct bmc_device *bmc,
			       struct ipmi_device_id *id,
			       bool *guid_set, guid_t *guid, int intf_num)
{
	int rv = 0;
	int prev_dyn_id_set, prev_guid_set;
	bool intf_set = intf != NULL;

	if (!intf) {
		mutex_lock(&bmc->dyn_mutex);
retry_bmc_lock:
		if (list_empty(&bmc->intfs)) {
			mutex_unlock(&bmc->dyn_mutex);
			return -ENOENT;
		}
		intf = list_first_entry(&bmc->intfs, struct ipmi_smi,
					bmc_link);
		kref_get(&intf->refcount);
		mutex_unlock(&bmc->dyn_mutex);
		mutex_lock(&intf->bmc_reg_mutex);
		mutex_lock(&bmc->dyn_mutex);
		if (intf != list_first_entry(&bmc->intfs, struct ipmi_smi,
					     bmc_link)) {
			mutex_unlock(&intf->bmc_reg_mutex);
			kref_put(&intf->refcount, intf_free);
			goto retry_bmc_lock;
		}
	} else {
		mutex_lock(&intf->bmc_reg_mutex);
		bmc = intf->bmc;
		mutex_lock(&bmc->dyn_mutex);
		kref_get(&intf->refcount);
	}

	/* If we have a valid and current ID, just return that. */
	if (intf->in_bmc_register ||
	    (bmc->dyn_id_set && time_is_after_jiffies(bmc->dyn_id_expiry)))
		goto out_noprocessing;

	prev_guid_set = bmc->dyn_guid_set;
	__get_guid(intf);

	prev_dyn_id_set = bmc->dyn_id_set;
	rv = __get_device_id(intf, bmc);
	if (rv)
		goto out;

	/*
	 * The guid, device id, manufacturer id, and product id should
	 * not change on a BMC.  If it does we have to do some dancing.
	 */
	if (!intf->bmc_registered
	    || (!prev_guid_set && bmc->dyn_guid_set)
	    || (!prev_dyn_id_set && bmc->dyn_id_set)
	    || (prev_guid_set && bmc->dyn_guid_set
		&& !guid_equal(&bmc->guid, &bmc->fetch_guid))
	    || bmc->id.device_id != bmc->fetch_id.device_id
	    || bmc->id.manufacturer_id != bmc->fetch_id.manufacturer_id
	    || bmc->id.product_id != bmc->fetch_id.product_id) {
		struct ipmi_device_id id = bmc->fetch_id;
		int guid_set = bmc->dyn_guid_set;
		guid_t guid;

		guid = bmc->fetch_guid;
		mutex_unlock(&bmc->dyn_mutex);

		__ipmi_bmc_unregister(intf);
		/* Fill in the temporary BMC for good measure. */
		intf->bmc->id = id;
		intf->bmc->dyn_guid_set = guid_set;
		intf->bmc->guid = guid;
		if (__ipmi_bmc_register(intf, &id, guid_set, &guid, intf_num))
			need_waiter(intf); /* Retry later on an error. */
		else
			__scan_channels(intf, &id);


		if (!intf_set) {
			/*
			 * We weren't given the interface on the
			 * command line, so restart the operation on
			 * the next interface for the BMC.
			 */
			mutex_unlock(&intf->bmc_reg_mutex);
			mutex_lock(&bmc->dyn_mutex);
			goto retry_bmc_lock;
		}

		/* We have a new BMC, set it up. */
		bmc = intf->bmc;
		mutex_lock(&bmc->dyn_mutex);
		goto out_noprocessing;
	} else if (memcmp(&bmc->fetch_id, &bmc->id, sizeof(bmc->id)))
		/* Version info changes, scan the channels again. */
		__scan_channels(intf, &bmc->fetch_id);

	bmc->dyn_id_expiry = jiffies + IPMI_DYN_DEV_ID_EXPIRY;

out:
	if (rv && prev_dyn_id_set) {
		rv = 0; /* Ignore failures if we have previous data. */
		bmc->dyn_id_set = prev_dyn_id_set;
	}
	if (!rv) {
		bmc->id = bmc->fetch_id;
		if (bmc->dyn_guid_set)
			bmc->guid = bmc->fetch_guid;
		else if (prev_guid_set)
			/*
			 * The guid used to be valid and it failed to fetch,
			 * just use the cached value.
			 */
			bmc->dyn_guid_set = prev_guid_set;
	}
out_noprocessing:
	if (!rv) {
		if (id)
			*id = bmc->id;

		if (guid_set)
			*guid_set = bmc->dyn_guid_set;

		if (guid && bmc->dyn_guid_set)
			*guid =  bmc->guid;
	}

	mutex_unlock(&bmc->dyn_mutex);
	mutex_unlock(&intf->bmc_reg_mutex);

	kref_put(&intf->refcount, intf_free);
	return rv;
}