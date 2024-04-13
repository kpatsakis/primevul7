static void xfrm_put_type(const struct xfrm_type *type)
{
	module_put(type->owner);
}