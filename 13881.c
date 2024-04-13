static gboolean reds_add_renderer(RedsState *reds, const char *name)
{
    uint32_t index;

    if (reds->config->renderers->len == RED_RENDERER_LAST ||
        !get_name_index(renderer_names, name, &index)) {
        return FALSE;
    }
    g_array_append_val(reds->config->renderers, renderer_names[index].id);
    return TRUE;
}