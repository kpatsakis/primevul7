RedRecord *reds_get_record(RedsState *reds)
{
    if (reds->record) {
        return red_record_ref(reds->record);
    }

    return NULL;
}