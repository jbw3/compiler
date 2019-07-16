; ModuleID = 'module'
source_filename = "module"

define i32 @noArgs() {
entry:
  ret i32 42
}

define i32 @oneArg(i32 %x) {
entry:
  %x1 = alloca i32
  store i32 %x, i32* %x1
  %x2 = load i32, i32* %x1
  %add = add i32 %x2, 2
  ret i32 %add
}

define i32 @integerLiterals(i32 %x) {
entry:
  %x1 = alloca i32
  store i32 %x, i32* %x1
  %x2 = load i32, i32* %x1
  %add = add i32 5, %x2
  %add3 = add i32 %add, 248
  %x4 = load i32, i32* %x1
  %add5 = add i32 %add3, %x4
  %add6 = add i32 %add5, 123
  %x7 = load i32, i32* %x1
  %add8 = add i32 %add6, %x7
  %add9 = add i32 %add8, 10846099
  ret i32 %add9
}

define i32 @twoArgs(i32 %arg1, i32 %arg2) {
entry:
  %arg22 = alloca i32
  %arg11 = alloca i32
  store i32 %arg1, i32* %arg11
  store i32 %arg2, i32* %arg22
  %arg13 = load i32, i32* %arg11
  %arg24 = load i32, i32* %arg22
  %sub = sub i32 %arg13, %arg24
  ret i32 %sub
}

define i32 @opOrder1(i32 %a, i32 %b, i32 %c, i32 %d) {
entry:
  %d4 = alloca i32
  %c3 = alloca i32
  %b2 = alloca i32
  %a1 = alloca i32
  store i32 %a, i32* %a1
  store i32 %b, i32* %b2
  store i32 %c, i32* %c3
  store i32 %d, i32* %d4
  %a5 = load i32, i32* %a1
  %b6 = load i32, i32* %b2
  %c7 = load i32, i32* %c3
  %mul = mul i32 %b6, %c7
  %add = add i32 %a5, %mul
  %d8 = load i32, i32* %d4
  %sub = sub i32 %add, %d8
  ret i32 %sub
}

define i32 @opOrder2(i32 %a, i32 %b, i32 %c, i32 %d) {
entry:
  %d4 = alloca i32
  %c3 = alloca i32
  %b2 = alloca i32
  %a1 = alloca i32
  store i32 %a, i32* %a1
  store i32 %b, i32* %b2
  store i32 %c, i32* %c3
  store i32 %d, i32* %d4
  %a5 = load i32, i32* %a1
  %b6 = load i32, i32* %b2
  %c7 = load i32, i32* %c3
  %mul = mul i32 %b6, %c7
  %add = add i32 %a5, %mul
  %d8 = load i32, i32* %d4
  %a9 = load i32, i32* %a1
  %div = sdiv i32 %d8, %a9
  %sub = sub i32 %add, %div
  %b10 = load i32, i32* %b2
  %mod = srem i32 %b10, 42
  %add11 = add i32 %sub, %mod
  ret i32 %add11
}

define i1 @opOrder3(i32 %a, i32 %b, i32 %c, i32 %d) {
entry:
  %d4 = alloca i32
  %c3 = alloca i32
  %b2 = alloca i32
  %a1 = alloca i32
  store i32 %a, i32* %a1
  store i32 %b, i32* %b2
  store i32 %c, i32* %c3
  store i32 %d, i32* %d4
  %a5 = load i32, i32* %a1
  %b6 = load i32, i32* %b2
  %c7 = load i32, i32* %c3
  %d8 = load i32, i32* %d4
  %mul = mul i32 %c7, %d8
  %add = add i32 %b6, %mul
  %cmpeq = icmp eq i32 %a5, %add
  ret i1 %cmpeq
}

define i32 @opOrder4(i32 %a, i32 %b, i32 %c, i32 %d, i32 %e, i32 %f) {
entry:
  %f6 = alloca i32
  %e5 = alloca i32
  %d4 = alloca i32
  %c3 = alloca i32
  %b2 = alloca i32
  %a1 = alloca i32
  store i32 %a, i32* %a1
  store i32 %b, i32* %b2
  store i32 %c, i32* %c3
  store i32 %d, i32* %d4
  store i32 %e, i32* %e5
  store i32 %f, i32* %f6
  %a7 = load i32, i32* %a1
  %b8 = load i32, i32* %b2
  %c9 = load i32, i32* %c3
  %d10 = load i32, i32* %d4
  %e11 = load i32, i32* %e5
  %shl = shl i32 %d10, %e11
  %f12 = load i32, i32* %f6
  %ashr = ashr i32 %shl, %f12
  %bitand = and i32 %c9, %ashr
  %bitxor = xor i32 %b8, %bitand
  %bitor = or i32 %a7, %bitxor
  ret i32 %bitor
}

