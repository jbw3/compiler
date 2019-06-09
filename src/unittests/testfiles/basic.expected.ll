; ModuleID = 'module'
source_filename = "module"

define i32 @noArgs() {
entry:
  ret i32 42
}

define i32 @oneArg(i32 %x) {
entry:
  %add = add i32 %x, 2
  ret i32 %add
}

define i32 @integerLiterals(i32 %x) {
entry:
  %add = add i32 5, %x
  %add1 = add i32 %add, 248
  %add2 = add i32 %add1, %x
  %add3 = add i32 %add2, 123
  %add4 = add i32 %add3, %x
  %add5 = add i32 %add4, 10846099
  ret i32 %add5
}

define i32 @twoArgs(i32 %arg1, i32 %arg2) {
entry:
  %sub = sub i32 %arg1, %arg2
  ret i32 %sub
}

define i32 @opOrder1(i32 %a, i32 %b, i32 %c, i32 %d) {
entry:
  %mul = mul i32 %b, %c
  %add = add i32 %a, %mul
  %sub = sub i32 %add, %d
  ret i32 %sub
}

define i32 @opOrder2(i32 %a, i32 %b, i32 %c, i32 %d) {
entry:
  %mul = mul i32 %b, %c
  %add = add i32 %a, %mul
  %div = sdiv i32 %d, %a
  %sub = sub i32 %add, %div
  %mod = srem i32 %b, 42
  %add1 = add i32 %sub, %mod
  ret i32 %add1
}

define i1 @opOrder3(i32 %a, i32 %b, i32 %c, i32 %d) {
entry:
  %mul = mul i32 %c, %d
  %add = add i32 %b, %mul
  %cmpeq = icmp eq i32 %a, %add
  ret i1 %cmpeq
}

define i32 @opOrder4(i32 %a, i32 %b, i32 %c, i32 %d) {
entry:
  %bitand = and i32 %c, %d
  %bitxor = xor i32 %b, %bitand
  %bitor = or i32 %a, %bitxor
  ret i32 %bitor
}

define i32 @opOrder5(i32 %a, i32 %b, i32 %c, i32 %d) {
entry:
  %add = add i32 %a, %b
  %neg = sub i32 0, %add
  %call = call i32 @opOrder6(i32 %a, i32 %b, i32 %c, i32 %d)
  %add1 = add i32 %neg, %call
  %mul = mul i32 %add1, %d
  ret i32 %mul
}

define i32 @opOrder6(i32 %a, i32 %b, i32 %c, i32 %d) {
entry:
  %bitor = or i32 %a, %b
  %bitand = and i32 %c, %d
  %bitxor = xor i32 %bitor, %bitand
  ret i32 %bitxor
}

define i32 @negatives(i32 %a, i32 %b, i32 %c) {
entry:
  %neg = sub i32 0, %a
  %neg1 = sub i32 0, %b
  %neg2 = sub i32 0, %c
  %add = add i32 2, %neg2
  %call = call i32 @oneArg(i32 %add)
  %neg3 = sub i32 0, %call
  %mul = mul i32 %neg1, %neg3
  %sub = sub i32 %neg, %mul
  ret i32 %sub
}

define i32 @not(i32 %a, i32 %b) {
entry:
  %not = xor i32 %a, -1
  %not1 = xor i32 %b, -1
  %bitand = and i32 %not, %not1
  %call = call i32 @noArgs()
  %not2 = xor i32 %call, -1
  %bitor = or i32 %bitand, %not2
  ret i32 %bitor
}

define i1 @equal(i32 %a, i32 %b) {
entry:
  %cmpeq = icmp eq i32 %a, %b
  ret i1 %cmpeq
}

define i1 @notEqual(i32 %a, i32 %b) {
entry:
  %cmpne = icmp ne i32 %a, %b
  ret i1 %cmpne
}

define i1 @lessThan(i32 %a, i32 %b) {
entry:
  %cmplt = icmp slt i32 %a, %b
  ret i1 %cmplt
}

define i1 @lessThanOrEqual(i32 %a, i32 %b) {
entry:
  %cmple = icmp sle i32 %a, %b
  ret i1 %cmple
}

