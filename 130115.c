stack_slot_is_boxed_value (ILStackDesc *value)
{
	return (value->stype & BOXED_MASK) == BOXED_MASK;
}