bool WasmBinaryBuilder::maybeVisitSIMDTernary(Expression*& out, uint32_t code) {
  SIMDTernary* curr;
  switch (code) {
    case BinaryConsts::V128Bitselect:
      curr = allocator.alloc<SIMDTernary>();
      curr->op = Bitselect;
      break;
    case BinaryConsts::I8x16Laneselect:
      curr = allocator.alloc<SIMDTernary>();
      curr->op = LaneselectI8x16;
      break;
    case BinaryConsts::I16x8Laneselect:
      curr = allocator.alloc<SIMDTernary>();
      curr->op = LaneselectI16x8;
      break;
    case BinaryConsts::I32x4Laneselect:
      curr = allocator.alloc<SIMDTernary>();
      curr->op = LaneselectI32x4;
      break;
    case BinaryConsts::I64x2Laneselect:
      curr = allocator.alloc<SIMDTernary>();
      curr->op = LaneselectI64x2;
      break;
    case BinaryConsts::F32x4RelaxedFma:
      curr = allocator.alloc<SIMDTernary>();
      curr->op = RelaxedFmaVecF32x4;
      break;
    case BinaryConsts::F32x4RelaxedFms:
      curr = allocator.alloc<SIMDTernary>();
      curr->op = RelaxedFmsVecF32x4;
      break;
    case BinaryConsts::F64x2RelaxedFma:
      curr = allocator.alloc<SIMDTernary>();
      curr->op = RelaxedFmaVecF64x2;
      break;
    case BinaryConsts::F64x2RelaxedFms:
      curr = allocator.alloc<SIMDTernary>();
      curr->op = RelaxedFmsVecF64x2;
      break;
    default:
      return false;
  }
  curr->c = popNonVoidExpression();
  curr->b = popNonVoidExpression();
  curr->a = popNonVoidExpression();
  curr->finalize();
  out = curr;
  return true;
}