stack_slot_is_null_literal (ILStackDesc *value)
{
	return (value->stype & NULL_LITERAL_MASK) == NULL_LITERAL_MASK;
}