define i32 @opOrder5(i32 %a, i32 %b, i32 %c, i32 %d) {
entry:
  %d4 = alloca i32
  %c3 = alloca i32
  %b2 = alloca i32
  %a1 = alloca i32
  store i32 %a, i32* %a1
  store i32 %b, i32* %b2
  store i32 %c, i32* %c3
  store i32 %d, i32* %d4
  %a5 = load i32, i32* %a1
  %b6 = load i32, i32* %b2
  %add = add i32 %a5, %b6
  %neg = sub i32 0, %add
  %a7 = load i32, i32* %a1
  %b8 = load i32, i32* %b2
  %c9 = load i32, i32* %c3
  %d10 = load i32, i32* %d4
  %call = call i32 @opOrder6(i32 %a7, i32 %b8, i32 %c9, i32 %d10)
  %add11 = add i32 %neg, %call
  %d12 = load i32, i32* %d4
  %mul = mul i32 %add11, %d12
  ret i32 %mul
}

define i32 @opOrder6(i32 %a, i32 %b, i32 %c, i32 %d) {
entry:
  %d4 = alloca i32
  %c3 = alloca i32
  %b2 = alloca i32
  %a1 = alloca i32
  store i32 %a, i32* %a1
  store i32 %b, i32* %b2
  store i32 %c, i32* %c3
  store i32 %d, i32* %d4
  %a5 = load i32, i32* %a1
  %b6 = load i32, i32* %b2
  %bitor = or i32 %a5, %b6
  %c7 = load i32, i32* %c3
  %d8 = load i32, i32* %d4
  %bitand = and i32 %c7, %d8
  %bitxor = xor i32 %bitor, %bitand
  ret i32 %bitxor
}

define i32 @negatives(i32 %a, i32 %b, i32 %c) {
entry:
  %c3 = alloca i32
  %b2 = alloca i32
  %a1 = alloca i32
  store i32 %a, i32* %a1
  store i32 %b, i32* %b2
  store i32 %c, i32* %c3
  %a4 = load i32, i32* %a1
  %neg = sub i32 0, %a4
  %b5 = load i32, i32* %b2
  %neg6 = sub i32 0, %b5
  %c7 = load i32, i32* %c3
  %neg8 = sub i32 0, %c7
  %add = add i32 2, %neg8
  %call = call i32 @oneArg(i32 %add)
  %neg9 = sub i32 0, %call
  %mul = mul i32 %neg6, %neg9
  %sub = sub i32 %neg, %mul
  ret i32 %sub
}

define i32 @not(i32 %a, i32 %b) {
entry:
  %b2 = alloca i32
  %a1 = alloca i32
  store i32 %a, i32* %a1
  store i32 %b, i32* %b2
  %a3 = load i32, i32* %a1
  %not = xor i32 %a3, -1
  %b4 = load i32, i32* %b2
  %not5 = xor i32 %b4, -1
  %bitand = and i32 %not, %not5
  %call = call i32 @noArgs()
  %not6 = xor i32 %call, -1
  %bitor = or i32 %bitand, %not6
  ret i32 %bitor
}

define i1 @equal(i32 %a, i32 %b) {
entry:
  %b2 = alloca i32
  %a1 = alloca i32
  store i32 %a, i32* %a1
  store i32 %b, i32* %b2
  %a3 = load i32, i32* %a1
  %b4 = load i32, i32* %b2
  %cmpeq = icmp eq i32 %a3, %b4
  ret i1 %cmpeq
}

define i1 @notEqual(i32 %a, i32 %b) {
entry:
  %b2 = alloca i32
  %a1 = alloca i32
  store i32 %a, i32* %a1
  store i32 %b, i32* %b2
  %a3 = load i32, i32* %a1
  %b4 = load i32, i32* %b2
  %cmpne = icmp ne i32 %a3, %b4
  ret i1 %cmpne
}

