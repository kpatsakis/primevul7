static int aspeed_lpc_ctrl_probe(struct platform_device *pdev)
{
	struct aspeed_lpc_ctrl *lpc_ctrl;
	struct device_node *node;
	struct resource resm;
	struct device *dev;
	struct device_node *np;
	int rc;

	dev = &pdev->dev;

	lpc_ctrl = devm_kzalloc(dev, sizeof(*lpc_ctrl), GFP_KERNEL);
	if (!lpc_ctrl)
		return -ENOMEM;

	/* If flash is described in device tree then store */
	node = of_parse_phandle(dev->of_node, "flash", 0);
	if (!node) {
		dev_dbg(dev, "Didn't find host pnor flash node\n");
	} else {
		rc = of_address_to_resource(node, 1, &resm);
		of_node_put(node);
		if (rc) {
			dev_err(dev, "Couldn't address to resource for flash\n");
			return rc;
		}

		lpc_ctrl->pnor_size = resource_size(&resm);
		lpc_ctrl->pnor_base = resm.start;
	}


	dev_set_drvdata(&pdev->dev, lpc_ctrl);

	/* If memory-region is described in device tree then store */
	node = of_parse_phandle(dev->of_node, "memory-region", 0);
	if (!node) {
		dev_dbg(dev, "Didn't find reserved memory\n");
	} else {
		rc = of_address_to_resource(node, 0, &resm);
		of_node_put(node);
		if (rc) {
			dev_err(dev, "Couldn't address to resource for reserved memory\n");
			return -ENXIO;
		}

		lpc_ctrl->mem_size = resource_size(&resm);
		lpc_ctrl->mem_base = resm.start;

		if (!is_power_of_2(lpc_ctrl->mem_size)) {
			dev_err(dev, "Reserved memory size must be a power of 2, got %u\n",
			       (unsigned int)lpc_ctrl->mem_size);
			return -EINVAL;
		}

		if (!IS_ALIGNED(lpc_ctrl->mem_base, lpc_ctrl->mem_size)) {
			dev_err(dev, "Reserved memory must be naturally aligned for size %u\n",
			       (unsigned int)lpc_ctrl->mem_size);
			return -EINVAL;
		}
	}

	np = pdev->dev.parent->of_node;
	if (!of_device_is_compatible(np, "aspeed,ast2400-lpc-v2") &&
	    !of_device_is_compatible(np, "aspeed,ast2500-lpc-v2") &&
	    !of_device_is_compatible(np, "aspeed,ast2600-lpc-v2")) {
		dev_err(dev, "unsupported LPC device binding\n");
		return -ENODEV;
	}

	lpc_ctrl->regmap = syscon_node_to_regmap(np);
	if (IS_ERR(lpc_ctrl->regmap)) {
		dev_err(dev, "Couldn't get regmap\n");
		return -ENODEV;
	}

	lpc_ctrl->clk = devm_clk_get(dev, NULL);
	if (IS_ERR(lpc_ctrl->clk)) {
		dev_err(dev, "couldn't get clock\n");
		return PTR_ERR(lpc_ctrl->clk);
	}
	rc = clk_prepare_enable(lpc_ctrl->clk);
	if (rc) {
		dev_err(dev, "couldn't enable clock\n");
		return rc;
	}

	if (of_device_is_compatible(dev->of_node, "aspeed,ast2600-lpc-ctrl"))
		lpc_ctrl->fwh2ahb = true;

	lpc_ctrl->miscdev.minor = MISC_DYNAMIC_MINOR;
	lpc_ctrl->miscdev.name = DEVICE_NAME;
	lpc_ctrl->miscdev.fops = &aspeed_lpc_ctrl_fops;
	lpc_ctrl->miscdev.parent = dev;
	rc = misc_register(&lpc_ctrl->miscdev);
	if (rc) {
		dev_err(dev, "Unable to register device\n");
		goto err;
	}

	return 0;

err:
	clk_disable_unprepare(lpc_ctrl->clk);
	return rc;
}