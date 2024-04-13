compare_genericparam (const void *a, const void *b)
{
	const GenericParamTableEntry **a_entry = (const GenericParamTableEntry **) a;
	const GenericParamTableEntry **b_entry = (const GenericParamTableEntry **) b;

	if ((*b_entry)->owner == (*a_entry)->owner)
		return 
			mono_type_get_generic_param_num (mono_reflection_type_get_handle ((MonoReflectionType*)(*a_entry)->gparam)) -
			mono_type_get_generic_param_num (mono_reflection_type_get_handle ((MonoReflectionType*)(*b_entry)->gparam));
	else
		return (*a_entry)->owner - (*b_entry)->owner;
}