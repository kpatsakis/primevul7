static void __exit usbip_host_exit(void)
{
	driver_remove_file(&stub_driver.drvwrap.driver,
			   &driver_attr_match_busid);

	driver_remove_file(&stub_driver.drvwrap.driver,
			   &driver_attr_rebind);

	/*
	 * deregister() calls stub_disconnect() for all devices. Device
	 * specific data is cleared in stub_disconnect().
	 */
	usb_deregister_device_driver(&stub_driver);

	/* initiate scan to attach devices */
	stub_device_rebind();

	kmem_cache_destroy(stub_priv_cache);
}