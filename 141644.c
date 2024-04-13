static void xfrm_put_mode(struct xfrm_mode *mode)
{
	module_put(mode->owner);
}