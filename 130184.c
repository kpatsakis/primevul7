mono_event_get_object (MonoDomain *domain, MonoClass *klass, MonoEvent *event)
{
	MonoReflectionEvent *res;
	MonoReflectionMonoEvent *mono_event;
	static MonoClass *monoevent_klass;

	CHECK_OBJECT (MonoReflectionEvent *, event, klass);
	if (!monoevent_klass)
		monoevent_klass = mono_class_from_name (mono_defaults.corlib, "System.Reflection", "MonoEvent");
	mono_event = (MonoReflectionMonoEvent *)mono_object_new (domain, monoevent_klass);
	mono_event->klass = klass;
	mono_event->event = event;
	res = (MonoReflectionEvent*)mono_event;
	CACHE_OBJECT (MonoReflectionEvent *, event, res, klass);
}