define i1 @greaterThan(i32 %a, i32 %b) {
entry:
  %cmpgt = icmp sgt i32 %a, %b
  ret i1 %cmpgt
}

define i1 @greaterThanOrEqual(i32 %a, i32 %b) {
entry:
  %cmpge = icmp sge i32 %a, %b
  ret i1 %cmpge
}

define i1 @logicalAnd2(i32 %x, i32 %y, i32 %z) {
entry:
  %cmpne = icmp ne i32 %x, 0
  br i1 %cmpne, label %andtrue, label %andfalse

andtrue:                                          ; preds = %entry
  %add = add i32 %z, 5
  %cmplt = icmp slt i32 %y, %add
  br label %andmerge

andfalse:                                         ; preds = %entry
  br label %andmerge

andmerge:                                         ; preds = %andfalse, %andtrue
  %andphi = phi i1 [ %cmplt, %andtrue ], [ false, %andfalse ]
  ret i1 %andphi
}

define i1 @logicalAnd3(i32 %x, i32 %y, i32 %z) {
entry:
  %cmpeq = icmp eq i32 %x, 0
  br i1 %cmpeq, label %andtrue, label %andfalse

andtrue:                                          ; preds = %entry
  %cmpeq1 = icmp eq i32 %y, 1
  br label %andmerge

andfalse:                                         ; preds = %entry
  br label %andmerge

andmerge:                                         ; preds = %andfalse, %andtrue
  %andphi = phi i1 [ %cmpeq1, %andtrue ], [ false, %andfalse ]
  br i1 %andphi, label %andtrue2, label %andfalse4

andtrue2:                                         ; preds = %andmerge
  %cmpeq3 = icmp eq i32 %z, 2
  br label %andmerge5

andfalse4:                                        ; preds = %andmerge
  br label %andmerge5

andmerge5:                                        ; preds = %andfalse4, %andtrue2
  %andphi6 = phi i1 [ %cmpeq3, %andtrue2 ], [ false, %andfalse4 ]
  ret i1 %andphi6
}

define i1 @logicalOr2(i32 %x, i32 %y, i32 %z) {
entry:
  %cmpeq = icmp eq i32 %z, 0
  br i1 %cmpeq, label %ortrue, label %orfalse

ortrue:                                           ; preds = %entry
  br label %ormerge

orfalse:                                          ; preds = %entry
  %div = sdiv i32 %y, %z
  %add = add i32 %x, %div
  %cmplt = icmp slt i32 %add, 100
  br label %ormerge

ormerge:                                          ; preds = %orfalse, %ortrue
  %orphi = phi i1 [ true, %ortrue ], [ %cmplt, %orfalse ]
  ret i1 %orphi
}

define i1 @logicalOr3(i32 %x, i32 %y, i32 %z) {
entry:
  %cmpeq = icmp eq i32 %x, 0
  br i1 %cmpeq, label %ortrue, label %orfalse

ortrue:                                           ; preds = %entry
  br label %ormerge

orfalse:                                          ; preds = %entry
  %cmpeq1 = icmp eq i32 %y, 1
  br label %ormerge

ormerge:                                          ; preds = %orfalse, %ortrue
  %orphi = phi i1 [ true, %ortrue ], [ %cmpeq1, %orfalse ]
  br i1 %orphi, label %ortrue2, label %orfalse3

ortrue2:                                          ; preds = %ormerge
  br label %ormerge5

orfalse3:                                         ; preds = %ormerge
  %cmpeq4 = icmp eq i32 %z, 2
  br label %ormerge5

ormerge5:                                         ; preds = %orfalse3, %ortrue2
  %orphi6 = phi i1 [ true, %ortrue2 ], [ %cmpeq4, %orfalse3 ]
  ret i1 %orphi6
}

define i32 @nestedCall(i32 %x, i32 %y, i32 %z) {
entry:
  %call = call i32 @twoArgs(i32 0, i32 %x)
  %add = add i32 %y, 42
  %sub = sub i32 %z, 57
  %call1 = call i32 @twoArgs(i32 %add, i32 %sub)
  %add2 = add i32 %call, %call1
  ret i32 %add2
}

