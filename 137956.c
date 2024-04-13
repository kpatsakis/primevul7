static int nvme_init_subsys(NvmeCtrl *n, Error **errp)
{
    int cntlid;

    if (!n->subsys) {
        return 0;
    }

    cntlid = nvme_subsys_register_ctrl(n, errp);
    if (cntlid < 0) {
        return -1;
    }

    n->cntlid = cntlid;

    return 0;
}