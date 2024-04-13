int ipmi_add_smi(struct module         *owner,
		 const struct ipmi_smi_handlers *handlers,
		 void		       *send_info,
		 struct device         *si_dev,
		 unsigned char         slave_addr)
{
	int              i, j;
	int              rv;
	struct ipmi_smi *intf, *tintf;
	struct list_head *link;
	struct ipmi_device_id id;

	/*
	 * Make sure the driver is actually initialized, this handles
	 * problems with initialization order.
	 */
	rv = ipmi_init_msghandler();
	if (rv)
		return rv;

	intf = kzalloc(sizeof(*intf), GFP_KERNEL);
	if (!intf)
		return -ENOMEM;

	rv = init_srcu_struct(&intf->users_srcu);
	if (rv) {
		kfree(intf);
		return rv;
	}

	intf->owner = owner;
	intf->bmc = &intf->tmp_bmc;
	INIT_LIST_HEAD(&intf->bmc->intfs);
	mutex_init(&intf->bmc->dyn_mutex);
	INIT_LIST_HEAD(&intf->bmc_link);
	mutex_init(&intf->bmc_reg_mutex);
	intf->intf_num = -1; /* Mark it invalid for now. */
	kref_init(&intf->refcount);
	INIT_WORK(&intf->bmc_reg_work, redo_bmc_reg);
	intf->si_dev = si_dev;
	for (j = 0; j < IPMI_MAX_CHANNELS; j++) {
		intf->addrinfo[j].address = IPMI_BMC_SLAVE_ADDR;
		intf->addrinfo[j].lun = 2;
	}
	if (slave_addr != 0)
		intf->addrinfo[0].address = slave_addr;
	INIT_LIST_HEAD(&intf->users);
	intf->handlers = handlers;
	intf->send_info = send_info;
	spin_lock_init(&intf->seq_lock);
	for (j = 0; j < IPMI_IPMB_NUM_SEQ; j++) {
		intf->seq_table[j].inuse = 0;
		intf->seq_table[j].seqid = 0;
	}
	intf->curr_seq = 0;
	spin_lock_init(&intf->waiting_rcv_msgs_lock);
	INIT_LIST_HEAD(&intf->waiting_rcv_msgs);
	tasklet_init(&intf->recv_tasklet,
		     smi_recv_tasklet,
		     (unsigned long) intf);
	atomic_set(&intf->watchdog_pretimeouts_to_deliver, 0);
	spin_lock_init(&intf->xmit_msgs_lock);
	INIT_LIST_HEAD(&intf->xmit_msgs);
	INIT_LIST_HEAD(&intf->hp_xmit_msgs);
	spin_lock_init(&intf->events_lock);
	spin_lock_init(&intf->watch_lock);
	atomic_set(&intf->event_waiters, 0);
	intf->ticks_to_req_ev = IPMI_REQUEST_EV_TIME;
	INIT_LIST_HEAD(&intf->waiting_events);
	intf->waiting_events_count = 0;
	mutex_init(&intf->cmd_rcvrs_mutex);
	spin_lock_init(&intf->maintenance_mode_lock);
	INIT_LIST_HEAD(&intf->cmd_rcvrs);
	init_waitqueue_head(&intf->waitq);
	for (i = 0; i < IPMI_NUM_STATS; i++)
		atomic_set(&intf->stats[i], 0);

	mutex_lock(&ipmi_interfaces_mutex);
	/* Look for a hole in the numbers. */
	i = 0;
	link = &ipmi_interfaces;
	list_for_each_entry_rcu(tintf, &ipmi_interfaces, link) {
		if (tintf->intf_num != i) {
			link = &tintf->link;
			break;
		}
		i++;
	}
	/* Add the new interface in numeric order. */
	if (i == 0)
		list_add_rcu(&intf->link, &ipmi_interfaces);
	else
		list_add_tail_rcu(&intf->link, link);

	rv = handlers->start_processing(send_info, intf);
	if (rv)
		goto out_err;

	rv = __bmc_get_device_id(intf, NULL, &id, NULL, NULL, i);
	if (rv) {
		dev_err(si_dev, "Unable to get the device id: %d\n", rv);
		goto out_err_started;
	}

	mutex_lock(&intf->bmc_reg_mutex);
	rv = __scan_channels(intf, &id);
	mutex_unlock(&intf->bmc_reg_mutex);
	if (rv)
		goto out_err_bmc_reg;

	/*
	 * Keep memory order straight for RCU readers.  Make
	 * sure everything else is committed to memory before
	 * setting intf_num to mark the interface valid.
	 */
	smp_wmb();
	intf->intf_num = i;
	mutex_unlock(&ipmi_interfaces_mutex);

	/* After this point the interface is legal to use. */
	call_smi_watchers(i, intf->si_dev);

	return 0;

 out_err_bmc_reg:
	ipmi_bmc_unregister(intf);
 out_err_started:
	if (intf->handlers->shutdown)
		intf->handlers->shutdown(intf->send_info);
 out_err:
	list_del_rcu(&intf->link);
	mutex_unlock(&ipmi_interfaces_mutex);
	synchronize_srcu(&ipmi_interfaces_srcu);
	cleanup_srcu_struct(&intf->users_srcu);
	kref_put(&intf->refcount, intf_free);

	return rv;
}