define i1 @lessThan(i32 %a, i32 %b) {
entry:
  %b2 = alloca i32
  %a1 = alloca i32
  store i32 %a, i32* %a1
  store i32 %b, i32* %b2
  %a3 = load i32, i32* %a1
  %b4 = load i32, i32* %b2
  %cmplt = icmp slt i32 %a3, %b4
  ret i1 %cmplt
}

define i1 @lessThanOrEqual(i32 %a, i32 %b) {
entry:
  %b2 = alloca i32
  %a1 = alloca i32
  store i32 %a, i32* %a1
  store i32 %b, i32* %b2
  %a3 = load i32, i32* %a1
  %b4 = load i32, i32* %b2
  %cmple = icmp sle i32 %a3, %b4
  ret i1 %cmple
}

define i1 @greaterThan(i32 %a, i32 %b) {
entry:
  %b2 = alloca i32
  %a1 = alloca i32
  store i32 %a, i32* %a1
  store i32 %b, i32* %b2
  %a3 = load i32, i32* %a1
  %b4 = load i32, i32* %b2
  %cmpgt = icmp sgt i32 %a3, %b4
  ret i1 %cmpgt
}

define i1 @greaterThanOrEqual(i32 %a, i32 %b) {
entry:
  %b2 = alloca i32
  %a1 = alloca i32
  store i32 %a, i32* %a1
  store i32 %b, i32* %b2
  %a3 = load i32, i32* %a1
  %b4 = load i32, i32* %b2
  %cmpge = icmp sge i32 %a3, %b4
  ret i1 %cmpge
}

define i1 @logicalAnd2(i32 %x, i32 %y, i32 %z) {
entry:
  %z3 = alloca i32
  %y2 = alloca i32
  %x1 = alloca i32
  store i32 %x, i32* %x1
  store i32 %y, i32* %y2
  store i32 %z, i32* %z3
  %x4 = load i32, i32* %x1
  %cmpne = icmp ne i32 %x4, 0
  br i1 %cmpne, label %andtrue, label %andfalse

andtrue:                                          ; preds = %entry
  %y5 = load i32, i32* %y2
  %z6 = load i32, i32* %z3
  %add = add i32 %z6, 5
  %cmplt = icmp slt i32 %y5, %add
  br label %andmerge

andfalse:                                         ; preds = %entry
  br label %andmerge

andmerge:                                         ; preds = %andfalse, %andtrue
  %andphi = phi i1 [ %cmplt, %andtrue ], [ false, %andfalse ]
  ret i1 %andphi
}

define i1 @logicalAnd3(i32 %x, i32 %y, i32 %z) {
entry:
  %z3 = alloca i32
  %y2 = alloca i32
  %x1 = alloca i32
  store i32 %x, i32* %x1
  store i32 %y, i32* %y2
  store i32 %z, i32* %z3
  %x4 = load i32, i32* %x1
  %cmpeq = icmp eq i32 %x4, 0
  br i1 %cmpeq, label %andtrue, label %andfalse

andtrue:                                          ; preds = %entry
  %y5 = load i32, i32* %y2
  %cmpeq6 = icmp eq i32 %y5, 1
  br label %andmerge

andfalse:                                         ; preds = %entry
  br label %andmerge

andmerge:                                         ; preds = %andfalse, %andtrue
  %andphi = phi i1 [ %cmpeq6, %andtrue ], [ false, %andfalse ]
  br i1 %andphi, label %andtrue7, label %andfalse10

andtrue7:                                         ; preds = %andmerge
  %z8 = load i32, i32* %z3
  %cmpeq9 = icmp eq i32 %z8, 2
  br label %andmerge11

andfalse10:                                       ; preds = %andmerge
  br label %andmerge11

andmerge11:                                       ; preds = %andfalse10, %andtrue7
  %andphi12 = phi i1 [ %cmpeq9, %andtrue7 ], [ false, %andfalse10 ]
  ret i1 %andphi12
}

define i1 @logicalOr2(i32 %x, i32 %y, i32 %z) {
entry:
  %z3 = alloca i32
  %y2 = alloca i32
  %x1 = alloca i32
  store i32 %x, i32* %x1
  store i32 %y, i32* %y2
  store i32 %z, i32* %z3
  %z4 = load i32, i32* %z3
  %cmpeq = icmp eq i32 %z4, 0
  br i1 %cmpeq, label %ortrue, label %orfalse

ortrue:                                           ; preds = %entry
  br label %ormerge

orfalse:                                          ; preds = %entry
  %x5 = load i32, i32* %x1
  %y6 = load i32, i32* %y2
  %z7 = load i32, i32* %z3
  %div = sdiv i32 %y6, %z7
  %add = add i32 %x5, %div
  %cmplt = icmp slt i32 %add, 100
  br label %ormerge

ormerge:                                          ; preds = %orfalse, %ortrue
  %orphi = phi i1 [ true, %ortrue ], [ %cmplt, %orfalse ]
  ret i1 %orphi
}

