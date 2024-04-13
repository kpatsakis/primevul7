Status ConstantFolding::SimplifyArithmeticOperations(
    const GraphProperties& properties, bool use_shape_info,
    GraphDef* optimized_graph, NodeDef* node) {
  const bool is_mul = IsAnyMul(*node) || IsLogicalAnd(*node);
  const bool is_matmul = IsAnyMatMul(*node);
  const bool is_add = IsAdd(*node) || IsBiasAdd(*node) || IsLogicalOr(*node);
  const bool is_sub = IsSub(*node);
  const bool is_any_div = IsAnyDiv(*node) && !IsFloorDiv(*node);
  // Simplify arithmetic operations with ones or zeros.
  if (use_shape_info &&
      (is_mul || is_matmul || is_add || is_sub || is_any_div) &&
      properties.HasInputProperties(node->name()) &&
      properties.HasOutputProperties(node->name())) {
    const NodeDef* x = node_map_->GetNode(node->input(0));
    const NodeDef* y = node_map_->GetNode(node->input(1));
    if (x == nullptr || y == nullptr) {
      return errors::InvalidArgument("Invalid inputs to node: ",
                                     node->DebugString());
    }
    const TensorShapeProto& output_shape =
        properties.GetOutputProperties(node->name())[0].shape();

    // Simplify element-wise multiplication by ones or addition/subtraction
    // of zeros.
    const TensorShapeProto& y_shape =
        properties.GetInputProperties(node->name())[1].shape();
    const TensorShapeProto& x_shape =
        properties.GetInputProperties(node->name())[0].shape();
    const bool y_matches_output_shape =
        ShapesSymbolicallyEqual(output_shape, y_shape);
    const bool x_matches_output_shape =
        ShapesSymbolicallyEqual(output_shape, x_shape);

    const bool x_is_zero = IsZeros(*x);
    const bool x_is_one = x_is_zero ? false : IsOnes(*x);
    if ((is_mul && x_is_one) || (is_add && x_is_zero)) {
      // 1 * y = y or 0 + y = y.
      if (y_matches_output_shape) {
        ReplaceOperationWithSnapshot(1, properties, node, optimized_graph);
      } else if (x_matches_output_shape) {
        ReplaceBinaryOperationWithBroadcastTo(1, properties, node,
                                              optimized_graph);
      }
      return Status::OK();
    }

    if (y_matches_output_shape && (is_sub && x_is_zero)) {
      // Replace 0 - y with Neg(y).
      ReplaceSubtractionFromZeroByNegation(node, optimized_graph);
      return Status::OK();
    }

    // Replace 1 / y with Reciprocal op.
    if (y_matches_output_shape && is_any_div && x_is_one) {
      TF_RETURN_IF_ERROR(CheckAttrExists(*node, "T"));
      DataType type = node->attr().at("T").type();
      if (DataTypeIsFloating(type) || DataTypeIsComplex(type)) {
        ReplaceDivisionOfOnesByReciprocal(node, optimized_graph);
        return Status::OK();
      }
    }

    const bool y_is_zero = IsZeros(*y);
    const bool y_is_one = y_is_zero ? false : IsOnes(*y);
    if (((is_mul || is_any_div) && y_is_one) ||
        ((is_add || is_sub) && y_is_zero)) {
      // x * 1 = x or x / 1 = x or x +/- 0 = x
      if (x_matches_output_shape) {
        ReplaceOperationWithSnapshot(0, properties, node, optimized_graph);
      } else if (y_matches_output_shape) {
        ReplaceBinaryOperationWithBroadcastTo(0, properties, node,
                                              optimized_graph);
      }
      return Status::OK();
    }

    // x OR true = true OR y = true.
    const PartialTensorShape shp(output_shape);
    if (shp.IsFullyDefined() && IsLogicalOr(*node) && (y_is_one || x_is_one)) {
      TF_RETURN_IF_ERROR(ReplaceOperationWithConstant(
          1, properties, output_shape, node, optimized_graph));
      return Status::OK();
    }

    // Simplify multiplication and matmul by zeros.
    // Also optimize zeros divided by a tensor, but only if we are in
    // aggressive mode, since we might get rid of divisions by zero.
    const bool is_aggressive = opt_level_ == RewriterConfig::AGGRESSIVE;
    bool optimize_zeros_divided_by_y = is_any_div && x_is_zero && is_aggressive;
    if ((x_is_zero || y_is_zero) &&
        (is_mul || is_matmul || optimize_zeros_divided_by_y)) {
      if (shp.IsFullyDefined()) {
        bool is_quantized = IsQuantizedMatMul(*node);
        TF_RETURN_IF_ERROR(ReplaceOperationWithConstant(
            0, properties, output_shape, node, optimized_graph));
        if (is_quantized && graph_modified_) {
          TF_RETURN_IF_ERROR(
              AddQuantizedMatMulMinMaxOutConstNodes(node, optimized_graph));
        }
        return Status::OK();
      }
      // Even if an input shape is only partially known, we may known that it
      // matches the output shape and thus forward or broadcast the
      // corresponding zero input.
      if ((is_mul || is_any_div) && x_is_zero) {
        if (x_matches_output_shape) {
          ReplaceOperationWithIdentity(0, properties, node, optimized_graph);
        } else if (y_matches_output_shape) {
          ReplaceBinaryOperationWithBroadcastTo(0, properties, node,
                                                optimized_graph);
        }
        return Status::OK();
      } else if (is_mul && y_is_zero) {
        if (y_matches_output_shape) {
          ReplaceOperationWithIdentity(1, properties, node, optimized_graph);
        } else if (x_matches_output_shape) {
          ReplaceBinaryOperationWithBroadcastTo(1, properties, node,
                                                optimized_graph);
        }
        return Status::OK();
      }
    }
  }
  return Status::OK();
}