static bool xfrm_is_alive(const struct km_event *c)
{
	return (bool)xfrm_acquire_is_on(c->net);
}