define i1 @logicalOr3(i32 %x, i32 %y, i32 %z) {
entry:
  %z3 = alloca i32
  %y2 = alloca i32
  %x1 = alloca i32
  store i32 %x, i32* %x1
  store i32 %y, i32* %y2
  store i32 %z, i32* %z3
  %x4 = load i32, i32* %x1
  %cmpeq = icmp eq i32 %x4, 0
  br i1 %cmpeq, label %ortrue, label %orfalse

ortrue:                                           ; preds = %entry
  br label %ormerge

orfalse:                                          ; preds = %entry
  %y5 = load i32, i32* %y2
  %cmpeq6 = icmp eq i32 %y5, 1
  br label %ormerge

ormerge:                                          ; preds = %orfalse, %ortrue
  %orphi = phi i1 [ true, %ortrue ], [ %cmpeq6, %orfalse ]
  br i1 %orphi, label %ortrue7, label %orfalse8

ortrue7:                                          ; preds = %ormerge
  br label %ormerge11

orfalse8:                                         ; preds = %ormerge
  %z9 = load i32, i32* %z3
  %cmpeq10 = icmp eq i32 %z9, 2
  br label %ormerge11

ormerge11:                                        ; preds = %orfalse8, %ortrue7
  %orphi12 = phi i1 [ true, %ortrue7 ], [ %cmpeq10, %orfalse8 ]
  ret i1 %orphi12
}

define i32 @nestedCall(i32 %x, i32 %y, i32 %z) {
entry:
  %z3 = alloca i32
  %y2 = alloca i32
  %x1 = alloca i32
  store i32 %x, i32* %x1
  store i32 %y, i32* %y2
  store i32 %z, i32* %z3
  %x4 = load i32, i32* %x1
  %call = call i32 @twoArgs(i32 0, i32 %x4)
  %y5 = load i32, i32* %y2
  %add = add i32 %y5, 42
  %z6 = load i32, i32* %z3
  %sub = sub i32 %z6, 57
  %call7 = call i32 @twoArgs(i32 %add, i32 %sub)
  %add8 = add i32 %call, %call7
  ret i32 %add8
}

define i1 @call_function_defined_later(i1 %a, i1 %b) {
entry:
  %b2 = alloca i1
  %a1 = alloca i1
  store i1 %a, i1* %a1
  store i1 %b, i1* %b2
  %a3 = load i1, i1* %a1
  %b4 = load i1, i1* %b2
  %call = call i1 @types_bool(i1 %a3, i1 %b4)
  ret i1 %call
}

define i1 @types_bool(i1 %a, i1 %b) {
entry:
  %b2 = alloca i1
  %a1 = alloca i1
  store i1 %a, i1* %a1
  store i1 %b, i1* %b2
  %a3 = load i1, i1* %a1
  %b4 = load i1, i1* %b2
  %bitor = or i1 %a3, %b4
  ret i1 %bitor
}

define i8 @types_i8(i8 %n1, i8 %n2) {
entry:
  %n22 = alloca i8
  %n11 = alloca i8
  store i8 %n1, i8* %n11
  store i8 %n2, i8* %n22
  %n13 = load i8, i8* %n11
  %n24 = load i8, i8* %n22
  %add = add i8 %n13, %n24
  ret i8 %add
}

define i16 @types_i16(i16 %n1, i16 %n2) {
entry:
  %n22 = alloca i16
  %n11 = alloca i16
  store i16 %n1, i16* %n11
  store i16 %n2, i16* %n22
  %n13 = load i16, i16* %n11
  %n24 = load i16, i16* %n22
  %sub = sub i16 %n13, %n24
  ret i16 %sub
}

define i32 @types_i32(i32 %n1, i32 %n2) {
entry:
  %n22 = alloca i32
  %n11 = alloca i32
  store i32 %n1, i32* %n11
  store i32 %n2, i32* %n22
  %n13 = load i32, i32* %n11
  %n24 = load i32, i32* %n22
  %mul = mul i32 %n13, %n24
  ret i32 %mul
}

