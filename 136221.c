MODULE_ENTRY (fill_info) (GdkPixbufFormat *info)
{
	static const GdkPixbufModulePattern signature[] = {
		{ "  \x1   ", "zz znz", 100 }, 
		{ "  \x2   ", "zz znz", 100 },
		{ NULL, NULL, 0 }
	};
	static const gchar *mime_types[] = {
		"image/x-icon",
		"image/x-ico",
		"image/x-win-bitmap",
                "image/vnd.microsoft.icon",
                "application/ico",
                "image/ico",
                "image/icon",
                "text/ico",
		NULL
	};
	static const gchar *extensions[] = {
		"ico",
		"cur",
		NULL
	};

	info->name = "ico";
	info->signature = (GdkPixbufModulePattern *) signature;
	info->description = NC_("image format", "Windows icon");
	info->mime_types = (gchar **) mime_types;
	info->extensions = (gchar **) extensions;
	info->flags = GDK_PIXBUF_FORMAT_WRITABLE | GDK_PIXBUF_FORMAT_THREADSAFE;
	info->license = "LGPL";
}