define i1 @call_function_defined_later(i1 %a, i1 %b) {
entry:
  %call = call i1 @types_bool(i1 %a, i1 %b)
  ret i1 %call
}

define i1 @types_bool(i1 %a, i1 %b) {
entry:
  %bitor = or i1 %a, %b
  ret i1 %bitor
}

define i8 @types_i8(i8 %n1, i8 %n2) {
entry:
  %add = add i8 %n1, %n2
  ret i8 %add
}

define i16 @types_i16(i16 %n1, i16 %n2) {
entry:
  %sub = sub i16 %n1, %n2
  ret i16 %sub
}

define i32 @types_i32(i32 %n1, i32 %n2) {
entry:
  %mul = mul i32 %n1, %n2
  ret i32 %mul
}

define i64 @types_i64(i64 %n1, i64 %n2) {
entry:
  %div = sdiv i64 %n1, %n2
  ret i64 %div
}

define i64 @sign_extend(i8 %a, i16 %b, i64 %c, i32 %d) {
entry:
  %signext = sext i8 %a to i16
  %mul = mul i16 %signext, %b
  %signext1 = sext i16 %mul to i64
  %div = sdiv i64 %signext1, %c
  %signext2 = sext i32 %d to i64
  %mod = srem i64 %div, %signext2
  ret i64 %mod
}

define i32 @basicBranch(i32 %x, i32 %y, i32 %z) {
entry:
  %add = add i32 %x, 1
  %div = sdiv i32 %y, 2
  %cmpeq = icmp eq i32 %add, %div
  br i1 %cmpeq, label %if, label %else

if:                                               ; preds = %entry
  %mul = mul i32 %z, 10
  br label %merge

else:                                             ; preds = %entry
  %neg = sub i32 0, %z
  br label %merge

merge:                                            ; preds = %else, %if
  %phi = phi i32 [ %mul, %if ], [ %neg, %else ]
  ret i32 %phi
}

define i32 @nestedBranches(i32 %x, i32 %y, i32 %z) {
entry:
  %cmpeq = icmp eq i32 %x, 0
  br i1 %cmpeq, label %if, label %else

if:                                               ; preds = %entry
  %add = add i32 %y, %z
  br label %merge4

else:                                             ; preds = %entry
  %cmpeq1 = icmp eq i32 %x, 1
  br i1 %cmpeq1, label %if2, label %else3

if2:                                              ; preds = %else
  %sub = sub i32 %y, %z
  br label %merge

else3:                                            ; preds = %else
  %mul = mul i32 %y, %z
  br label %merge

merge:                                            ; preds = %else3, %if2
  %phi = phi i32 [ %sub, %if2 ], [ %mul, %else3 ]
  br label %merge4

merge4:                                           ; preds = %merge, %if
  %phi5 = phi i32 [ %add, %if ], [ %phi, %merge ]
  ret i32 %phi5
}

define i32 @elseIfBranches(i32 %x, i32 %y, i32 %z) {
entry:
  %cmpeq = icmp eq i32 %x, 0
  br i1 %cmpeq, label %if, label %else

if:                                               ; preds = %entry
  %add = add i32 %y, %z
  br label %merge9

else:                                             ; preds = %entry
  %cmpeq1 = icmp eq i32 %x, 1
  br i1 %cmpeq1, label %if2, label %else3

if2:                                              ; preds = %else
  %sub = sub i32 %y, %z
  br label %merge7

else3:                                            ; preds = %else
  %cmpeq4 = icmp eq i32 %x, 2
  br i1 %cmpeq4, label %if5, label %else6

if5:                                              ; preds = %else3
  %mul = mul i32 %y, %z
  br label %merge

else6:                                            ; preds = %else3
  br label %merge

merge:                                            ; preds = %else6, %if5
  %phi = phi i32 [ %mul, %if5 ], [ 42, %else6 ]
  br label %merge7

merge7:                                           ; preds = %merge, %if2
  %phi8 = phi i32 [ %sub, %if2 ], [ %phi, %merge ]
  br label %merge9

merge9:                                           ; preds = %merge7, %if
  %phi10 = phi i32 [ %add, %if ], [ %phi8, %merge7 ]
  ret i32 %phi10
}

