static void nvme_realize(PCIDevice *pci_dev, Error **errp)
{
    NvmeCtrl *n = NVME(pci_dev);
    NvmeNamespace *ns;
    Error *local_err = NULL;

    nvme_check_constraints(n, &local_err);
    if (local_err) {
        error_propagate(errp, local_err);
        return;
    }

    qbus_init(&n->bus, sizeof(NvmeBus), TYPE_NVME_BUS,
              &pci_dev->qdev, n->parent_obj.qdev.id);

    nvme_init_state(n);
    if (nvme_init_pci(n, pci_dev, errp)) {
        return;
    }

    if (nvme_init_subsys(n, errp)) {
        error_propagate(errp, local_err);
        return;
    }
    nvme_init_ctrl(n, pci_dev);

    /* setup a namespace if the controller drive property was given */
    if (n->namespace.blkconf.blk) {
        ns = &n->namespace;
        ns->params.nsid = 1;

        if (nvme_ns_setup(ns, errp)) {
            return;
        }

        nvme_attach_ns(n, ns);
    }
}