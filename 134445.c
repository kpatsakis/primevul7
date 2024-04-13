static bool megasas_is_jbod(MegasasState *s)
{
    return s->flags & MEGASAS_MASK_USE_JBOD;
}