define i32 @elseIfBranchesNesting(i32 %x, i32 %y, i32 %z) {
entry:
  %cmpeq = icmp eq i32 %x, 0
  br i1 %cmpeq, label %if, label %else

if:                                               ; preds = %entry
  %add = add i32 %y, %z
  br label %merge24

else:                                             ; preds = %entry
  %cmpeq1 = icmp eq i32 %x, 1
  br i1 %cmpeq1, label %if2, label %else3

if2:                                              ; preds = %else
  %sub = sub i32 %y, %z
  br label %merge22

else3:                                            ; preds = %else
  %cmpeq4 = icmp eq i32 %x, 2
  br i1 %cmpeq4, label %if5, label %else6

if5:                                              ; preds = %else3
  %mul = mul i32 %y, %z
  br label %merge20

else6:                                            ; preds = %else3
  %cmpeq7 = icmp eq i32 %x, 3
  br i1 %cmpeq7, label %if8, label %else12

if8:                                              ; preds = %else6
  %cmpeq9 = icmp eq i32 %z, 0
  br i1 %cmpeq9, label %if10, label %else11

if10:                                             ; preds = %if8
  br label %merge

else11:                                           ; preds = %if8
  %div = sdiv i32 %y, %z
  br label %merge

merge:                                            ; preds = %else11, %if10
  %phi = phi i32 [ 0, %if10 ], [ %div, %else11 ]
  br label %merge18

else12:                                           ; preds = %else6
  %cmpeq13 = icmp eq i32 %z, 0
  br i1 %cmpeq13, label %if14, label %else15

if14:                                             ; preds = %else12
  br label %merge16

else15:                                           ; preds = %else12
  %mod = srem i32 %y, %z
  br label %merge16

merge16:                                          ; preds = %else15, %if14
  %phi17 = phi i32 [ 0, %if14 ], [ %mod, %else15 ]
  br label %merge18

merge18:                                          ; preds = %merge16, %merge
  %phi19 = phi i32 [ %phi, %merge ], [ %phi17, %merge16 ]
  br label %merge20

merge20:                                          ; preds = %merge18, %if5
  %phi21 = phi i32 [ %mul, %if5 ], [ %phi19, %merge18 ]
  br label %merge22

merge22:                                          ; preds = %merge20, %if2
  %phi23 = phi i32 [ %sub, %if2 ], [ %phi21, %merge20 ]
  br label %merge24

merge24:                                          ; preds = %merge22, %if
  %phi25 = phi i32 [ %add, %if ], [ %phi23, %merge22 ]
  ret i32 %phi25
}

define i32 @branchWithLogicalOperators(i32 %x, i32 %y, i32 %z) {
entry:
  %cmpeq = icmp eq i32 %x, 0
  br i1 %cmpeq, label %andtrue, label %andfalse

andtrue:                                          ; preds = %entry
  %cmpeq1 = icmp eq i32 %y, 1
  br label %andmerge

andfalse:                                         ; preds = %entry
  br label %andmerge

andmerge:                                         ; preds = %andfalse, %andtrue
  %andphi = phi i1 [ %cmpeq1, %andtrue ], [ false, %andfalse ]
  br i1 %andphi, label %ortrue, label %orfalse

ortrue:                                           ; preds = %andmerge
  br label %ormerge

orfalse:                                          ; preds = %andmerge
  %cmpeq2 = icmp eq i32 %z, 1
  br label %ormerge

ormerge:                                          ; preds = %orfalse, %ortrue
  %orphi = phi i1 [ true, %ortrue ], [ %cmpeq2, %orfalse ]
  br i1 %orphi, label %if, label %else

if:                                               ; preds = %ormerge
  br label %merge

else:                                             ; preds = %ormerge
  br label %merge

merge:                                            ; preds = %else, %if
  %phi = phi i32 [ 100, %if ], [ 200, %else ]
  ret i32 %phi
}
