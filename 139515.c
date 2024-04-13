vg_virgl_update_cursor_data(VuGpu *g, uint32_t resource_id,
                            gpointer data)
{
    uint32_t width, height;
    uint32_t *cursor;

    cursor = virgl_renderer_get_cursor_data(resource_id, &width, &height);
    g_return_if_fail(cursor != NULL);
    g_return_if_fail(width == 64);
    g_return_if_fail(height == 64);

    memcpy(data, cursor, 64 * 64 * sizeof(uint32_t));
    free(cursor);
}