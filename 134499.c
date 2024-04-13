static bool megasas_intr_enabled(MegasasState *s)
{
    if ((s->intr_mask & MEGASAS_INTR_DISABLED_MASK) !=
        MEGASAS_INTR_DISABLED_MASK) {
        return true;
    }
    return false;
}