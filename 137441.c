static int stub_recv_cmd_unlink(struct stub_device *sdev,
				struct usbip_header *pdu)
{
	int ret;
	unsigned long flags;
	struct stub_priv *priv;

	spin_lock_irqsave(&sdev->priv_lock, flags);

	list_for_each_entry(priv, &sdev->priv_init, list) {
		if (priv->seqnum != pdu->u.cmd_unlink.seqnum)
			continue;

		dev_info(&priv->urb->dev->dev, "unlink urb %p\n",
			 priv->urb);

		/*
		 * This matched urb is not completed yet (i.e., be in
		 * flight in usb hcd hardware/driver). Now we are
		 * cancelling it. The unlinking flag means that we are
		 * now not going to return the normal result pdu of a
		 * submission request, but going to return a result pdu
		 * of the unlink request.
		 */
		priv->unlinking = 1;

		/*
		 * In the case that unlinking flag is on, prev->seqnum
		 * is changed from the seqnum of the cancelling urb to
		 * the seqnum of the unlink request. This will be used
		 * to make the result pdu of the unlink request.
		 */
		priv->seqnum = pdu->base.seqnum;

		spin_unlock_irqrestore(&sdev->priv_lock, flags);

		/*
		 * usb_unlink_urb() is now out of spinlocking to avoid
		 * spinlock recursion since stub_complete() is
		 * sometimes called in this context but not in the
		 * interrupt context.  If stub_complete() is executed
		 * before we call usb_unlink_urb(), usb_unlink_urb()
		 * will return an error value. In this case, stub_tx
		 * will return the result pdu of this unlink request
		 * though submission is completed and actual unlinking
		 * is not executed. OK?
		 */
		/* In the above case, urb->status is not -ECONNRESET,
		 * so a driver in a client host will know the failure
		 * of the unlink request ?
		 */
		ret = usb_unlink_urb(priv->urb);
		if (ret != -EINPROGRESS)
			dev_err(&priv->urb->dev->dev,
				"failed to unlink a urb %p, ret %d\n",
				priv->urb, ret);

		return 0;
	}

	usbip_dbg_stub_rx("seqnum %d is not pending\n",
			  pdu->u.cmd_unlink.seqnum);

	/*
	 * The urb of the unlink target is not found in priv_init queue. It was
	 * already completed and its results is/was going to be sent by a
	 * CMD_RET pdu. In this case, usb_unlink_urb() is not needed. We only
	 * return the completeness of this unlink request to vhci_hcd.
	 */
	stub_enqueue_ret_unlink(sdev, pdu->base.seqnum, 0);

	spin_unlock_irqrestore(&sdev->priv_lock, flags);

	return 0;
}