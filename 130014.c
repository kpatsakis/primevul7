find_property_index (MonoClass *klass, MonoProperty *property) {
	int i;

	for (i = 0; i < klass->ext->property.count; ++i) {
		if (property == &klass->ext->properties [i])
			return klass->ext->property.first + 1 + i;
	}
	return 0;
}