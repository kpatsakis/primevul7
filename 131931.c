bool LIRGenerator::can_inline_as_constant(LIR_Const* c) const {
  if (c->type() == T_LONG) return false;
  return c->type() != T_OBJECT || c->as_jobject() == NULL;
}