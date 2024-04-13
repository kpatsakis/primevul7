static void xfrm_put_type_offload(const struct xfrm_type_offload *type)
{
	module_put(type->owner);
}