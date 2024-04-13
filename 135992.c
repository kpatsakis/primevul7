static bool nsim_xdp_offload_active(struct netdevsim *ns)
{
	return ns->xdp_hw.prog;
}