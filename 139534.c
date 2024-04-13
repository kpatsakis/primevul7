void stub_device_cleanup_urbs(struct stub_device *sdev)
{
	struct stub_priv *priv;
	struct urb *urb;

	dev_dbg(&sdev->udev->dev, "Stub device cleaning up urbs\n");

	while ((priv = stub_priv_pop(sdev))) {
		urb = priv->urb;
		dev_dbg(&sdev->udev->dev, "free urb seqnum %lu\n",
			priv->seqnum);
		usb_kill_urb(urb);

		kmem_cache_free(stub_priv_cache, priv);

		kfree(urb->transfer_buffer);
		urb->transfer_buffer = NULL;

		kfree(urb->setup_packet);
		urb->setup_packet = NULL;

		usb_free_urb(urb);
	}
}