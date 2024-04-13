static int __init usbip_host_init(void)
{
	int ret;

	init_busid_table();

	stub_priv_cache = KMEM_CACHE(stub_priv, SLAB_HWCACHE_ALIGN);
	if (!stub_priv_cache) {
		pr_err("kmem_cache_create failed\n");
		return -ENOMEM;
	}

	ret = usb_register_device_driver(&stub_driver, THIS_MODULE);
	if (ret) {
		pr_err("usb_register failed %d\n", ret);
		goto err_usb_register;
	}

	ret = driver_create_file(&stub_driver.drvwrap.driver,
				 &driver_attr_match_busid);
	if (ret) {
		pr_err("driver_create_file failed\n");
		goto err_create_file;
	}

	ret = driver_create_file(&stub_driver.drvwrap.driver,
				 &driver_attr_rebind);
	if (ret) {
		pr_err("driver_create_file failed\n");
		goto err_create_file;
	}

	return ret;

err_create_file:
	usb_deregister_device_driver(&stub_driver);
err_usb_register:
	kmem_cache_destroy(stub_priv_cache);
	return ret;
}