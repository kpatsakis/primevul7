static gboolean
verify_valuetype_layout (MonoClass *class)
{
	gboolean res;
	res = verify_valuetype_layout_with_target (class, class);
	return res;