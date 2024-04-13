gdk_pixbuf__ico_is_save_option_supported (const gchar *option_key)
{
        if (g_strcmp0 (option_key, "depth") == 0 ||
            g_strcmp0 (option_key, "x_hot") == 0 ||
            g_strcmp0 (option_key, "y_hot") == 0)
                return TRUE;

        return FALSE;
}