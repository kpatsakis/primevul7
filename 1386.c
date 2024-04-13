bool ConstantFolding::ConstantPushDown(GraphProperties* properties,
                                       GraphDef* optimized_graph,
                                       NodeDef* node) {
  // Consider the transformation
  //
  //                      +                +       = parent
  //                     / \              / \
  //                    C   +    -- >    X   +     = children
  //                       / \              / \
  //                      X   Y            C   Y   = leaves
  //
  // where C is constant, X is non-constant, Y may be constant or non-constant,
  // and '+' denotes an associative and commutative operator like addition or
  // multiplication. This optimization pushes constants down in the tree to
  // canonicalize it. Moreover, in cases where the child node has a second
  // constant input Y we will create a leaf node that can be folded, e.g.
  //
  //    Add(C1, Add(C2, X)) -> Add(X, Add(C1, C2)) -> Add(X, C1 + C2)
  //
  // We also handle the non-commutative cases of subtraction and division
  // by rotating the tree locally, e.g.
  //    Sub(C, Add(X, Y)) -> Sub(Sub(C, Y), X)
  //    Mul(C, Div(X, Y)) -> Mul(X, Div(C, Y)).

  // Get parent op type.
  const bool is_add = IsAdd(*node);
  const bool is_mul = IsMul(*node);
  const bool is_sub = IsSub(*node);
  const bool is_div = IsDiv(*node);
  if (!(is_add || is_sub || is_mul || is_div)) return false;
  const bool is_symmetric = is_add || is_mul;

  ConstantPushDownContext ctx;
  if (!PrepareConstantPushDown(*node, *properties,
                               /*must_have_properties=*/false, &ctx)) {
    return false;
  }

  // Get child op type.
  const bool is_child_add = IsAdd(*ctx.op_child);
  const bool is_child_mul = IsMul(*ctx.op_child);
  const bool is_child_sub = IsSub(*ctx.op_child);
  const bool is_child_div = IsDiv(*ctx.op_child);
  const bool is_add_sub = (is_add || is_sub) && (is_child_add || is_child_sub);
  const bool is_mul_div = (is_mul || is_div) && (is_child_mul || is_child_div);
  if (!is_add_sub && !is_mul_div) {
    return false;
  }
  const bool is_child_symmetric = is_child_add || is_child_mul;

  if (!CheckAttrExists(*node, "T").ok()) return false;
  DataType dtype = node->attr().at("T").type();
  if (!(is_symmetric && is_child_symmetric) &&
      !(DataTypeIsFloating(dtype) || DataTypeIsComplex(dtype))) {
    return false;
  }

  const NodeDef* y_node =
      ctx.left_leaf_is_const ? ctx.left_leaf : ctx.right_leaf;
  if (!IsReallyConstant(*y_node) && !ctx.parent_input_props->empty() &&
      !ctx.op_child_input_props->empty()) {
    // If we know the shapes of the nodes being swapped, make sure we don't push
    // down a larger node and create more work by broadcasting earlier in the
    // expressions tree.
    const PartialTensorShape c_shape(
        (*ctx.parent_input_props)[ctx.left_child_is_const ? 0 : 1].shape());
    const PartialTensorShape x_shape(
        (*ctx.op_child_input_props)[ctx.left_leaf_is_const ? 0 : 1].shape());

    if (c_shape.IsFullyDefined() && x_shape.IsFullyDefined() &&
        c_shape.num_elements() > x_shape.num_elements()) {
      return false;
    } else if (!c_shape.unknown_rank() && !x_shape.unknown_rank() &&
               c_shape.dims() > 0) {
      for (int idx = 0; idx < std::min(x_shape.dims(), c_shape.dims()); ++idx) {
        if (x_shape.dim_size(idx) >= 0 &&
            c_shape.dim_size(idx) > x_shape.dim_size(idx)) {
          return false;
        }
      }
    }
  }

  // Get the node names corresponding to X, Y, and C.
  const string input_x =
      ctx.left_leaf_is_const ? ctx.op_child->input(1) : ctx.op_child->input(0);
  const string input_y = input_x == ctx.op_child->input(0)
                             ? ctx.op_child->input(1)
                             : ctx.op_child->input(0);
  const string input_c =
      ctx.left_child_is_const ? node->input(0) : node->input(1);
  const string input_op =
      ctx.left_child_is_const ? node->input(1) : node->input(0);
  VLOG(1) << "input_c = " << input_c << "\ninput_x = " << input_x;

  // Now we have identified the nodes to swap, update the nodemap accordingly.
  node_map_->UpdateInput(node->name(), input_c, input_x);
  node_map_->AddOutput(input_c, ctx.op_child->name());
  if (input_x != input_y) {
    node_map_->RemoveOutput(input_x, ctx.op_child->name());
  }
  properties->ClearInputProperties(node->name());
  properties->ClearInputProperties(ctx.op_child->name());

  if (is_symmetric && is_child_symmetric) {
    // Easy case (only commutative ops). We always write this as one of
    //   +
    //  / \
    // X   +
    //    / \
    //   C   Y
    node->set_input(0, input_x);
    node->set_input(1, input_op);
    ctx.op_child->set_input(0, input_c);
    ctx.op_child->set_input(1, input_y);
  } else {
    // More complicated case: When there are non-commutative operations like
    // subtractions or divisions involved, we may have to rotate the tree
    // and/or change op types. There are 6 non-trivial cases depending on
    // the effective generalized "sign" of each of the three terms C, Y, and X.
    // Here are the final trees we want to generate for those 6 cases:
    //
    // (CYX signs):   ++-      +--      -+-    --+     +-+      -++
    //
    //                 -        -        -      -       +        +
    //                / \      / \      / \    / \     / \      / \
    //               +   X    -   X    -   X  X   +   X   -    X   -
    //              / \      / \      / \        / \     / \      / \
    //             C   Y    C   Y    Y   C      Y   C   C   Y    Y   C
    //

    // First, let's determine the effective sign of each term in the original
    // expression
    auto is_leaf_negated = [&](const bool is_right_leaf) -> bool {
      bool leaf_negated = !is_child_symmetric && is_right_leaf;
      bool child_negated = !is_symmetric && (ctx.left_child_is_const);
      return leaf_negated != child_negated;
    };
    const string symmetric_op = (is_add || is_sub) ? "Add" : "Mul";
    const string nonsymmetric_op = (is_add || is_sub) ? "Sub" : "Div";
    bool neg_c = !is_symmetric && !ctx.left_child_is_const;
    bool neg_x = is_leaf_negated(ctx.left_leaf_is_const);
    bool neg_y = is_leaf_negated(!ctx.left_leaf_is_const);
    // Rewrite the parent node.
    node->set_op((neg_x || (neg_c && neg_y)) ? nonsymmetric_op : symmetric_op);
    node->set_input(0, neg_x ? input_op : input_x);
    node->set_input(1, neg_x ? input_x : input_op);
    // Rewrite the child node.
    ctx.op_child->set_op(neg_c != neg_y ? nonsymmetric_op : symmetric_op);
    ctx.op_child->set_input(0, neg_c ? input_y : input_c);
    ctx.op_child->set_input(1, neg_c ? input_c : input_y);
  }
  return true;
}