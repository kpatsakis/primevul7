static void
typebuilder_setup_events (MonoClass *klass, MonoError *error)
{
	MonoReflectionTypeBuilder *tb = mono_class_get_ref_info (klass);
	MonoReflectionEventBuilder *eb;
	MonoImage *image = klass->image;
	MonoEvent *events;
	int i;

	mono_error_init (error);

	if (!klass->ext)
		klass->ext = image_g_new0 (image, MonoClassExt, 1);

	klass->ext->event.count = tb->events ? mono_array_length (tb->events) : 0;
	klass->ext->event.first = 0;

	events = image_g_new0 (image, MonoEvent, klass->ext->event.count);
	klass->ext->events = events;
	for (i = 0; i < klass->ext->event.count; ++i) {
		eb = mono_array_get (tb->events, MonoReflectionEventBuilder*, i);
		events [i].parent = klass;
		events [i].attrs = eb->attrs;
		events [i].name = mono_string_to_utf8_image (image, eb->name, error);
		if (!mono_error_ok (error))
			return;
		if (eb->add_method)
			events [i].add = eb->add_method->mhandle;
		if (eb->remove_method)
			events [i].remove = eb->remove_method->mhandle;
		if (eb->raise_method)
			events [i].raise = eb->raise_method->mhandle;

#ifndef MONO_SMALL_CONFIG
		if (eb->other_methods) {
			int j;
			events [i].other = image_g_new0 (image, MonoMethod*, mono_array_length (eb->other_methods) + 1);
			for (j = 0; j < mono_array_length (eb->other_methods); ++j) {
				MonoReflectionMethodBuilder *mb = 
					mono_array_get (eb->other_methods,
									MonoReflectionMethodBuilder*, j);
				events [i].other [j] = mb->mhandle;
			}
		}
#endif
		mono_save_custom_attrs (klass->image, &events [i], eb->cattrs);
	}