int nsim_bpf_setup_tc_block_cb(enum tc_setup_type type,
			       void *type_data, void *cb_priv)
{
	struct tc_cls_bpf_offload *cls_bpf = type_data;
	struct bpf_prog *prog = cls_bpf->prog;
	struct netdevsim *ns = cb_priv;
	struct bpf_prog *oldprog;

	if (type != TC_SETUP_CLSBPF) {
		NSIM_EA(cls_bpf->common.extack,
			"only offload of BPF classifiers supported");
		return -EOPNOTSUPP;
	}

	if (!tc_cls_can_offload_and_chain0(ns->netdev, &cls_bpf->common))
		return -EOPNOTSUPP;

	if (cls_bpf->common.protocol != htons(ETH_P_ALL)) {
		NSIM_EA(cls_bpf->common.extack,
			"only ETH_P_ALL supported as filter protocol");
		return -EOPNOTSUPP;
	}

	if (!ns->bpf_tc_accept) {
		NSIM_EA(cls_bpf->common.extack,
			"netdevsim configured to reject BPF TC offload");
		return -EOPNOTSUPP;
	}
	/* Note: progs without skip_sw will probably not be dev bound */
	if (prog && !prog->aux->offload && !ns->bpf_tc_non_bound_accept) {
		NSIM_EA(cls_bpf->common.extack,
			"netdevsim configured to reject unbound programs");
		return -EOPNOTSUPP;
	}

	if (cls_bpf->command != TC_CLSBPF_OFFLOAD)
		return -EOPNOTSUPP;

	oldprog = cls_bpf->oldprog;

	/* Don't remove if oldprog doesn't match driver's state */
	if (ns->bpf_offloaded != oldprog) {
		oldprog = NULL;
		if (!cls_bpf->prog)
			return 0;
		if (ns->bpf_offloaded) {
			NSIM_EA(cls_bpf->common.extack,
				"driver and netdev offload states mismatch");
			return -EBUSY;
		}
	}

	return nsim_bpf_offload(ns, cls_bpf->prog, oldprog);
}