define i64 @types_i64(i64 %n1, i64 %n2) {
entry:
  %n22 = alloca i64
  %n11 = alloca i64
  store i64 %n1, i64* %n11
  store i64 %n2, i64* %n22
  %n13 = load i64, i64* %n11
  %n24 = load i64, i64* %n22
  %div = sdiv i64 %n13, %n24
  ret i64 %div
}

define i8 @types_add_literal(i8 %num) {
entry:
  %num1 = alloca i8
  store i8 %num, i8* %num1
  %num2 = load i8, i8* %num1
  %add = add i8 %num2, 1
  ret i8 %add
}

define i64 @sign_extend_bin_op(i8 %a, i16 %b, i64 %c, i32 %d) {
entry:
  %d4 = alloca i32
  %c3 = alloca i64
  %b2 = alloca i16
  %a1 = alloca i8
  store i8 %a, i8* %a1
  store i16 %b, i16* %b2
  store i64 %c, i64* %c3
  store i32 %d, i32* %d4
  %a5 = load i8, i8* %a1
  %b6 = load i16, i16* %b2
  %signext = sext i8 %a5 to i16
  %mul = mul i16 %signext, %b6
  %c7 = load i64, i64* %c3
  %signext8 = sext i16 %mul to i64
  %div = sdiv i64 %signext8, %c7
  %d9 = load i32, i32* %d4
  %signext10 = sext i32 %d9 to i64
  %mod = srem i64 %div, %signext10
  ret i64 %mod
}

define i64 @sign_extend_ret(i32 %param) {
entry:
  %param1 = alloca i32
  store i32 %param, i32* %param1
  %param2 = load i32, i32* %param1
  %add = add i32 %param2, 5
  %signext = sext i32 %add to i64
  ret i64 %signext
}

define i64 @sign_extend_call(i8 %param) {
entry:
  %param1 = alloca i8
  store i8 %param, i8* %param1
  %param2 = load i8, i8* %param1
  %signext = sext i8 %param2 to i32
  %call = call i64 @sign_extend_ret(i32 %signext)
  ret i64 %call
}

define i32 @basicBranch(i32 %x, i32 %y, i32 %z) {
entry:
  %z3 = alloca i32
  %y2 = alloca i32
  %x1 = alloca i32
  store i32 %x, i32* %x1
  store i32 %y, i32* %y2
  store i32 %z, i32* %z3
  %x4 = load i32, i32* %x1
  %add = add i32 %x4, 1
  %y5 = load i32, i32* %y2
  %div = sdiv i32 %y5, 2
  %cmpeq = icmp eq i32 %add, %div
  br i1 %cmpeq, label %if, label %else

if:                                               ; preds = %entry
  %z6 = load i32, i32* %z3
  %mul = mul i32 %z6, 10
  br label %merge

else:                                             ; preds = %entry
  %z7 = load i32, i32* %z3
  %neg = sub i32 0, %z7
  br label %merge

merge:                                            ; preds = %else, %if
  %phi = phi i32 [ %mul, %if ], [ %neg, %else ]
  ret i32 %phi
}

define i64 @signExtendBranch(i1 %b, i16 %x, i16 %y) {
entry:
  %y3 = alloca i16
  %x2 = alloca i16
  %b1 = alloca i1
  store i1 %b, i1* %b1
  store i16 %x, i16* %x2
  store i16 %y, i16* %y3
  %b4 = load i1, i1* %b1
  br i1 %b4, label %if, label %else

if:                                               ; preds = %entry
  %x5 = load i16, i16* %x2
  %y6 = load i16, i16* %y3
  %call = call i16 @types_i16(i16 %x5, i16 %y6)
  %signext = sext i16 %call to i32
  br label %merge

else:                                             ; preds = %entry
  %x7 = load i16, i16* %x2
  %signext8 = sext i16 %x7 to i32
  %y9 = load i16, i16* %y3
  %signext10 = sext i16 %y9 to i32
  %call11 = call i32 @types_i32(i32 %signext8, i32 %signext10)
  br label %merge

merge:                                            ; preds = %else, %if
  %phi = phi i32 [ %signext, %if ], [ %call11, %else ]
  %signext12 = sext i32 %phi to i64
  ret i64 %signext12
}

