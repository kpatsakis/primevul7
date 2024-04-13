bool LIRGenerator::can_inline_as_constant(Value v) const {
  if (v->type()->tag() == longTag) return false;
  return v->type()->tag() != objectTag ||
    (v->type()->is_constant() && v->type()->as_ObjectType()->constant_value()->is_null_object());
}