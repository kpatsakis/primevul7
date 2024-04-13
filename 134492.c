static bool megasas_use_msix(MegasasState *s)
{
    return s->msix != ON_OFF_AUTO_OFF;
}