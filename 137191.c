static unsigned long xdp_get_metalen(const struct xdp_buff *xdp)
{
	return xdp_data_meta_unsupported(xdp) ? 0 :
	       xdp->data - xdp->data_meta;
}