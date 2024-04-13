static bool get_protected_flag(struct pai_val *pal)
{
	if (!pal)
		return False;
	return pal->pai_protected;
}