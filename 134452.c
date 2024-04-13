static bool megasas_use_queue64(MegasasState *s)
{
    return s->flags & MEGASAS_MASK_USE_QUEUE64;
}