define i32 @nestedBranches(i32 %x, i32 %y, i32 %z) {
entry:
  %z3 = alloca i32
  %y2 = alloca i32
  %x1 = alloca i32
  store i32 %x, i32* %x1
  store i32 %y, i32* %y2
  store i32 %z, i32* %z3
  %x4 = load i32, i32* %x1
  %cmpeq = icmp eq i32 %x4, 0
  br i1 %cmpeq, label %if, label %else

if:                                               ; preds = %entry
  %y5 = load i32, i32* %y2
  %z6 = load i32, i32* %z3
  %add = add i32 %y5, %z6
  br label %merge15

else:                                             ; preds = %entry
  %x7 = load i32, i32* %x1
  %cmpeq8 = icmp eq i32 %x7, 1
  br i1 %cmpeq8, label %if9, label %else12

if9:                                              ; preds = %else
  %y10 = load i32, i32* %y2
  %z11 = load i32, i32* %z3
  %sub = sub i32 %y10, %z11
  br label %merge

else12:                                           ; preds = %else
  %y13 = load i32, i32* %y2
  %z14 = load i32, i32* %z3
  %mul = mul i32 %y13, %z14
  br label %merge

merge:                                            ; preds = %else12, %if9
  %phi = phi i32 [ %sub, %if9 ], [ %mul, %else12 ]
  br label %merge15

merge15:                                          ; preds = %merge, %if
  %phi16 = phi i32 [ %add, %if ], [ %phi, %merge ]
  ret i32 %phi16
}

define i32 @elseIfBranches(i32 %x, i32 %y, i32 %z) {
entry:
  %z3 = alloca i32
  %y2 = alloca i32
  %x1 = alloca i32
  store i32 %x, i32* %x1
  store i32 %y, i32* %y2
  store i32 %z, i32* %z3
  %x4 = load i32, i32* %x1
  %cmpeq = icmp eq i32 %x4, 0
  br i1 %cmpeq, label %if, label %else

if:                                               ; preds = %entry
  %y5 = load i32, i32* %y2
  %z6 = load i32, i32* %z3
  %add = add i32 %y5, %z6
  br label %merge21

else:                                             ; preds = %entry
  %x7 = load i32, i32* %x1
  %cmpeq8 = icmp eq i32 %x7, 1
  br i1 %cmpeq8, label %if9, label %else12

if9:                                              ; preds = %else
  %y10 = load i32, i32* %y2
  %z11 = load i32, i32* %z3
  %sub = sub i32 %y10, %z11
  br label %merge19

else12:                                           ; preds = %else
  %x13 = load i32, i32* %x1
  %cmpeq14 = icmp eq i32 %x13, 2
  br i1 %cmpeq14, label %if15, label %else18

if15:                                             ; preds = %else12
  %y16 = load i32, i32* %y2
  %z17 = load i32, i32* %z3
  %mul = mul i32 %y16, %z17
  br label %merge

else18:                                           ; preds = %else12
  br label %merge

merge:                                            ; preds = %else18, %if15
  %phi = phi i32 [ %mul, %if15 ], [ 42, %else18 ]
  br label %merge19

merge19:                                          ; preds = %merge, %if9
  %phi20 = phi i32 [ %sub, %if9 ], [ %phi, %merge ]
  br label %merge21

merge21:                                          ; preds = %merge19, %if
  %phi22 = phi i32 [ %add, %if ], [ %phi20, %merge19 ]
  ret i32 %phi22
}

