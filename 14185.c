bool WasmBinaryBuilder::maybeVisitSIMDUnary(Expression*& out, uint32_t code) {
  Unary* curr;
  switch (code) {
    case BinaryConsts::I8x16Splat:
      curr = allocator.alloc<Unary>();
      curr->op = SplatVecI8x16;
      break;
    case BinaryConsts::I16x8Splat:
      curr = allocator.alloc<Unary>();
      curr->op = SplatVecI16x8;
      break;
    case BinaryConsts::I32x4Splat:
      curr = allocator.alloc<Unary>();
      curr->op = SplatVecI32x4;
      break;
    case BinaryConsts::I64x2Splat:
      curr = allocator.alloc<Unary>();
      curr->op = SplatVecI64x2;
      break;
    case BinaryConsts::F32x4Splat:
      curr = allocator.alloc<Unary>();
      curr->op = SplatVecF32x4;
      break;
    case BinaryConsts::F64x2Splat:
      curr = allocator.alloc<Unary>();
      curr->op = SplatVecF64x2;
      break;
    case BinaryConsts::V128Not:
      curr = allocator.alloc<Unary>();
      curr->op = NotVec128;
      break;
    case BinaryConsts::V128AnyTrue:
      curr = allocator.alloc<Unary>();
      curr->op = AnyTrueVec128;
      break;
    case BinaryConsts::I8x16Popcnt:
      curr = allocator.alloc<Unary>();
      curr->op = PopcntVecI8x16;
      break;
    case BinaryConsts::I8x16Abs:
      curr = allocator.alloc<Unary>();
      curr->op = AbsVecI8x16;
      break;
    case BinaryConsts::I8x16Neg:
      curr = allocator.alloc<Unary>();
      curr->op = NegVecI8x16;
      break;
    case BinaryConsts::I8x16AllTrue:
      curr = allocator.alloc<Unary>();
      curr->op = AllTrueVecI8x16;
      break;
    case BinaryConsts::I8x16Bitmask:
      curr = allocator.alloc<Unary>();
      curr->op = BitmaskVecI8x16;
      break;
    case BinaryConsts::I16x8Abs:
      curr = allocator.alloc<Unary>();
      curr->op = AbsVecI16x8;
      break;
    case BinaryConsts::I16x8Neg:
      curr = allocator.alloc<Unary>();
      curr->op = NegVecI16x8;
      break;
    case BinaryConsts::I16x8AllTrue:
      curr = allocator.alloc<Unary>();
      curr->op = AllTrueVecI16x8;
      break;
    case BinaryConsts::I16x8Bitmask:
      curr = allocator.alloc<Unary>();
      curr->op = BitmaskVecI16x8;
      break;
    case BinaryConsts::I32x4Abs:
      curr = allocator.alloc<Unary>();
      curr->op = AbsVecI32x4;
      break;
    case BinaryConsts::I32x4Neg:
      curr = allocator.alloc<Unary>();
      curr->op = NegVecI32x4;
      break;
    case BinaryConsts::I32x4AllTrue:
      curr = allocator.alloc<Unary>();
      curr->op = AllTrueVecI32x4;
      break;
    case BinaryConsts::I32x4Bitmask:
      curr = allocator.alloc<Unary>();
      curr->op = BitmaskVecI32x4;
      break;
    case BinaryConsts::I64x2Abs:
      curr = allocator.alloc<Unary>();
      curr->op = AbsVecI64x2;
      break;
    case BinaryConsts::I64x2Neg:
      curr = allocator.alloc<Unary>();
      curr->op = NegVecI64x2;
      break;
    case BinaryConsts::I64x2AllTrue:
      curr = allocator.alloc<Unary>();
      curr->op = AllTrueVecI64x2;
      break;
    case BinaryConsts::I64x2Bitmask:
      curr = allocator.alloc<Unary>();
      curr->op = BitmaskVecI64x2;
      break;
    case BinaryConsts::F32x4Abs:
      curr = allocator.alloc<Unary>();
      curr->op = AbsVecF32x4;
      break;
    case BinaryConsts::F32x4Neg:
      curr = allocator.alloc<Unary>();
      curr->op = NegVecF32x4;
      break;
    case BinaryConsts::F32x4Sqrt:
      curr = allocator.alloc<Unary>();
      curr->op = SqrtVecF32x4;
      break;
    case BinaryConsts::F32x4Ceil:
      curr = allocator.alloc<Unary>();
      curr->op = CeilVecF32x4;
      break;
    case BinaryConsts::F32x4Floor:
      curr = allocator.alloc<Unary>();
      curr->op = FloorVecF32x4;
      break;
    case BinaryConsts::F32x4Trunc:
      curr = allocator.alloc<Unary>();
      curr->op = TruncVecF32x4;
      break;
    case BinaryConsts::F32x4Nearest:
      curr = allocator.alloc<Unary>();
      curr->op = NearestVecF32x4;
      break;
    case BinaryConsts::F64x2Abs:
      curr = allocator.alloc<Unary>();
      curr->op = AbsVecF64x2;
      break;
    case BinaryConsts::F64x2Neg:
      curr = allocator.alloc<Unary>();
      curr->op = NegVecF64x2;
      break;
    case BinaryConsts::F64x2Sqrt:
      curr = allocator.alloc<Unary>();
      curr->op = SqrtVecF64x2;
      break;
    case BinaryConsts::F64x2Ceil:
      curr = allocator.alloc<Unary>();
      curr->op = CeilVecF64x2;
      break;
    case BinaryConsts::F64x2Floor:
      curr = allocator.alloc<Unary>();
      curr->op = FloorVecF64x2;
      break;
    case BinaryConsts::F64x2Trunc:
      curr = allocator.alloc<Unary>();
      curr->op = TruncVecF64x2;
      break;
    case BinaryConsts::F64x2Nearest:
      curr = allocator.alloc<Unary>();
      curr->op = NearestVecF64x2;
      break;
    case BinaryConsts::I16x8ExtaddPairwiseI8x16S:
      curr = allocator.alloc<Unary>();
      curr->op = ExtAddPairwiseSVecI8x16ToI16x8;
      break;
    case BinaryConsts::I16x8ExtaddPairwiseI8x16U:
      curr = allocator.alloc<Unary>();
      curr->op = ExtAddPairwiseUVecI8x16ToI16x8;
      break;
    case BinaryConsts::I32x4ExtaddPairwiseI16x8S:
      curr = allocator.alloc<Unary>();
      curr->op = ExtAddPairwiseSVecI16x8ToI32x4;
      break;
    case BinaryConsts::I32x4ExtaddPairwiseI16x8U:
      curr = allocator.alloc<Unary>();
      curr->op = ExtAddPairwiseUVecI16x8ToI32x4;
      break;
    case BinaryConsts::I32x4TruncSatF32x4S:
      curr = allocator.alloc<Unary>();
      curr->op = TruncSatSVecF32x4ToVecI32x4;
      break;
    case BinaryConsts::I32x4TruncSatF32x4U:
      curr = allocator.alloc<Unary>();
      curr->op = TruncSatUVecF32x4ToVecI32x4;
      break;
    case BinaryConsts::F32x4ConvertI32x4S:
      curr = allocator.alloc<Unary>();
      curr->op = ConvertSVecI32x4ToVecF32x4;
      break;
    case BinaryConsts::F32x4ConvertI32x4U:
      curr = allocator.alloc<Unary>();
      curr->op = ConvertUVecI32x4ToVecF32x4;
      break;
    case BinaryConsts::I16x8ExtendLowI8x16S:
      curr = allocator.alloc<Unary>();
      curr->op = ExtendLowSVecI8x16ToVecI16x8;
      break;
    case BinaryConsts::I16x8ExtendHighI8x16S:
      curr = allocator.alloc<Unary>();
      curr->op = ExtendHighSVecI8x16ToVecI16x8;
      break;
    case BinaryConsts::I16x8ExtendLowI8x16U:
      curr = allocator.alloc<Unary>();
      curr->op = ExtendLowUVecI8x16ToVecI16x8;
      break;
    case BinaryConsts::I16x8ExtendHighI8x16U:
      curr = allocator.alloc<Unary>();
      curr->op = ExtendHighUVecI8x16ToVecI16x8;
      break;
    case BinaryConsts::I32x4ExtendLowI16x8S:
      curr = allocator.alloc<Unary>();
      curr->op = ExtendLowSVecI16x8ToVecI32x4;
      break;
    case BinaryConsts::I32x4ExtendHighI16x8S:
      curr = allocator.alloc<Unary>();
      curr->op = ExtendHighSVecI16x8ToVecI32x4;
      break;
    case BinaryConsts::I32x4ExtendLowI16x8U:
      curr = allocator.alloc<Unary>();
      curr->op = ExtendLowUVecI16x8ToVecI32x4;
      break;
    case BinaryConsts::I32x4ExtendHighI16x8U:
      curr = allocator.alloc<Unary>();
      curr->op = ExtendHighUVecI16x8ToVecI32x4;
      break;
    case BinaryConsts::I64x2ExtendLowI32x4S:
      curr = allocator.alloc<Unary>();
      curr->op = ExtendLowSVecI32x4ToVecI64x2;
      break;
    case BinaryConsts::I64x2ExtendHighI32x4S:
      curr = allocator.alloc<Unary>();
      curr->op = ExtendHighSVecI32x4ToVecI64x2;
      break;
    case BinaryConsts::I64x2ExtendLowI32x4U:
      curr = allocator.alloc<Unary>();
      curr->op = ExtendLowUVecI32x4ToVecI64x2;
      break;
    case BinaryConsts::I64x2ExtendHighI32x4U:
      curr = allocator.alloc<Unary>();
      curr->op = ExtendHighUVecI32x4ToVecI64x2;
      break;
    case BinaryConsts::F64x2ConvertLowI32x4S:
      curr = allocator.alloc<Unary>();
      curr->op = ConvertLowSVecI32x4ToVecF64x2;
      break;
    case BinaryConsts::F64x2ConvertLowI32x4U:
      curr = allocator.alloc<Unary>();
      curr->op = ConvertLowUVecI32x4ToVecF64x2;
      break;
    case BinaryConsts::I32x4TruncSatF64x2SZero:
      curr = allocator.alloc<Unary>();
      curr->op = TruncSatZeroSVecF64x2ToVecI32x4;
      break;
    case BinaryConsts::I32x4TruncSatF64x2UZero:
      curr = allocator.alloc<Unary>();
      curr->op = TruncSatZeroUVecF64x2ToVecI32x4;
      break;
    case BinaryConsts::F32x4DemoteF64x2Zero:
      curr = allocator.alloc<Unary>();
      curr->op = DemoteZeroVecF64x2ToVecF32x4;
      break;
    case BinaryConsts::F64x2PromoteLowF32x4:
      curr = allocator.alloc<Unary>();
      curr->op = PromoteLowVecF32x4ToVecF64x2;
      break;
    case BinaryConsts::I32x4RelaxedTruncF32x4S:
      curr = allocator.alloc<Unary>();
      curr->op = RelaxedTruncSVecF32x4ToVecI32x4;
      break;
    case BinaryConsts::I32x4RelaxedTruncF32x4U:
      curr = allocator.alloc<Unary>();
      curr->op = RelaxedTruncUVecF32x4ToVecI32x4;
      break;
    case BinaryConsts::I32x4RelaxedTruncF64x2SZero:
      curr = allocator.alloc<Unary>();
      curr->op = RelaxedTruncZeroSVecF64x2ToVecI32x4;
      break;
    case BinaryConsts::I32x4RelaxedTruncF64x2UZero:
      curr = allocator.alloc<Unary>();
      curr->op = RelaxedTruncZeroUVecF64x2ToVecI32x4;
      break;
    default:
      return false;
  }
  curr->value = popNonVoidExpression();
  curr->finalize();
  out = curr;
  return true;
}