mono_custom_attrs_free (MonoCustomAttrInfo *ainfo)
{
	if (!ainfo->cached)
		g_free (ainfo);
}