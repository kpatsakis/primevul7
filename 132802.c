call_smi_watchers(int i, struct device *dev)
{
	struct ipmi_smi_watcher *w;

	mutex_lock(&smi_watchers_mutex);
	list_for_each_entry(w, &smi_watchers, link) {
		if (try_module_get(w->owner)) {
			w->new_smi(i, dev);
			module_put(w->owner);
		}
	}
	mutex_unlock(&smi_watchers_mutex);
}