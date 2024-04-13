static void delete_obj(RBinJavaCPTypeObj *obj) {
	if (obj && obj->metas && obj->metas->type_info) {
		RBinJavaCPTypeMetas *ti = obj->metas->type_info;
		if (ti && ti->allocs && ti->allocs->delete_obj) {
			ti->allocs->delete_obj (obj);
		}
	}
}