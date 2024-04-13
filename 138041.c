static void nvme_init_subnqn(NvmeCtrl *n)
{
    NvmeSubsystem *subsys = n->subsys;
    NvmeIdCtrl *id = &n->id_ctrl;

    if (!subsys) {
        snprintf((char *)id->subnqn, sizeof(id->subnqn),
                 "nqn.2019-08.org.qemu:%s", n->params.serial);
    } else {
        pstrcpy((char *)id->subnqn, sizeof(id->subnqn), (char*)subsys->subnqn);
    }
}