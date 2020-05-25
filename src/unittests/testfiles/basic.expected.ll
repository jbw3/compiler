; ModuleID = 'src/unittests/testfiles/basic.wip'
source_filename = "src/unittests/testfiles/basic.wip"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

%UnitType = type {}
%str = type { i64, [0 x i8] }
%Test2 = type { %Test1, i32 }
%Test1 = type { i32, i1, %str* }
%EmptyType = type {}

@strStruct0 = constant { i64, [0 x i8] } zeroinitializer
@strStruct1 = constant { i64, [15 x i8] } { i64 15, [15 x i8] c"Is this a test?" }
@strStruct2 = constant { i64, [6 x i8] } { i64 6, [6 x i8] c"\09\0D\0A\\\22'" }
@strStruct3 = constant { i64, [5 x i8] } { i64 5, [5 x i8] c"caf\C3\A9" }
@strStruct4 = constant { i64, [3 x i8] } { i64 3, [3 x i8] c"JBW" }
@strStruct5 = constant { i64, [11 x i8] } { i64 11, [11 x i8] c"\0A0\CF\80\E2\82\BF\F0\9F\98\80" }
@strStruct6 = constant { i64, [3 x i8] } { i64 3, [3 x i8] c"abc" }

declare %UnitType @extern1()

declare i64 @extern2(i32, i1)

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
  %rem = srem i32 %b10, 42
  %add11 = add i32 %sub, %rem
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
  %lshr = lshr i32 %shl, %f12
  %a13 = load i32, i32* %a1
  %ashr = ashr i32 %lshr, %a13
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

