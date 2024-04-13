mono_reflection_parse_type (char *name, MonoTypeNameParse *info)
{
	return _mono_reflection_parse_type (name, NULL, FALSE, info);
}