define i32 @elseIfBranchesNesting(i32 %x, i32 %y, i32 %z) {
entry:
  %z3 = alloca i32
  %y2 = alloca i32
  %x1 = alloca i32
  store i32 %x, i32* %x1
  store i32 %y, i32* %y2
  store i32 %z, i32* %z3
  %x4 = load i32, i32* %x1
  %cmpeq = icmp eq i32 %x4, 0
  br i1 %cmpeq, label %if, label %else

if:                                               ; preds = %entry
  %y5 = load i32, i32* %y2
  %z6 = load i32, i32* %z3
  %add = add i32 %y5, %z6
  br label %merge43

else:                                             ; preds = %entry
  %x7 = load i32, i32* %x1
  %cmpeq8 = icmp eq i32 %x7, 1
  br i1 %cmpeq8, label %if9, label %else12

if9:                                              ; preds = %else
  %y10 = load i32, i32* %y2
  %z11 = load i32, i32* %z3
  %sub = sub i32 %y10, %z11
  br label %merge41

else12:                                           ; preds = %else
  %x13 = load i32, i32* %x1
  %cmpeq14 = icmp eq i32 %x13, 2
  br i1 %cmpeq14, label %if15, label %else18

if15:                                             ; preds = %else12
  %y16 = load i32, i32* %y2
  %z17 = load i32, i32* %z3
  %mul = mul i32 %y16, %z17
  br label %merge39

else18:                                           ; preds = %else12
  %x19 = load i32, i32* %x1
  %cmpeq20 = icmp eq i32 %x19, 3
  br i1 %cmpeq20, label %if21, label %else28

if21:                                             ; preds = %else18
  %z22 = load i32, i32* %z3
  %cmpeq23 = icmp eq i32 %z22, 0
  br i1 %cmpeq23, label %if24, label %else25

if24:                                             ; preds = %if21
  br label %merge

else25:                                           ; preds = %if21
  %y26 = load i32, i32* %y2
  %z27 = load i32, i32* %z3
  %div = sdiv i32 %y26, %z27
  br label %merge

merge:                                            ; preds = %else25, %if24
  %phi = phi i32 [ 0, %if24 ], [ %div, %else25 ]
  br label %merge37

else28:                                           ; preds = %else18
  %z29 = load i32, i32* %z3
  %cmpeq30 = icmp eq i32 %z29, 0
  br i1 %cmpeq30, label %if31, label %else32

if31:                                             ; preds = %else28
  br label %merge35

else32:                                           ; preds = %else28
  %y33 = load i32, i32* %y2
  %z34 = load i32, i32* %z3
  %mod = srem i32 %y33, %z34
  br label %merge35

merge35:                                          ; preds = %else32, %if31
  %phi36 = phi i32 [ 0, %if31 ], [ %mod, %else32 ]
  br label %merge37

merge37:                                          ; preds = %merge35, %merge
  %phi38 = phi i32 [ %phi, %merge ], [ %phi36, %merge35 ]
  br label %merge39

merge39:                                          ; preds = %merge37, %if15
  %phi40 = phi i32 [ %mul, %if15 ], [ %phi38, %merge37 ]
  br label %merge41

merge41:                                          ; preds = %merge39, %if9
  %phi42 = phi i32 [ %sub, %if9 ], [ %phi40, %merge39 ]
  br label %merge43

merge43:                                          ; preds = %merge41, %if
  %phi44 = phi i32 [ %add, %if ], [ %phi42, %merge41 ]
  ret i32 %phi44
}

define i32 @branchWithLogicalOperators(i32 %x, i32 %y, i32 %z) {
entry:
  %z3 = alloca i32
  %y2 = alloca i32
  %x1 = alloca i32
  store i32 %x, i32* %x1
  store i32 %y, i32* %y2
  store i32 %z, i32* %z3
  %x4 = load i32, i32* %x1
  %cmpeq = icmp eq i32 %x4, 0
  br i1 %cmpeq, label %andtrue, label %andfalse

andtrue:                                          ; preds = %entry
  %y5 = load i32, i32* %y2
  %cmpeq6 = icmp eq i32 %y5, 1
  br label %andmerge

andfalse:                                         ; preds = %entry
  br label %andmerge

andmerge:                                         ; preds = %andfalse, %andtrue
  %andphi = phi i1 [ %cmpeq6, %andtrue ], [ false, %andfalse ]
  br i1 %andphi, label %ortrue, label %orfalse

ortrue:                                           ; preds = %andmerge
  br label %ormerge

orfalse:                                          ; preds = %andmerge
  %z7 = load i32, i32* %z3
  %cmpeq8 = icmp eq i32 %z7, 1
  br label %ormerge

ormerge:                                          ; preds = %orfalse, %ortrue
  %orphi = phi i1 [ true, %ortrue ], [ %cmpeq8, %orfalse ]
  br i1 %orphi, label %if, label %else

if:                                               ; preds = %ormerge
  br label %merge

else:                                             ; preds = %ormerge
  br label %merge

merge:                                            ; preds = %else, %if
  %phi = phi i16 [ 100, %if ], [ 200, %else ]
  %signext = sext i16 %phi to i32
  ret i32 %signext
}

