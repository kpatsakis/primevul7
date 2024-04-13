dump_predicate(PredType type, PredIdx idx)
/* predicate function to use for ordinary decompilation */
{
    switch (type) {
    case BOOLEAN:
	return (cur_type->Booleans[idx] == FALSE)
	    ? FAIL : cur_type->Booleans[idx];

    case NUMBER:
	return (cur_type->Numbers[idx] == ABSENT_NUMERIC)
	    ? FAIL : cur_type->Numbers[idx];

    case STRING:
	return (cur_type->Strings[idx] != ABSENT_STRING)
	    ? (int) TRUE : FAIL;
    }

    return (FALSE);		/* pacify compiler */
}