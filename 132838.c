static int ipmi_register_driver(void)
{
	int rv;

	if (drvregistered)
		return 0;

	rv = driver_register(&ipmidriver.driver);
	if (rv)
		pr_err("Could not register IPMI driver\n");
	else
		drvregistered = true;
	return rv;
}