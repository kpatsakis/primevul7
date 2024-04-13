MODULE_ENTRY (fill_vtable) (GdkPixbufModule *module)
{
	module->begin_load = gdk_pixbuf__ico_image_begin_load;
	module->stop_load = gdk_pixbuf__ico_image_stop_load;
	module->load_increment = gdk_pixbuf__ico_image_load_increment;
        module->save = gdk_pixbuf__ico_image_save;
        module->is_save_option_supported = gdk_pixbuf__ico_is_save_option_supported;
}