define %UnitType @assignOps(i8 %a, i16 %b, i32 %c) {
entry:
  %c3 = alloca i32
  %b2 = alloca i16
  %a1 = alloca i8
  store i8 %a, i8* %a1
  store i16 %b, i16* %b2
  store i32 %c, i32* %c3
  %a4 = load i8, i8* %a1
  %signext = sext i8 %a4 to i32
  %load = load i32, i32* %c3
  %add = add i32 %load, %signext
  store i32 %add, i32* %c3
  %a5 = load i8, i8* %a1
  %b6 = load i16, i16* %b2
  %signext7 = sext i8 %a5 to i16
  %add8 = add i16 %signext7, %b6
  %signext9 = sext i16 %add8 to i32
  %load10 = load i32, i32* %c3
  %sub = sub i32 %load10, %signext9
  store i32 %sub, i32* %c3
  %a11 = load i8, i8* %a1
  %signext12 = sext i8 %a11 to i32
  %load13 = load i32, i32* %c3
  %mul = mul i32 %load13, %signext12
  store i32 %mul, i32* %c3
  %a14 = load i8, i8* %a1
  %b15 = load i16, i16* %b2
  %signext16 = sext i8 %a14 to i16
  %add17 = add i16 %signext16, %b15
  %signext18 = sext i16 %add17 to i32
  %load19 = load i32, i32* %c3
  %div = sdiv i32 %load19, %signext18
  store i32 %div, i32* %c3
  %a20 = load i8, i8* %a1
  %signext21 = sext i8 %a20 to i32
  %load22 = load i32, i32* %c3
  %rem = srem i32 %load22, %signext21
  store i32 %rem, i32* %c3
  %a23 = load i8, i8* %a1
  %b24 = load i16, i16* %b2
  %signext25 = sext i8 %a23 to i16
  %add26 = add i16 %signext25, %b24
  %signext27 = sext i16 %add26 to i32
  %load28 = load i32, i32* %c3
  %shl = shl i32 %load28, %signext27
  store i32 %shl, i32* %c3
  %a29 = load i8, i8* %a1
  %signext30 = sext i8 %a29 to i32
  %load31 = load i32, i32* %c3
  %lshr = lshr i32 %load31, %signext30
  store i32 %lshr, i32* %c3
  %b32 = load i16, i16* %b2
  %signext33 = sext i16 %b32 to i32
  %load34 = load i32, i32* %c3
  %ashr = ashr i32 %load34, %signext33
  store i32 %ashr, i32* %c3
  %a35 = load i8, i8* %a1
  %b36 = load i16, i16* %b2
  %signext37 = sext i8 %a35 to i16
  %add38 = add i16 %signext37, %b36
  %signext39 = sext i16 %add38 to i32
  %load40 = load i32, i32* %c3
  %bitand = and i32 %load40, %signext39
  store i32 %bitand, i32* %c3
  %a41 = load i8, i8* %a1
  %signext42 = sext i8 %a41 to i32
  %load43 = load i32, i32* %c3
  %bitxor = xor i32 %load43, %signext42
  store i32 %bitxor, i32* %c3
  %a44 = load i8, i8* %a1
  %b45 = load i16, i16* %b2
  %signext46 = sext i8 %a44 to i16
  %add47 = add i16 %signext46, %b45
  %signext48 = sext i16 %add47 to i32
  %load49 = load i32, i32* %c3
  %bitor = or i32 %load49, %signext48
  store i32 %bitor, i32* %c3
  ret %UnitType zeroinitializer
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

define %UnitType @types_unit(i64 %number) {
entry:
  %number1 = alloca i64
  store i64 %number, i64* %number1
  ret %UnitType zeroinitializer
}

define %UnitType @types_unit2() {
entry:
  %call = call %UnitType @types_unit(i64 123)
  ret %UnitType %call
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

define i64 @types_isize(i64 %n1, i64 %n2) {
entry:
  %n22 = alloca i64
  %n11 = alloca i64
  store i64 %n1, i64* %n11
  store i64 %n2, i64* %n22
  %n13 = load i64, i64* %n11
  %n24 = load i64, i64* %n22
  %rem = srem i64 %n13, %n24
  ret i64 %rem
}

define i8 @types_u8(i8 %n1, i8 %n2) {
entry:
  %n22 = alloca i8
  %n11 = alloca i8
  store i8 %n1, i8* %n11
  store i8 %n2, i8* %n22
  %n13 = load i8, i8* %n11
  %n24 = load i8, i8* %n22
  %rem = urem i8 %n13, %n24
  ret i8 %rem
}

define i16 @types_u16(i16 %n1, i16 %n2) {
entry:
  %n22 = alloca i16
  %n11 = alloca i16
  store i16 %n1, i16* %n11
  store i16 %n2, i16* %n22
  %n13 = load i16, i16* %n11
  %n24 = load i16, i16* %n22
  %div = udiv i16 %n13, %n24
  ret i16 %div
}

define i32 @types_u32(i32 %n1, i32 %n2) {
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

define i64 @types_u64(i64 %n1, i64 %n2) {
entry:
  %n22 = alloca i64
  %n11 = alloca i64
  store i64 %n1, i64* %n11
  store i64 %n2, i64* %n22
  %n13 = load i64, i64* %n11
  %n24 = load i64, i64* %n22
  %sub = sub i64 %n13, %n24
  ret i64 %sub
}

define i64 @types_usize(i64 %n1, i64 %n2) {
entry:
  %n22 = alloca i64
  %n11 = alloca i64
  store i64 %n1, i64* %n11
  store i64 %n2, i64* %n22
  %n13 = load i64, i64* %n11
  %n24 = load i64, i64* %n22
  %add = add i64 %n13, %n24
  ret i64 %add
}

define %UnitType @types_str() {
entry:
  %dup2 = alloca %str*
  %dup1 = alloca %str*
  %s6 = alloca %str*
  %s5 = alloca %str*
  %s4 = alloca %str*
  %s3 = alloca %str*
  %s2 = alloca %str*
  %s1 = alloca %str*
  store %str* bitcast ({ i64, [0 x i8] }* @strStruct0 to %str*), %str** %s1
  store %str* bitcast ({ i64, [15 x i8] }* @strStruct1 to %str*), %str** %s2
  store %str* bitcast ({ i64, [6 x i8] }* @strStruct2 to %str*), %str** %s3
  store %str* bitcast ({ i64, [5 x i8] }* @strStruct3 to %str*), %str** %s4
  store %str* bitcast ({ i64, [3 x i8] }* @strStruct4 to %str*), %str** %s5
  store %str* bitcast ({ i64, [11 x i8] }* @strStruct5 to %str*), %str** %s6
  store %str* bitcast ({ i64, [3 x i8] }* @strStruct6 to %str*), %str** %dup1
  store %str* bitcast ({ i64, [3 x i8] }* @strStruct6 to %str*), %str** %dup2
  ret %UnitType zeroinitializer
}

define i64 @str_member1() {
entry:
  %size = alloca i64
  %s = alloca %str*
  store %str* bitcast ({ i64, [3 x i8] }* @strStruct6 to %str*), %str** %s
  %s1 = load %str*, %str** %s
  %mber = getelementptr inbounds %str, %str* %s1, i64 0, i32 0
  %load = load i64, i64* %mber
  store i64 %load, i64* %size
  %size2 = load i64, i64* %size
  ret i64 %size2
}

define i64 @str_member2(i32 %x) {
entry:
  %x1 = alloca i32
  store i32 %x, i32* %x1
  %x2 = load i32, i32* %x1
  %cmpeq = icmp eq i32 %x2, 0
  br i1 %cmpeq, label %if, label %else

if:                                               ; preds = %entry
  br label %merge

else:                                             ; preds = %entry
  br label %merge

merge:                                            ; preds = %else, %if
  %phi = phi %str* [ bitcast ({ i64, [3 x i8] }* @strStruct6 to %str*), %if ], [ bitcast ({ i64, [5 x i8] }* @strStruct3 to %str*), %else ]
  %mber = getelementptr inbounds %str, %str* %phi, i64 0, i32 0
  %load = load i64, i64* %mber
  ret i64 %load
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
  %rem = srem i64 %div, %signext10
  ret i64 %rem
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

define i32 @noElseBranches(i32 %x, i32 %y) {
entry:
  %z = alloca i32
  %y2 = alloca i32
  %x1 = alloca i32
  store i32 %x, i32* %x1
  store i32 %y, i32* %y2
  store i32 0, i32* %z
  %x3 = load i32, i32* %x1
  %cmpeq = icmp eq i32 %x3, 0
  br i1 %cmpeq, label %if, label %else

if:                                               ; preds = %entry
  %x4 = load i32, i32* %x1
  %y5 = load i32, i32* %y2
  %add = add i32 %x4, %y5
  store i32 %add, i32* %z
  br label %merge

else:                                             ; preds = %entry
  br label %merge

merge:                                            ; preds = %else, %if
  %phi = phi %UnitType [ zeroinitializer, %if ], [ zeroinitializer, %else ]
  %y6 = load i32, i32* %y2
  %cmplt = icmp slt i32 %y6, -5
  br i1 %cmplt, label %if7, label %else8

if7:                                              ; preds = %merge
  %load = load i32, i32* %z
  %mul = mul i32 %load, 2
  store i32 %mul, i32* %z
  br label %merge15

else8:                                            ; preds = %merge
  %y9 = load i32, i32* %y2
  %cmpgt = icmp sgt i32 %y9, 5
  br i1 %cmpgt, label %if10, label %else12

if10:                                             ; preds = %else8
  %load11 = load i32, i32* %z
  %div = sdiv i32 %load11, 2
  store i32 %div, i32* %z
  br label %merge13

else12:                                           ; preds = %else8
  br label %merge13

merge13:                                          ; preds = %else12, %if10
  %phi14 = phi %UnitType [ zeroinitializer, %if10 ], [ zeroinitializer, %else12 ]
  br label %merge15

merge15:                                          ; preds = %merge13, %if7
  %phi16 = phi %UnitType [ zeroinitializer, %if7 ], [ %phi14, %merge13 ]
  %z17 = load i32, i32* %z
  ret i32 %z17
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
  %rem = srem i32 %y33, %z34
  br label %merge35

merge35:                                          ; preds = %else32, %if31
  %phi36 = phi i32 [ 0, %if31 ], [ %rem, %else32 ]
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

define i32 @branchInExpression(i32 %x, i32 %y, i32 %z) {
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
  br label %merge

else:                                             ; preds = %entry
  %z6 = load i32, i32* %z3
  br label %merge

merge:                                            ; preds = %else, %if
  %phi = phi i32 [ %y5, %if ], [ %z6, %else ]
  %add = add i32 %phi, 58
  ret i32 %add
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
  %phi = phi i8 [ 100, %if ], [ -56, %else ]
  %zeroext = zext i8 %phi to i32
  ret i32 %zeroext
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
  %temp2 = alloca i64
  %temp1 = alloca i32
  %rv = alloca i64
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

define i32 @inferTypes(i32 %a, i32 %b) {
entry:
  %quotient = alloca i32
  %bIsZero = alloca i1
  %unaryOps = alloca i16
  %n64 = alloca i64
  %n32 = alloca i32
  %n16 = alloca i16
  %n8 = alloca i8
  %b2 = alloca i32
  %a1 = alloca i32
  store i32 %a, i32* %a1
  store i32 %b, i32* %b2
  store i8 0, i8* %n8
  store i16 200, i16* %n16
  store i32 1000000, i32* %n32
  store i64 3000000000, i64* %n64
  store i16 200, i16* %unaryOps
  %b3 = load i32, i32* %b2
  %cmpeq = icmp eq i32 %b3, 0
  store i1 %cmpeq, i1* %bIsZero
  %bIsZero4 = load i1, i1* %bIsZero
  br i1 %bIsZero4, label %if, label %else

if:                                               ; preds = %entry
  br label %merge

else:                                             ; preds = %entry
  %a5 = load i32, i32* %a1
  %b6 = load i32, i32* %b2
  %div = sdiv i32 %a5, %b6
  br label %merge

merge:                                            ; preds = %else, %if
  %phi = phi i32 [ 0, %if ], [ %div, %else ]
  store i32 %phi, i32* %quotient
  %quotient7 = load i32, i32* %quotient
  ret i32 %quotient7
}

define i32 @whileLoop(i32 %num) {
entry:
  %rv = alloca i32
  %i = alloca i32
  %num1 = alloca i32
  store i32 %num, i32* %num1
  store i32 0, i32* %i
  store i32 1, i32* %rv
  br label %whileCond

whileCond:                                        ; preds = %whileBody, %entry
  %i2 = load i32, i32* %i
  %num3 = load i32, i32* %num1
  %cmplt = icmp slt i32 %i2, %num3
  br i1 %cmplt, label %whileBody, label %whileExit

whileBody:                                        ; preds = %whileCond
  %rv4 = load i32, i32* %rv
  %mul = mul i32 %rv4, 2
  store i32 %mul, i32* %rv
  %i5 = load i32, i32* %i
  %add = add i32 %i5, 1
  store i32 %add, i32* %i
  br label %whileCond

whileExit:                                        ; preds = %whileCond
  %rv6 = load i32, i32* %rv
  ret i32 %rv6
}

define i32 @nestedLoop(i32 %num) {
entry:
  %j = alloca i32
  %rv = alloca i32
  %i = alloca i32
  %num1 = alloca i32
  store i32 %num, i32* %num1
  store i32 0, i32* %i
  store i32 1, i32* %rv
  br label %whileCond

whileCond:                                        ; preds = %whileExit, %entry
  %i2 = load i32, i32* %i
  %num3 = load i32, i32* %num1
  %cmplt = icmp slt i32 %i2, %num3
  br i1 %cmplt, label %whileBody, label %whileExit18

whileBody:                                        ; preds = %whileCond
  %i4 = load i32, i32* %i
  %rem = srem i32 %i4, 2
  %cmpeq = icmp eq i32 %rem, 0
  br i1 %cmpeq, label %if, label %else

if:                                               ; preds = %whileBody
  %rv5 = load i32, i32* %rv
  %mul = mul i32 %rv5, 2
  br label %merge

else:                                             ; preds = %whileBody
  %rv6 = load i32, i32* %rv
  %mul7 = mul i32 %rv6, 3
  br label %merge

merge:                                            ; preds = %else, %if
  %phi = phi i32 [ %mul, %if ], [ %mul7, %else ]
  store i32 %phi, i32* %rv
  store i32 0, i32* %j
  br label %whileCond8

whileCond8:                                       ; preds = %whileBody12, %merge
  %j9 = load i32, i32* %j
  %i10 = load i32, i32* %i
  %cmplt11 = icmp slt i32 %j9, %i10
  br i1 %cmplt11, label %whileBody12, label %whileExit

whileBody12:                                      ; preds = %whileCond8
  %rv13 = load i32, i32* %rv
  %add = add i32 %rv13, 1
  store i32 %add, i32* %rv
  %j14 = load i32, i32* %j
  %add15 = add i32 %j14, 1
  store i32 %add15, i32* %j
  br label %whileCond8

whileExit:                                        ; preds = %whileCond8
  %i16 = load i32, i32* %i
  %add17 = add i32 %i16, 1
  store i32 %add17, i32* %i
  br label %whileCond

whileExit18:                                      ; preds = %whileCond
  %rv19 = load i32, i32* %rv
  ret i32 %rv19
}

define i32 @blockExpression(i32 %param) {
entry:
  %x = alloca i32
  %param1 = alloca i32
  store i32 %param, i32* %param1
  %call = call %UnitType @types_unit(i64 100)
  %param2 = load i32, i32* %param1
  %cmpgt = icmp sgt i32 %param2, 0
  %param3 = load i32, i32* %param1
  %rem = srem i32 %param3, 12
  %cmpeq = icmp eq i32 %rem, 5
  %call4 = call i1 @types_bool(i1 %cmpgt, i1 %cmpeq)
  %param5 = load i32, i32* %param1
  %param6 = load i32, i32* %param1
  %call7 = call i32 @types_i32(i32 %param5, i32 %param6)
  store i32 %call7, i32* %x
  %x8 = load i32, i32* %x
  ret i32 %x8
}

define i64 @externTest() {
entry:
  %call = call %UnitType @extern1()
  %call1 = call i64 @extern2(i32 5, i1 true)
  ret i64 %call1
}

define i32 @scopes(i32 %x) {
entry:
  %y4 = alloca i16
  %y = alloca i32
  %rv = alloca i32
  %x1 = alloca i32
  store i32 %x, i32* %x1
  store i32 0, i32* %rv
  %x2 = load i32, i32* %x1
  %cmplt = icmp slt i32 %x2, 0
  br i1 %cmplt, label %if, label %else

if:                                               ; preds = %entry
  store i32 5, i32* %y
  %y3 = load i32, i32* %y
  store i32 %y3, i32* %rv
  br label %merge

else:                                             ; preds = %entry
  store i16 12, i16* %y4
  %y5 = load i16, i16* %y4
  %signext = sext i16 %y5 to i32
  store i32 %signext, i32* %rv
  br label %merge

merge:                                            ; preds = %else, %if
  %phi = phi %UnitType [ zeroinitializer, %if ], [ zeroinitializer, %else ]
  %rv6 = load i32, i32* %rv
  ret i32 %rv6
}

define %UnitType @initTypes() {
entry:
  %test2 = alloca %Test2
  %test1 = alloca %Test1
  %empty = alloca %EmptyType
  store %EmptyType undef, %EmptyType* %empty
  %call = call i16 @types_i16(i16 1, i16 2)
  %signext = sext i16 %call to i32
  %agg = insertvalue %Test1 { i32 undef, i1 undef, %str* bitcast ({ i64, [5 x i8] }* @strStruct3 to %str*) }, i32 %signext, 0
  %agg1 = insertvalue %Test1 %agg, i1 true, 1
  store %Test1 %agg1, %Test1* %test1
  store %Test2 { %Test1 { i32 1, i1 false, %str* bitcast ({ i64, [3 x i8] }* @strStruct6 to %str*) }, i32 12 }, %Test2* %test2
  %mber = getelementptr inbounds %Test1, %Test1* %test1, i64 0, i32 1
  store i1 false, i1* %mber
  %mber2 = getelementptr inbounds %Test2, %Test2* %test2, i64 0, i32 0
  %mber3 = getelementptr inbounds %Test1, %Test1* %mber2, i64 0, i32 0
  %load = load i32, i32* %mber3
  %add = add i32 %load, 7
  store i32 %add, i32* %mber3
  ret %UnitType zeroinitializer
}

define %UnitType @sign_zero_extension(i1 %b, i8 %x8, i8 %y8) {
entry:
  %r6 = alloca i32
  %r5 = alloca i32
  %r4 = alloca i32
  %r3 = alloca i32
  %r2 = alloca i32
  %r1 = alloca i32
  %y83 = alloca i8
  %x82 = alloca i8
  %b1 = alloca i1
  store i1 %b, i1* %b1
  store i8 %x8, i8* %x82
  store i8 %y8, i8* %y83
  %b4 = load i1, i1* %b1
  br i1 %b4, label %if, label %else

if:                                               ; preds = %entry
  %x85 = load i8, i8* %x82
  %signext = sext i8 %x85 to i16
  br label %merge

else:                                             ; preds = %entry
  br label %merge

merge:                                            ; preds = %else, %if
  %phi = phi i16 [ %signext, %if ], [ 200, %else ]
  %signext6 = sext i16 %phi to i32
  store i32 %signext6, i32* %r1
  %b7 = load i1, i1* %b1
  br i1 %b7, label %if8, label %else10

if8:                                              ; preds = %merge
  %y89 = load i8, i8* %y83
  br label %merge11

else10:                                           ; preds = %merge
  br label %merge11

merge11:                                          ; preds = %else10, %if8
  %phi12 = phi i8 [ %y89, %if8 ], [ -56, %else10 ]
  %zeroext = zext i8 %phi12 to i32
  store i32 %zeroext, i32* %r2
  %x813 = load i8, i8* %x82
  %add = add i8 %x813, 20
  %signext14 = sext i8 %add to i32
  store i32 %signext14, i32* %r3
  %y815 = load i8, i8* %y83
  %add16 = add i8 %y815, 20
  %zeroext17 = zext i8 %add16 to i32
  store i32 %zeroext17, i32* %r4
  %x818 = load i8, i8* %x82
  %signext19 = sext i8 %x818 to i16
  %add20 = add i16 %signext19, 200
  %signext21 = sext i16 %add20 to i32
  store i32 %signext21, i32* %r5
  %y822 = load i8, i8* %y83
  %add23 = add i8 %y822, -56
  %zeroext24 = zext i8 %add23 to i32
  store i32 %zeroext24, i32* %r6
  ret %UnitType zeroinitializer
}

define %UnitType @sign_operator(i8 %x8, i8 %y8) {
entry:
  %r2 = alloca i32
  %r1 = alloca i32
  %y82 = alloca i8
  %x81 = alloca i8
  store i8 %x8, i8* %x81
  store i8 %y8, i8* %y82
  %x83 = load i8, i8* %x81
  %signext = sext i8 %x83 to i16
  %div = sdiv i16 200, %signext
  %signext4 = sext i16 %div to i32
  store i32 %signext4, i32* %r1
  %y85 = load i8, i8* %y82
  %div6 = udiv i8 -56, %y85
  %zeroext = zext i8 %div6 to i32
  store i32 %zeroext, i32* %r2
  ret %UnitType zeroinitializer
}

define %UnitType @unary_operator(i1 %b, i8 %x, i8 %y) {
entry:
  %r16 = alloca i32
  %r15 = alloca i32
  %r14 = alloca i32
  %r13 = alloca i32
  %r12 = alloca i32
  %r11 = alloca i32
  %r10 = alloca i32
  %r9 = alloca i32
  %r8 = alloca i32
  %r7 = alloca i32
  %r6 = alloca i32
  %r5 = alloca i32
  %r4 = alloca i32
  %r3 = alloca i32
  %r2 = alloca i32
  %r1 = alloca i32
  %y3 = alloca i8
  %x2 = alloca i8
  %b1 = alloca i1
  store i1 %b, i1* %b1
  store i8 %x, i8* %x2
  store i8 %y, i8* %y3
  %b4 = load i1, i1* %b1
  br i1 %b4, label %if, label %else

if:                                               ; preds = %entry
  %x5 = load i8, i8* %x2
  %signext = sext i8 %x5 to i16
  br label %merge

else:                                             ; preds = %entry
  br label %merge

merge:                                            ; preds = %else, %if
  %phi = phi i16 [ %signext, %if ], [ -200, %else ]
  %signext6 = sext i16 %phi to i32
  store i32 %signext6, i32* %r1
  %x7 = load i8, i8* %x2
  %signext8 = sext i8 %x7 to i16
  %add = add i16 %signext8, -200
  %signext9 = sext i16 %add to i32
  store i32 %signext9, i32* %r2
  store i32 -20, i32* %r3
  store i32 20, i32* %r4
  store i32 -200, i32* %r5
  store i32 200, i32* %r6
  store i32 -21, i32* %r7
  store i32 20, i32* %r8
  store i32 235, i32* %r9
  store i32 20, i32* %r10
  store i32 55, i32* %r11
  store i32 200, i32* %r12
  store i32 55, i32* %r13
  store i32 200, i32* %r14
  %x10 = load i8, i8* %x2
  %signext11 = sext i8 %x10 to i16
  %add12 = add i16 %signext11, 200
  %signext13 = sext i16 %add12 to i32
  store i32 %signext13, i32* %r15
  %y14 = load i8, i8* %y3
  %add15 = add i8 %y14, -56
  %zeroext = zext i8 %add15 to i32
  store i32 %zeroext, i32* %r16
  ret %UnitType zeroinitializer
}
