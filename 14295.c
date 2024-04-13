bool WasmBinaryBuilder::maybeVisitSIMDLoad(Expression*& out, uint32_t code) {
  if (code == BinaryConsts::V128Load) {
    auto* curr = allocator.alloc<Load>();
    curr->type = Type::v128;
    curr->bytes = 16;
    readMemoryAccess(curr->align, curr->offset);
    curr->isAtomic = false;
    curr->ptr = popNonVoidExpression();
    curr->finalize();
    out = curr;
    return true;
  }
  SIMDLoad* curr;
  switch (code) {
    case BinaryConsts::V128Load8Splat:
      curr = allocator.alloc<SIMDLoad>();
      curr->op = Load8SplatVec128;
      break;
    case BinaryConsts::V128Load16Splat:
      curr = allocator.alloc<SIMDLoad>();
      curr->op = Load16SplatVec128;
      break;
    case BinaryConsts::V128Load32Splat:
      curr = allocator.alloc<SIMDLoad>();
      curr->op = Load32SplatVec128;
      break;
    case BinaryConsts::V128Load64Splat:
      curr = allocator.alloc<SIMDLoad>();
      curr->op = Load64SplatVec128;
      break;
    case BinaryConsts::V128Load8x8S:
      curr = allocator.alloc<SIMDLoad>();
      curr->op = Load8x8SVec128;
      break;
    case BinaryConsts::V128Load8x8U:
      curr = allocator.alloc<SIMDLoad>();
      curr->op = Load8x8UVec128;
      break;
    case BinaryConsts::V128Load16x4S:
      curr = allocator.alloc<SIMDLoad>();
      curr->op = Load16x4SVec128;
      break;
    case BinaryConsts::V128Load16x4U:
      curr = allocator.alloc<SIMDLoad>();
      curr->op = Load16x4UVec128;
      break;
    case BinaryConsts::V128Load32x2S:
      curr = allocator.alloc<SIMDLoad>();
      curr->op = Load32x2SVec128;
      break;
    case BinaryConsts::V128Load32x2U:
      curr = allocator.alloc<SIMDLoad>();
      curr->op = Load32x2UVec128;
      break;
    case BinaryConsts::V128Load32Zero:
      curr = allocator.alloc<SIMDLoad>();
      curr->op = Load32ZeroVec128;
      break;
    case BinaryConsts::V128Load64Zero:
      curr = allocator.alloc<SIMDLoad>();
      curr->op = Load64ZeroVec128;
      break;
    default:
      return false;
  }
  readMemoryAccess(curr->align, curr->offset);
  curr->ptr = popNonVoidExpression();
  curr->finalize();
  out = curr;
  return true;
}