define i32 @assign(i32 %a, i32 %b, i32 %c) {
entry:
  %c3 = alloca i32
  %b2 = alloca i32
  %a1 = alloca i32
  store i32 %a, i32* %a1
  store i32 %b, i32* %b2
  store i32 %c, i32* %c3
  %b4 = load i32, i32* %b2
  %c5 = load i32, i32* %c3
  %mul = mul i32 %b4, %c5
  store i32 %mul, i32* %a1
  %a6 = load i32, i32* %a1
  %b7 = load i32, i32* %b2
  %mul8 = mul i32 3, %b7
  %add = add i32 %a6, %mul8
  %c9 = load i32, i32* %c3
  %add10 = add i32 %add, %c9
  store i32 %add10, i32* %b2
  %c11 = load i32, i32* %c3
  %add12 = add i32 %c11, 42
  store i32 %add12, i32* %c3
  %c13 = load i32, i32* %c3
  ret i32 %c13
}

define i64 @assignWithBranch(i32 %a, i32 %b, i64 %c) {
entry:
  %c3 = alloca i64
  %b2 = alloca i32
  %a1 = alloca i32
  store i32 %a, i32* %a1
  store i32 %b, i32* %b2
  store i64 %c, i64* %c3
  %b4 = load i32, i32* %b2
  %cmple = icmp sle i32 %b4, 0
  br i1 %cmple, label %if, label %else

if:                                               ; preds = %entry
  %a5 = load i32, i32* %a1
  %b6 = load i32, i32* %b2
  %mul = mul i32 %a5, %b6
  %c7 = load i64, i64* %c3
  %signext = sext i32 %mul to i64
  %add = add i64 %signext, %c7
  br label %merge

else:                                             ; preds = %entry
  %a8 = load i32, i32* %a1
  %b9 = load i32, i32* %b2
  %div = sdiv i32 %a8, %b9
  %c10 = load i64, i64* %c3
  %signext11 = sext i32 %div to i64
  %sub = sub i64 %signext11, %c10
  br label %merge

merge:                                            ; preds = %else, %if
  %phi = phi i64 [ %add, %if ], [ %sub, %else ]
  store i64 %phi, i64* %c3
  %c12 = load i64, i64* %c3
  %mul13 = mul i64 %c12, 2
  store i64 %mul13, i64* %c3
  %c14 = load i64, i64* %c3
  ret i64 %c14
}

define i32 @assignInBranch(i32 %a, i32 %b, i32 %c) {
entry:
  %c3 = alloca i32
  %b2 = alloca i32
  %a1 = alloca i32
  store i32 %a, i32* %a1
  store i32 %b, i32* %b2
  store i32 %c, i32* %c3
  %a4 = load i32, i32* %a1
  %cmple = icmp sle i32 %a4, 0
  br i1 %cmple, label %if, label %else

if:                                               ; preds = %entry
  store i32 10, i32* %b2
  %c5 = load i32, i32* %c3
  %add = add i32 %c5, 1
  br label %merge

else:                                             ; preds = %entry
  store i32 10000, i32* %b2
  %c6 = load i32, i32* %c3
  %add7 = add i32 %c6, 2
  br label %merge

merge:                                            ; preds = %else, %if
  %phi = phi i32 [ %add, %if ], [ %add7, %else ]
  store i32 %phi, i32* %c3
  %b8 = load i32, i32* %b2
  %c9 = load i32, i32* %c3
  %mul = mul i32 %b8, %c9
  store i32 %mul, i32* %c3
  %c10 = load i32, i32* %c3
  ret i32 %c10
}

define i64 @createVars(i32 %num) {
entry:
  %rv = alloca i64
  %temp2 = alloca i64
  %temp1 = alloca i32
  %num1 = alloca i32
  store i32 %num, i32* %num1
  %num2 = load i32, i32* %num1
  %cmplt = icmp slt i32 %num2, 0
  br i1 %cmplt, label %if, label %else

if:                                               ; preds = %entry
  %num3 = load i32, i32* %num1
  %mul = mul i32 %num3, 10
  store i32 %mul, i32* %temp1
  %temp14 = load i32, i32* %temp1
  %signext = sext i32 %temp14 to i64
  br label %merge

else:                                             ; preds = %entry
  %num5 = load i32, i32* %num1
  %div = sdiv i32 %num5, 10
  %signext6 = sext i32 %div to i64
  store i64 %signext6, i64* %temp2
  %temp27 = load i64, i64* %temp2
  br label %merge

merge:                                            ; preds = %else, %if
  %phi = phi i64 [ %signext, %if ], [ %temp27, %else ]
  store i64 %phi, i64* %rv
  %rv8 = load i64, i64* %rv
  ret i64 %rv8
}
