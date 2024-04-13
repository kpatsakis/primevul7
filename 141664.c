void xfrm_flush_gc(void)
{
	flush_work(&xfrm_state_gc_work);
}