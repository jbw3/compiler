$module_id
$source_filename
$target_datalayout
$target_triple

%str = type { i64, i8* }
%UnitType = type {}
%Range16 = type { i16, i16 }
%Range32 = type { i32, i32 }
%Range8 = type { i8, i8 }
%"[i32]" = type { i64, i32* }
%"[i8]" = type { i64, i8* }
%Test2 = type { %Test1, i32 }
%Test1 = type { i32, i1, %str }
%EmptyType = type {}
%SubscriptTest1 = type { %SubscriptTest2 }
%SubscriptTest2 = type { %str }
%ArrayTest = type { %"[i32]" }
%"[i16]" = type { i64, i16* }
%"[[i32]]" = type { i64, %"[i32]"* }
%"[u8]" = type { i64, i8* }

@strData0 = constant [0 x i8] zeroinitializer
@strStruct0 = constant %str { i64 0, i8* getelementptr inbounds ([0 x i8], [0 x i8]* @strData0, i32 0, i32 0) }
@strData1 = constant [15 x i8] c"Is this a test?"
@strStruct1 = constant %str { i64 15, i8* getelementptr inbounds ([15 x i8], [15 x i8]* @strData1, i32 0, i32 0) }
@strData2 = constant [6 x i8] c"\09\0D\0A\\\22'"
@strStruct2 = constant %str { i64 6, i8* getelementptr inbounds ([6 x i8], [6 x i8]* @strData2, i32 0, i32 0) }
@strData3 = constant [5 x i8] c"caf\C3\A9"
@strStruct3 = constant %str { i64 5, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @strData3, i32 0, i32 0) }
@strData4 = constant [3 x i8] c"JBW"
@strStruct4 = constant %str { i64 3, i8* getelementptr inbounds ([3 x i8], [3 x i8]* @strData4, i32 0, i32 0) }
@strData5 = constant [11 x i8] c"\0A0\CF\80\E2\82\BF\F0\9F\98\80"
@strStruct5 = constant %str { i64 11, i8* getelementptr inbounds ([11 x i8], [11 x i8]* @strData5, i32 0, i32 0) }
@strData6 = constant [3 x i8] c"abc"
@strStruct6 = constant %str { i64 3, i8* getelementptr inbounds ([3 x i8], [3 x i8]* @strData6, i32 0, i32 0) }
@strData7 = constant [4 x i8] c"cdef"
@strStruct7 = constant %str { i64 4, i8* getelementptr inbounds ([4 x i8], [4 x i8]* @strData7, i32 0, i32 0) }
@strData8 = constant [3 x i8] c"cde"
@strStruct8 = constant %str { i64 3, i8* getelementptr inbounds ([3 x i8], [3 x i8]* @strData8, i32 0, i32 0) }
@strData9 = constant [2 x i8] c"ij"
@strStruct9 = constant %str { i64 2, i8* getelementptr inbounds ([2 x i8], [2 x i8]* @strData9, i32 0, i32 0) }

declare %UnitType @extern1()

declare i64 @extern2(i32, i1)

; Function Attrs: noinline nounwind optnone
define i32 @noArgs() #0 {
entry:
  ret i32 42
}

; Function Attrs: noinline nounwind optnone
define i32 @oneArg(i32 %x) #0 {
entry:
  %x1 = alloca i32, align 4
  store i32 %x, i32* %x1, align 4
  %x2 = load i32, i32* %x1, align 4
  %add = add i32 %x2, 2
  ret i32 %add
}

; Function Attrs: noinline nounwind optnone
define i32 @integerLiterals(i32 %x) #0 {
entry:
  %x1 = alloca i32, align 4
  store i32 %x, i32* %x1, align 4
  %x2 = load i32, i32* %x1, align 4
  %add = add i32 5, %x2
  %add3 = add i32 %add, 248
  %x4 = load i32, i32* %x1, align 4
  %add5 = add i32 %add3, %x4
  %add6 = add i32 %add5, 123
  %x7 = load i32, i32* %x1, align 4
  %add8 = add i32 %add6, %x7
  %add9 = add i32 %add8, 10846099
  ret i32 %add9
}

; Function Attrs: noinline nounwind optnone
define i32 @twoArgs(i32 %arg1, i32 %arg2) #0 {
entry:
  %arg22 = alloca i32, align 4
  %arg11 = alloca i32, align 4
  store i32 %arg1, i32* %arg11, align 4
  store i32 %arg2, i32* %arg22, align 4
  %arg13 = load i32, i32* %arg11, align 4
  %arg24 = load i32, i32* %arg22, align 4
  %sub = sub i32 %arg13, %arg24
  ret i32 %sub
}

; Function Attrs: noinline nounwind optnone
define i32 @opOrder1(i32 %a, i32 %b, i32 %c, i32 %d) #0 {
entry:
  %d4 = alloca i32, align 4
  %c3 = alloca i32, align 4
  %b2 = alloca i32, align 4
  %a1 = alloca i32, align 4
  store i32 %a, i32* %a1, align 4
  store i32 %b, i32* %b2, align 4
  store i32 %c, i32* %c3, align 4
  store i32 %d, i32* %d4, align 4
  %a5 = load i32, i32* %a1, align 4
  %b6 = load i32, i32* %b2, align 4
  %c7 = load i32, i32* %c3, align 4
  %mul = mul i32 %b6, %c7
  %add = add i32 %a5, %mul
  %d8 = load i32, i32* %d4, align 4
  %sub = sub i32 %add, %d8
  ret i32 %sub
}

; Function Attrs: noinline nounwind optnone
define i32 @opOrder2(i32 %a, i32 %b, i32 %c, i32 %d) #0 {
entry:
  %d4 = alloca i32, align 4
  %c3 = alloca i32, align 4
  %b2 = alloca i32, align 4
  %a1 = alloca i32, align 4
  store i32 %a, i32* %a1, align 4
  store i32 %b, i32* %b2, align 4
  store i32 %c, i32* %c3, align 4
  store i32 %d, i32* %d4, align 4
  %a5 = load i32, i32* %a1, align 4
  %b6 = load i32, i32* %b2, align 4
  %c7 = load i32, i32* %c3, align 4
  %mul = mul i32 %b6, %c7
  %add = add i32 %a5, %mul
  %d8 = load i32, i32* %d4, align 4
  %a9 = load i32, i32* %a1, align 4
  %div = sdiv i32 %d8, %a9
  %sub = sub i32 %add, %div
  %b10 = load i32, i32* %b2, align 4
  %rem = srem i32 %b10, 42
  %add11 = add i32 %sub, %rem
  ret i32 %add11
}

; Function Attrs: noinline nounwind optnone
define i1 @opOrder3(i32 %a, i32 %b, i32 %c, i32 %d) #0 {
entry:
  %d4 = alloca i32, align 4
  %c3 = alloca i32, align 4
  %b2 = alloca i32, align 4
  %a1 = alloca i32, align 4
  store i32 %a, i32* %a1, align 4
  store i32 %b, i32* %b2, align 4
  store i32 %c, i32* %c3, align 4
  store i32 %d, i32* %d4, align 4
  %a5 = load i32, i32* %a1, align 4
  %b6 = load i32, i32* %b2, align 4
  %c7 = load i32, i32* %c3, align 4
  %d8 = load i32, i32* %d4, align 4
  %mul = mul i32 %c7, %d8
  %add = add i32 %b6, %mul
  %cmpeq = icmp eq i32 %a5, %add
  ret i1 %cmpeq
}

; Function Attrs: noinline nounwind optnone
define i32 @opOrder4(i32 %a, i32 %b, i32 %c, i32 %d, i32 %e, i32 %f) #0 {
entry:
  %f6 = alloca i32, align 4
  %e5 = alloca i32, align 4
  %d4 = alloca i32, align 4
  %c3 = alloca i32, align 4
  %b2 = alloca i32, align 4
  %a1 = alloca i32, align 4
  store i32 %a, i32* %a1, align 4
  store i32 %b, i32* %b2, align 4
  store i32 %c, i32* %c3, align 4
  store i32 %d, i32* %d4, align 4
  store i32 %e, i32* %e5, align 4
  store i32 %f, i32* %f6, align 4
  %a7 = load i32, i32* %a1, align 4
  %b8 = load i32, i32* %b2, align 4
  %c9 = load i32, i32* %c3, align 4
  %d10 = load i32, i32* %d4, align 4
  %e11 = load i32, i32* %e5, align 4
  %shl = shl i32 %d10, %e11
  %f12 = load i32, i32* %f6, align 4
  %lshr = lshr i32 %shl, %f12
  %a13 = load i32, i32* %a1, align 4
  %ashr = ashr i32 %lshr, %a13
  %bitand = and i32 %c9, %ashr
  %bitxor = xor i32 %b8, %bitand
  %bitor = or i32 %a7, %bitxor
  ret i32 %bitor
}

; Function Attrs: noinline nounwind optnone
define i32 @opOrder5(i32 %a, i32 %b, i32 %c, i32 %d) #0 {
entry:
  %d4 = alloca i32, align 4
  %c3 = alloca i32, align 4
  %b2 = alloca i32, align 4
  %a1 = alloca i32, align 4
  store i32 %a, i32* %a1, align 4
  store i32 %b, i32* %b2, align 4
  store i32 %c, i32* %c3, align 4
  store i32 %d, i32* %d4, align 4
  %a5 = load i32, i32* %a1, align 4
  %b6 = load i32, i32* %b2, align 4
  %add = add i32 %a5, %b6
  %neg = sub i32 0, %add
  %a7 = load i32, i32* %a1, align 4
  %b8 = load i32, i32* %b2, align 4
  %c9 = load i32, i32* %c3, align 4
  %d10 = load i32, i32* %d4, align 4
  %call = call i32 @opOrder6(i32 %a7, i32 %b8, i32 %c9, i32 %d10)
  %add11 = add i32 %neg, %call
  %d12 = load i32, i32* %d4, align 4
  %mul = mul i32 %add11, %d12
  ret i32 %mul
}

; Function Attrs: noinline nounwind optnone
define i32 @opOrder6(i32 %a, i32 %b, i32 %c, i32 %d) #0 {
entry:
  %d4 = alloca i32, align 4
  %c3 = alloca i32, align 4
  %b2 = alloca i32, align 4
  %a1 = alloca i32, align 4
  store i32 %a, i32* %a1, align 4
  store i32 %b, i32* %b2, align 4
  store i32 %c, i32* %c3, align 4
  store i32 %d, i32* %d4, align 4
  %a5 = load i32, i32* %a1, align 4
  %b6 = load i32, i32* %b2, align 4
  %bitor = or i32 %a5, %b6
  %c7 = load i32, i32* %c3, align 4
  %d8 = load i32, i32* %d4, align 4
  %bitand = and i32 %c7, %d8
  %bitxor = xor i32 %bitor, %bitand
  ret i32 %bitxor
}

; Function Attrs: noinline nounwind optnone
define i32 @negatives(i32 %a, i32 %b, i32 %c) #0 {
entry:
  %c3 = alloca i32, align 4
  %b2 = alloca i32, align 4
  %a1 = alloca i32, align 4
  store i32 %a, i32* %a1, align 4
  store i32 %b, i32* %b2, align 4
  store i32 %c, i32* %c3, align 4
  %a4 = load i32, i32* %a1, align 4
  %neg = sub i32 0, %a4
  %b5 = load i32, i32* %b2, align 4
  %neg6 = sub i32 0, %b5
  %c7 = load i32, i32* %c3, align 4
  %neg8 = sub i32 0, %c7
  %add = add i32 2, %neg8
  %call = call i32 @oneArg(i32 %add)
  %neg9 = sub i32 0, %call
  %mul = mul i32 %neg6, %neg9
  %sub = sub i32 %neg, %mul
  ret i32 %sub
}

; Function Attrs: noinline nounwind optnone
define i32 @not(i32 %a, i32 %b) #0 {
entry:
  %b2 = alloca i32, align 4
  %a1 = alloca i32, align 4
  store i32 %a, i32* %a1, align 4
  store i32 %b, i32* %b2, align 4
  %a3 = load i32, i32* %a1, align 4
  %not = xor i32 %a3, -1
  %b4 = load i32, i32* %b2, align 4
  %not5 = xor i32 %b4, -1
  %bitand = and i32 %not, %not5
  %call = call i32 @noArgs()
  %not6 = xor i32 %call, -1
  %bitor = or i32 %bitand, %not6
  ret i32 %bitor
}

; Function Attrs: noinline nounwind optnone
define i1 @equal(i32 %a, i32 %b) #0 {
entry:
  %b2 = alloca i32, align 4
  %a1 = alloca i32, align 4
  store i32 %a, i32* %a1, align 4
  store i32 %b, i32* %b2, align 4
  %a3 = load i32, i32* %a1, align 4
  %b4 = load i32, i32* %b2, align 4
  %cmpeq = icmp eq i32 %a3, %b4
  ret i1 %cmpeq
}

; Function Attrs: noinline nounwind optnone
define i1 @notEqual(i32 %a, i32 %b) #0 {
entry:
  %b2 = alloca i32, align 4
  %a1 = alloca i32, align 4
  store i32 %a, i32* %a1, align 4
  store i32 %b, i32* %b2, align 4
  %a3 = load i32, i32* %a1, align 4
  %b4 = load i32, i32* %b2, align 4
  %cmpne = icmp ne i32 %a3, %b4
  ret i1 %cmpne
}

; Function Attrs: noinline nounwind optnone
define i1 @lessThan(i32 %a, i32 %b) #0 {
entry:
  %b2 = alloca i32, align 4
  %a1 = alloca i32, align 4
  store i32 %a, i32* %a1, align 4
  store i32 %b, i32* %b2, align 4
  %a3 = load i32, i32* %a1, align 4
  %b4 = load i32, i32* %b2, align 4
  %cmplt = icmp slt i32 %a3, %b4
  ret i1 %cmplt
}

; Function Attrs: noinline nounwind optnone
define i1 @lessThanOrEqual(i32 %a, i32 %b) #0 {
entry:
  %b2 = alloca i32, align 4
  %a1 = alloca i32, align 4
  store i32 %a, i32* %a1, align 4
  store i32 %b, i32* %b2, align 4
  %a3 = load i32, i32* %a1, align 4
  %b4 = load i32, i32* %b2, align 4
  %cmple = icmp sle i32 %a3, %b4
  ret i1 %cmple
}

; Function Attrs: noinline nounwind optnone
define i1 @greaterThan(i32 %a, i32 %b) #0 {
entry:
  %b2 = alloca i32, align 4
  %a1 = alloca i32, align 4
  store i32 %a, i32* %a1, align 4
  store i32 %b, i32* %b2, align 4
  %a3 = load i32, i32* %a1, align 4
  %b4 = load i32, i32* %b2, align 4
  %cmpgt = icmp sgt i32 %a3, %b4
  ret i1 %cmpgt
}

; Function Attrs: noinline nounwind optnone
define i1 @greaterThanOrEqual(i32 %a, i32 %b) #0 {
entry:
  %b2 = alloca i32, align 4
  %a1 = alloca i32, align 4
  store i32 %a, i32* %a1, align 4
  store i32 %b, i32* %b2, align 4
  %a3 = load i32, i32* %a1, align 4
  %b4 = load i32, i32* %b2, align 4
  %cmpge = icmp sge i32 %a3, %b4
  ret i1 %cmpge
}

; Function Attrs: noinline nounwind optnone
define i1 @logicalAnd2(i32 %x, i32 %y, i32 %z) #0 {
entry:
  %z3 = alloca i32, align 4
  %y2 = alloca i32, align 4
  %x1 = alloca i32, align 4
  store i32 %x, i32* %x1, align 4
  store i32 %y, i32* %y2, align 4
  store i32 %z, i32* %z3, align 4
  %x4 = load i32, i32* %x1, align 4
  %cmpne = icmp ne i32 %x4, 0
  br i1 %cmpne, label %andtrue, label %andmerge

andtrue:                                          ; preds = %entry
  %y5 = load i32, i32* %y2, align 4
  %z6 = load i32, i32* %z3, align 4
  %add = add i32 %z6, 5
  %cmplt = icmp slt i32 %y5, %add
  br label %andmerge

andmerge:                                         ; preds = %andtrue, %entry
  %andphi = phi i1 [ %cmplt, %andtrue ], [ false, %entry ]
  ret i1 %andphi
}

; Function Attrs: noinline nounwind optnone
define i1 @logicalAnd3(i32 %x, i32 %y, i32 %z) #0 {
entry:
  %z3 = alloca i32, align 4
  %y2 = alloca i32, align 4
  %x1 = alloca i32, align 4
  store i32 %x, i32* %x1, align 4
  store i32 %y, i32* %y2, align 4
  store i32 %z, i32* %z3, align 4
  %x4 = load i32, i32* %x1, align 4
  %cmpeq = icmp eq i32 %x4, 0
  br i1 %cmpeq, label %andtrue, label %andmerge

andtrue:                                          ; preds = %entry
  %y5 = load i32, i32* %y2, align 4
  %cmpeq6 = icmp eq i32 %y5, 1
  br label %andmerge

andmerge:                                         ; preds = %andtrue, %entry
  %andphi = phi i1 [ %cmpeq6, %andtrue ], [ false, %entry ]
  br i1 %andphi, label %andtrue7, label %andmerge10

andtrue7:                                         ; preds = %andmerge
  %z8 = load i32, i32* %z3, align 4
  %cmpeq9 = icmp eq i32 %z8, 2
  br label %andmerge10

andmerge10:                                       ; preds = %andtrue7, %andmerge
  %andphi11 = phi i1 [ %cmpeq9, %andtrue7 ], [ false, %andmerge ]
  ret i1 %andphi11
}

; Function Attrs: noinline nounwind optnone
define i1 @logicalOr2(i32 %x, i32 %y, i32 %z) #0 {
entry:
  %z3 = alloca i32, align 4
  %y2 = alloca i32, align 4
  %x1 = alloca i32, align 4
  store i32 %x, i32* %x1, align 4
  store i32 %y, i32* %y2, align 4
  store i32 %z, i32* %z3, align 4
  %z4 = load i32, i32* %z3, align 4
  %cmpeq = icmp eq i32 %z4, 0
  br i1 %cmpeq, label %ormerge, label %orfalse

orfalse:                                          ; preds = %entry
  %x5 = load i32, i32* %x1, align 4
  %y6 = load i32, i32* %y2, align 4
  %z7 = load i32, i32* %z3, align 4
  %div = sdiv i32 %y6, %z7
  %add = add i32 %x5, %div
  %cmplt = icmp slt i32 %add, 100
  br label %ormerge

ormerge:                                          ; preds = %orfalse, %entry
  %orphi = phi i1 [ %cmplt, %orfalse ], [ true, %entry ]
  ret i1 %orphi
}

; Function Attrs: noinline nounwind optnone
define i1 @logicalOr3(i32 %x, i32 %y, i32 %z) #0 {
entry:
  %z3 = alloca i32, align 4
  %y2 = alloca i32, align 4
  %x1 = alloca i32, align 4
  store i32 %x, i32* %x1, align 4
  store i32 %y, i32* %y2, align 4
  store i32 %z, i32* %z3, align 4
  %x4 = load i32, i32* %x1, align 4
  %cmpeq = icmp eq i32 %x4, 0
  br i1 %cmpeq, label %ormerge, label %orfalse

orfalse:                                          ; preds = %entry
  %y5 = load i32, i32* %y2, align 4
  %cmpeq6 = icmp eq i32 %y5, 1
  br label %ormerge

ormerge:                                          ; preds = %orfalse, %entry
  %orphi = phi i1 [ %cmpeq6, %orfalse ], [ true, %entry ]
  br i1 %orphi, label %ormerge10, label %orfalse7

orfalse7:                                         ; preds = %ormerge
  %z8 = load i32, i32* %z3, align 4
  %cmpeq9 = icmp eq i32 %z8, 2
  br label %ormerge10

ormerge10:                                        ; preds = %orfalse7, %ormerge
  %orphi11 = phi i1 [ %cmpeq9, %orfalse7 ], [ true, %ormerge ]
  ret i1 %orphi11
}

; Function Attrs: noinline nounwind optnone
define %UnitType @assignOps(i8 %a, i16 %b, i32 %c) #0 {
entry:
  %c3 = alloca i32, align 4
  %b2 = alloca i16, align 2
  %a1 = alloca i8, align 1
  store i8 %a, i8* %a1, align 1
  store i16 %b, i16* %b2, align 2
  store i32 %c, i32* %c3, align 4
  %a4 = load i8, i8* %a1, align 1
  %signext = sext i8 %a4 to i32
  %load = load i32, i32* %c3, align 4
  %add = add i32 %load, %signext
  store i32 %add, i32* %c3, align 4
  %a5 = load i8, i8* %a1, align 1
  %signext6 = sext i8 %a5 to i16
  %b7 = load i16, i16* %b2, align 2
  %add8 = add i16 %signext6, %b7
  %signext9 = sext i16 %add8 to i32
  %load10 = load i32, i32* %c3, align 4
  %sub = sub i32 %load10, %signext9
  store i32 %sub, i32* %c3, align 4
  %a11 = load i8, i8* %a1, align 1
  %signext12 = sext i8 %a11 to i32
  %load13 = load i32, i32* %c3, align 4
  %mul = mul i32 %load13, %signext12
  store i32 %mul, i32* %c3, align 4
  %a14 = load i8, i8* %a1, align 1
  %signext15 = sext i8 %a14 to i16
  %b16 = load i16, i16* %b2, align 2
  %add17 = add i16 %signext15, %b16
  %signext18 = sext i16 %add17 to i32
  %load19 = load i32, i32* %c3, align 4
  %div = sdiv i32 %load19, %signext18
  store i32 %div, i32* %c3, align 4
  %a20 = load i8, i8* %a1, align 1
  %signext21 = sext i8 %a20 to i32
  %load22 = load i32, i32* %c3, align 4
  %rem = srem i32 %load22, %signext21
  store i32 %rem, i32* %c3, align 4
  %a23 = load i8, i8* %a1, align 1
  %signext24 = sext i8 %a23 to i16
  %b25 = load i16, i16* %b2, align 2
  %add26 = add i16 %signext24, %b25
  %signext27 = sext i16 %add26 to i32
  %load28 = load i32, i32* %c3, align 4
  %shl = shl i32 %load28, %signext27
  store i32 %shl, i32* %c3, align 4
  %a29 = load i8, i8* %a1, align 1
  %signext30 = sext i8 %a29 to i32
  %load31 = load i32, i32* %c3, align 4
  %lshr = lshr i32 %load31, %signext30
  store i32 %lshr, i32* %c3, align 4
  %b32 = load i16, i16* %b2, align 2
  %signext33 = sext i16 %b32 to i32
  %load34 = load i32, i32* %c3, align 4
  %ashr = ashr i32 %load34, %signext33
  store i32 %ashr, i32* %c3, align 4
  %a35 = load i8, i8* %a1, align 1
  %signext36 = sext i8 %a35 to i16
  %b37 = load i16, i16* %b2, align 2
  %add38 = add i16 %signext36, %b37
  %signext39 = sext i16 %add38 to i32
  %load40 = load i32, i32* %c3, align 4
  %bitand = and i32 %load40, %signext39
  store i32 %bitand, i32* %c3, align 4
  %a41 = load i8, i8* %a1, align 1
  %signext42 = sext i8 %a41 to i32
  %load43 = load i32, i32* %c3, align 4
  %bitxor = xor i32 %load43, %signext42
  store i32 %bitxor, i32* %c3, align 4
  %a44 = load i8, i8* %a1, align 1
  %signext45 = sext i8 %a44 to i16
  %b46 = load i16, i16* %b2, align 2
  %add47 = add i16 %signext45, %b46
  %signext48 = sext i16 %add47 to i32
  %load49 = load i32, i32* %c3, align 4
  %bitor = or i32 %load49, %signext48
  store i32 %bitor, i32* %c3, align 4
  ret %UnitType zeroinitializer
}

; Function Attrs: noinline nounwind optnone
define i32 @nestedCall(i32 %x, i32 %y, i32 %z) #0 {
entry:
  %z3 = alloca i32, align 4
  %y2 = alloca i32, align 4
  %x1 = alloca i32, align 4
  store i32 %x, i32* %x1, align 4
  store i32 %y, i32* %y2, align 4
  store i32 %z, i32* %z3, align 4
  %x4 = load i32, i32* %x1, align 4
  %call = call i32 @twoArgs(i32 0, i32 %x4)
  %y5 = load i32, i32* %y2, align 4
  %add = add i32 %y5, 42
  %z6 = load i32, i32* %z3, align 4
  %sub = sub i32 %z6, 57
  %call7 = call i32 @twoArgs(i32 %add, i32 %sub)
  %add8 = add i32 %call, %call7
  ret i32 %add8
}

; Function Attrs: noinline nounwind optnone
define i1 @call_function_defined_later(i1 %a, i1 %b) #0 {
entry:
  %b2 = alloca i1, align 1
  %a1 = alloca i1, align 1
  store i1 %a, i1* %a1, align 1
  store i1 %b, i1* %b2, align 1
  %a3 = load i1, i1* %a1, align 1
  %b4 = load i1, i1* %b2, align 1
  %call = call i1 @types_bool(i1 %a3, i1 %b4)
  ret i1 %call
}

; Function Attrs: noinline nounwind optnone
define %UnitType @types_unit(i64 %number) #0 {
entry:
  %number1 = alloca i64, align 8
  store i64 %number, i64* %number1, align 8
  ret %UnitType zeroinitializer
}

; Function Attrs: noinline nounwind optnone
define %UnitType @types_unit2() #0 {
entry:
  %call = call %UnitType @types_unit(i64 123)
  ret %UnitType %call
}

; Function Attrs: noinline nounwind optnone
define i1 @types_bool(i1 %a, i1 %b) #0 {
entry:
  %b2 = alloca i1, align 1
  %a1 = alloca i1, align 1
  store i1 %a, i1* %a1, align 1
  store i1 %b, i1* %b2, align 1
  %a3 = load i1, i1* %a1, align 1
  %b4 = load i1, i1* %b2, align 1
  %bitor = or i1 %a3, %b4
  ret i1 %bitor
}

; Function Attrs: noinline nounwind optnone
define i8 @types_i8(i8 %n1, i8 %n2) #0 {
entry:
  %n22 = alloca i8, align 1
  %n11 = alloca i8, align 1
  store i8 %n1, i8* %n11, align 1
  store i8 %n2, i8* %n22, align 1
  %n13 = load i8, i8* %n11, align 1
  %n24 = load i8, i8* %n22, align 1
  %add = add i8 %n13, %n24
  ret i8 %add
}

; Function Attrs: noinline nounwind optnone
define i16 @types_i16(i16 %n1, i16 %n2) #0 {
entry:
  %n22 = alloca i16, align 2
  %n11 = alloca i16, align 2
  store i16 %n1, i16* %n11, align 2
  store i16 %n2, i16* %n22, align 2
  %n13 = load i16, i16* %n11, align 2
  %n24 = load i16, i16* %n22, align 2
  %sub = sub i16 %n13, %n24
  ret i16 %sub
}

; Function Attrs: noinline nounwind optnone
define i32 @types_i32(i32 %n1, i32 %n2) #0 {
entry:
  %n22 = alloca i32, align 4
  %n11 = alloca i32, align 4
  store i32 %n1, i32* %n11, align 4
  store i32 %n2, i32* %n22, align 4
  %n13 = load i32, i32* %n11, align 4
  %n24 = load i32, i32* %n22, align 4
  %mul = mul i32 %n13, %n24
  ret i32 %mul
}

; Function Attrs: noinline nounwind optnone
define i64 @types_i64(i64 %n1, i64 %n2) #0 {
entry:
  %n22 = alloca i64, align 8
  %n11 = alloca i64, align 8
  store i64 %n1, i64* %n11, align 8
  store i64 %n2, i64* %n22, align 8
  %n13 = load i64, i64* %n11, align 8
  %n24 = load i64, i64* %n22, align 8
  %div = sdiv i64 %n13, %n24
  ret i64 %div
}

; Function Attrs: noinline nounwind optnone
define i64 @types_isize(i64 %n1, i64 %n2) #0 {
entry:
  %n22 = alloca i64, align 8
  %n11 = alloca i64, align 8
  store i64 %n1, i64* %n11, align 8
  store i64 %n2, i64* %n22, align 8
  %n13 = load i64, i64* %n11, align 8
  %n24 = load i64, i64* %n22, align 8
  %rem = srem i64 %n13, %n24
  ret i64 %rem
}

; Function Attrs: noinline nounwind optnone
define i8 @types_u8(i8 %n1, i8 %n2) #0 {
entry:
  %n22 = alloca i8, align 1
  %n11 = alloca i8, align 1
  store i8 %n1, i8* %n11, align 1
  store i8 %n2, i8* %n22, align 1
  %n13 = load i8, i8* %n11, align 1
  %n24 = load i8, i8* %n22, align 1
  %rem = urem i8 %n13, %n24
  ret i8 %rem
}

; Function Attrs: noinline nounwind optnone
define i16 @types_u16(i16 %n1, i16 %n2) #0 {
entry:
  %n22 = alloca i16, align 2
  %n11 = alloca i16, align 2
  store i16 %n1, i16* %n11, align 2
  store i16 %n2, i16* %n22, align 2
  %n13 = load i16, i16* %n11, align 2
  %n24 = load i16, i16* %n22, align 2
  %div = udiv i16 %n13, %n24
  ret i16 %div
}

; Function Attrs: noinline nounwind optnone
define i32 @types_u32(i32 %n1, i32 %n2) #0 {
entry:
  %n22 = alloca i32, align 4
  %n11 = alloca i32, align 4
  store i32 %n1, i32* %n11, align 4
  store i32 %n2, i32* %n22, align 4
  %n13 = load i32, i32* %n11, align 4
  %n24 = load i32, i32* %n22, align 4
  %mul = mul i32 %n13, %n24
  ret i32 %mul
}

; Function Attrs: noinline nounwind optnone
define i64 @types_u64(i64 %n1, i64 %n2) #0 {
entry:
  %n22 = alloca i64, align 8
  %n11 = alloca i64, align 8
  store i64 %n1, i64* %n11, align 8
  store i64 %n2, i64* %n22, align 8
  %n13 = load i64, i64* %n11, align 8
  %n24 = load i64, i64* %n22, align 8
  %sub = sub i64 %n13, %n24
  ret i64 %sub
}

; Function Attrs: noinline nounwind optnone
define i64 @types_usize(i64 %n1, i64 %n2) #0 {
entry:
  %n22 = alloca i64, align 8
  %n11 = alloca i64, align 8
  store i64 %n1, i64* %n11, align 8
  store i64 %n2, i64* %n22, align 8
  %n13 = load i64, i64* %n11, align 8
  %n24 = load i64, i64* %n22, align 8
  %add = add i64 %n13, %n24
  ret i64 %add
}

; Function Attrs: noinline nounwind optnone
define %UnitType @types_str() #0 {
entry:
  %dup2 = alloca %str, align 8
  %dup1 = alloca %str, align 8
  %s6 = alloca %str, align 8
  %s5 = alloca %str, align 8
  %s4 = alloca %str, align 8
  %s3 = alloca %str, align 8
  %s2 = alloca %str, align 8
  %s1 = alloca %str, align 8
  %load = load %str, %str* @strStruct0, align 8
  store %str %load, %str* %s1, align 8
  %load1 = load %str, %str* @strStruct1, align 8
  store %str %load1, %str* %s2, align 8
  %load2 = load %str, %str* @strStruct2, align 8
  store %str %load2, %str* %s3, align 8
  %load3 = load %str, %str* @strStruct3, align 8
  store %str %load3, %str* %s4, align 8
  %load4 = load %str, %str* @strStruct4, align 8
  store %str %load4, %str* %s5, align 8
  %load5 = load %str, %str* @strStruct5, align 8
  store %str %load5, %str* %s6, align 8
  %load6 = load %str, %str* @strStruct6, align 8
  store %str %load6, %str* %dup1, align 8
  %load7 = load %str, %str* @strStruct6, align 8
  store %str %load7, %str* %dup2, align 8
  ret %UnitType zeroinitializer
}

; Function Attrs: noinline nounwind optnone
define %UnitType @types_range(i1 %b) #0 {
entry:
  %r1 = alloca %Range16, align 8
  %ir4 = alloca %Range32, align 8
  %ir3 = alloca %Range32, align 8
  %ir2 = alloca %Range32, align 8
  %ir1 = alloca %Range8, align 8
  %er3 = alloca %Range32, align 8
  %er2 = alloca %Range32, align 8
  %er1 = alloca %Range8, align 8
  %y2 = alloca i32, align 4
  %y1 = alloca i32, align 4
  %x2 = alloca i32, align 4
  %x1 = alloca i32, align 4
  %b1 = alloca i1, align 1
  store i1 %b, i1* %b1, align 1
  store i32 10, i32* %x1, align 4
  store i32 13, i32* %x2, align 4
  store i32 25, i32* %y1, align 4
  store i32 39, i32* %y2, align 4
  store %Range8 { i8 39, i8 124 }, %Range8* %er1, align 1
  %x12 = load i32, i32* %x1, align 4
  %rng = insertvalue %Range32 { i32 0, i32 undef }, i32 %x12, 1
  store %Range32 %rng, %Range32* %er2, align 4
  %x13 = load i32, i32* %x1, align 4
  %x24 = load i32, i32* %x2, align 4
  %rng5 = insertvalue %Range32 undef, i32 %x13, 0
  %rng6 = insertvalue %Range32 %rng5, i32 %x24, 1
  store %Range32 %rng6, %Range32* %er3, align 4
  store %Range8 { i8 1, i8 10 }, %Range8* %ir1, align 1
  %y17 = load i32, i32* %y1, align 4
  %rng8 = insertvalue %Range32 { i32 0, i32 undef }, i32 %y17, 1
  store %Range32 %rng8, %Range32* %ir2, align 4
  %y19 = load i32, i32* %y1, align 4
  %y210 = load i32, i32* %y2, align 4
  %rng11 = insertvalue %Range32 undef, i32 %y19, 0
  %rng12 = insertvalue %Range32 %rng11, i32 %y210, 1
  store %Range32 %rng12, %Range32* %ir3, align 4
  %x213 = load i32, i32* %x2, align 4
  %neg = sub i32 0, %x213
  %x114 = load i32, i32* %x1, align 4
  %x215 = load i32, i32* %x2, align 4
  %add = add i32 %x114, %x215
  %rng16 = insertvalue %Range32 undef, i32 %neg, 0
  %rng17 = insertvalue %Range32 %rng16, i32 %add, 1
  store %Range32 %rng17, %Range32* %ir4, align 4
  %b18 = load i1, i1* %b1, align 1
  br i1 %b18, label %if, label %else

if:                                               ; preds = %entry
  br label %merge

else:                                             ; preds = %entry
  br label %merge

merge:                                            ; preds = %else, %if
  %phi = phi %Range16 [ { i16 0, i16 3 }, %if ], [ { i16 100, i16 200 }, %else ]
  store %Range16 %phi, %Range16* %r1, align 2
  ret %UnitType zeroinitializer
}

; Function Attrs: noinline nounwind optnone
define i64 @str_member1() #0 {
entry:
  %size = alloca i64, align 8
  %s = alloca %str, align 8
  %load = load %str, %str* @strStruct6, align 8
  store %str %load, %str* %s, align 8
  %s1 = load %str, %str* %s, align 8
  %mber = extractvalue %str %s1, 0
  store i64 %mber, i64* %size, align 8
  %size2 = load i64, i64* %size, align 8
  ret i64 %size2
}

; Function Attrs: noinline nounwind optnone
define i64 @str_member2(i32 %x) #0 {
entry:
  %x1 = alloca i32, align 4
  store i32 %x, i32* %x1, align 4
  %x2 = load i32, i32* %x1, align 4
  %cmpeq = icmp eq i32 %x2, 0
  br i1 %cmpeq, label %if, label %else

if:                                               ; preds = %entry
  %load = load %str, %str* @strStruct6, align 8
  br label %merge

else:                                             ; preds = %entry
  %load3 = load %str, %str* @strStruct3, align 8
  br label %merge

merge:                                            ; preds = %else, %if
  %phi = phi %str [ %load, %if ], [ %load3, %else ]
  %mber = extractvalue %str %phi, 0
  ret i64 %mber
}

; Function Attrs: noinline nounwind optnone
define i8 @types_add_literal(i8 %num) #0 {
entry:
  %num1 = alloca i8, align 1
  store i8 %num, i8* %num1, align 1
  %num2 = load i8, i8* %num1, align 1
  %add = add i8 %num2, 1
  ret i8 %add
}

; Function Attrs: noinline nounwind optnone
define i64 @sign_extend_bin_op(i8 %a, i16 %b, i64 %c, i32 %d) #0 {
entry:
  %d4 = alloca i32, align 4
  %c3 = alloca i64, align 8
  %b2 = alloca i16, align 2
  %a1 = alloca i8, align 1
  store i8 %a, i8* %a1, align 1
  store i16 %b, i16* %b2, align 2
  store i64 %c, i64* %c3, align 8
  store i32 %d, i32* %d4, align 4
  %a5 = load i8, i8* %a1, align 1
  %signext = sext i8 %a5 to i16
  %b6 = load i16, i16* %b2, align 2
  %mul = mul i16 %signext, %b6
  %signext7 = sext i16 %mul to i64
  %c8 = load i64, i64* %c3, align 8
  %div = sdiv i64 %signext7, %c8
  %d9 = load i32, i32* %d4, align 4
  %signext10 = sext i32 %d9 to i64
  %rem = srem i64 %div, %signext10
  ret i64 %rem
}

; Function Attrs: noinline nounwind optnone
define i64 @sign_extend_ret(i32 %param) #0 {
entry:
  %param1 = alloca i32, align 4
  store i32 %param, i32* %param1, align 4
  %param2 = load i32, i32* %param1, align 4
  %add = add i32 %param2, 5
  %signext = sext i32 %add to i64
  ret i64 %signext
}

; Function Attrs: noinline nounwind optnone
define i64 @sign_extend_call(i8 %param) #0 {
entry:
  %param1 = alloca i8, align 1
  store i8 %param, i8* %param1, align 1
  %param2 = load i8, i8* %param1, align 1
  %signext = sext i8 %param2 to i32
  %call = call i64 @sign_extend_ret(i32 %signext)
  ret i64 %call
}

; Function Attrs: noinline nounwind optnone
define i32 @basicBranch(i32 %x, i32 %y, i32 %z) #0 {
entry:
  %z3 = alloca i32, align 4
  %y2 = alloca i32, align 4
  %x1 = alloca i32, align 4
  store i32 %x, i32* %x1, align 4
  store i32 %y, i32* %y2, align 4
  store i32 %z, i32* %z3, align 4
  %x4 = load i32, i32* %x1, align 4
  %add = add i32 %x4, 1
  %y5 = load i32, i32* %y2, align 4
  %div = sdiv i32 %y5, 2
  %cmpeq = icmp eq i32 %add, %div
  br i1 %cmpeq, label %if, label %else

if:                                               ; preds = %entry
  %z6 = load i32, i32* %z3, align 4
  %mul = mul i32 %z6, 10
  br label %merge

else:                                             ; preds = %entry
  %z7 = load i32, i32* %z3, align 4
  %neg = sub i32 0, %z7
  br label %merge

merge:                                            ; preds = %else, %if
  %phi = phi i32 [ %mul, %if ], [ %neg, %else ]
  ret i32 %phi
}

; Function Attrs: noinline nounwind optnone
define i64 @signExtendBranch(i1 %b, i16 %x, i16 %y) #0 {
entry:
  %y3 = alloca i16, align 2
  %x2 = alloca i16, align 2
  %b1 = alloca i1, align 1
  store i1 %b, i1* %b1, align 1
  store i16 %x, i16* %x2, align 2
  store i16 %y, i16* %y3, align 2
  %b4 = load i1, i1* %b1, align 1
  br i1 %b4, label %if, label %else

if:                                               ; preds = %entry
  %x5 = load i16, i16* %x2, align 2
  %y6 = load i16, i16* %y3, align 2
  %call = call i16 @types_i16(i16 %x5, i16 %y6)
  %signext = sext i16 %call to i32
  br label %merge

else:                                             ; preds = %entry
  %x7 = load i16, i16* %x2, align 2
  %signext8 = sext i16 %x7 to i32
  %y9 = load i16, i16* %y3, align 2
  %signext10 = sext i16 %y9 to i32
  %call11 = call i32 @types_i32(i32 %signext8, i32 %signext10)
  br label %merge

merge:                                            ; preds = %else, %if
  %phi = phi i32 [ %signext, %if ], [ %call11, %else ]
  %signext12 = sext i32 %phi to i64
  ret i64 %signext12
}

; Function Attrs: noinline nounwind optnone
define i32 @nestedBranches(i32 %x, i32 %y, i32 %z) #0 {
entry:
  %z3 = alloca i32, align 4
  %y2 = alloca i32, align 4
  %x1 = alloca i32, align 4
  store i32 %x, i32* %x1, align 4
  store i32 %y, i32* %y2, align 4
  store i32 %z, i32* %z3, align 4
  %x4 = load i32, i32* %x1, align 4
  %cmpeq = icmp eq i32 %x4, 0
  br i1 %cmpeq, label %if, label %else

if:                                               ; preds = %entry
  %y5 = load i32, i32* %y2, align 4
  %z6 = load i32, i32* %z3, align 4
  %add = add i32 %y5, %z6
  br label %merge15

else:                                             ; preds = %entry
  %x7 = load i32, i32* %x1, align 4
  %cmpeq8 = icmp eq i32 %x7, 1
  br i1 %cmpeq8, label %if9, label %else12

if9:                                              ; preds = %else
  %y10 = load i32, i32* %y2, align 4
  %z11 = load i32, i32* %z3, align 4
  %sub = sub i32 %y10, %z11
  br label %merge

else12:                                           ; preds = %else
  %y13 = load i32, i32* %y2, align 4
  %z14 = load i32, i32* %z3, align 4
  %mul = mul i32 %y13, %z14
  br label %merge

merge:                                            ; preds = %else12, %if9
  %phi = phi i32 [ %sub, %if9 ], [ %mul, %else12 ]
  br label %merge15

merge15:                                          ; preds = %merge, %if
  %phi16 = phi i32 [ %add, %if ], [ %phi, %merge ]
  ret i32 %phi16
}

; Function Attrs: noinline nounwind optnone
define i32 @elseIfBranches(i32 %x, i32 %y, i32 %z) #0 {
entry:
  %z3 = alloca i32, align 4
  %y2 = alloca i32, align 4
  %x1 = alloca i32, align 4
  store i32 %x, i32* %x1, align 4
  store i32 %y, i32* %y2, align 4
  store i32 %z, i32* %z3, align 4
  %x4 = load i32, i32* %x1, align 4
  %cmpeq = icmp eq i32 %x4, 0
  br i1 %cmpeq, label %if, label %else

if:                                               ; preds = %entry
  %y5 = load i32, i32* %y2, align 4
  %z6 = load i32, i32* %z3, align 4
  %add = add i32 %y5, %z6
  br label %merge21

else:                                             ; preds = %entry
  %x7 = load i32, i32* %x1, align 4
  %cmpeq8 = icmp eq i32 %x7, 1
  br i1 %cmpeq8, label %if9, label %else12

if9:                                              ; preds = %else
  %y10 = load i32, i32* %y2, align 4
  %z11 = load i32, i32* %z3, align 4
  %sub = sub i32 %y10, %z11
  br label %merge19

else12:                                           ; preds = %else
  %x13 = load i32, i32* %x1, align 4
  %cmpeq14 = icmp eq i32 %x13, 2
  br i1 %cmpeq14, label %if15, label %else18

if15:                                             ; preds = %else12
  %y16 = load i32, i32* %y2, align 4
  %z17 = load i32, i32* %z3, align 4
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

; Function Attrs: noinline nounwind optnone
define i32 @noElseBranches(i32 %x, i32 %y) #0 {
entry:
  %z = alloca i32, align 4
  %y2 = alloca i32, align 4
  %x1 = alloca i32, align 4
  store i32 %x, i32* %x1, align 4
  store i32 %y, i32* %y2, align 4
  store i32 0, i32* %z, align 4
  %x3 = load i32, i32* %x1, align 4
  %cmpeq = icmp eq i32 %x3, 0
  br i1 %cmpeq, label %if, label %else

if:                                               ; preds = %entry
  %x4 = load i32, i32* %x1, align 4
  %y5 = load i32, i32* %y2, align 4
  %add = add i32 %x4, %y5
  store i32 %add, i32* %z, align 4
  br label %merge

else:                                             ; preds = %entry
  br label %merge

merge:                                            ; preds = %else, %if
  %phi = phi %UnitType [ zeroinitializer, %if ], [ zeroinitializer, %else ]
  %y6 = load i32, i32* %y2, align 4
  %cmplt = icmp slt i32 %y6, -5
  br i1 %cmplt, label %if7, label %else8

if7:                                              ; preds = %merge
  %load = load i32, i32* %z, align 4
  %mul = mul i32 %load, 2
  store i32 %mul, i32* %z, align 4
  br label %merge15

else8:                                            ; preds = %merge
  %y9 = load i32, i32* %y2, align 4
  %cmpgt = icmp sgt i32 %y9, 5
  br i1 %cmpgt, label %if10, label %else12

if10:                                             ; preds = %else8
  %load11 = load i32, i32* %z, align 4
  %div = sdiv i32 %load11, 2
  store i32 %div, i32* %z, align 4
  br label %merge13

else12:                                           ; preds = %else8
  br label %merge13

merge13:                                          ; preds = %else12, %if10
  %phi14 = phi %UnitType [ zeroinitializer, %if10 ], [ zeroinitializer, %else12 ]
  br label %merge15

merge15:                                          ; preds = %merge13, %if7
  %phi16 = phi %UnitType [ zeroinitializer, %if7 ], [ %phi14, %merge13 ]
  %z17 = load i32, i32* %z, align 4
  ret i32 %z17
}

; Function Attrs: noinline nounwind optnone
define i32 @elseIfBranchesNesting(i32 %x, i32 %y, i32 %z) #0 {
entry:
  %z3 = alloca i32, align 4
  %y2 = alloca i32, align 4
  %x1 = alloca i32, align 4
  store i32 %x, i32* %x1, align 4
  store i32 %y, i32* %y2, align 4
  store i32 %z, i32* %z3, align 4
  %x4 = load i32, i32* %x1, align 4
  %cmpeq = icmp eq i32 %x4, 0
  br i1 %cmpeq, label %if, label %else

if:                                               ; preds = %entry
  %y5 = load i32, i32* %y2, align 4
  %z6 = load i32, i32* %z3, align 4
  %add = add i32 %y5, %z6
  br label %merge43

else:                                             ; preds = %entry
  %x7 = load i32, i32* %x1, align 4
  %cmpeq8 = icmp eq i32 %x7, 1
  br i1 %cmpeq8, label %if9, label %else12

if9:                                              ; preds = %else
  %y10 = load i32, i32* %y2, align 4
  %z11 = load i32, i32* %z3, align 4
  %sub = sub i32 %y10, %z11
  br label %merge41

else12:                                           ; preds = %else
  %x13 = load i32, i32* %x1, align 4
  %cmpeq14 = icmp eq i32 %x13, 2
  br i1 %cmpeq14, label %if15, label %else18

if15:                                             ; preds = %else12
  %y16 = load i32, i32* %y2, align 4
  %z17 = load i32, i32* %z3, align 4
  %mul = mul i32 %y16, %z17
  br label %merge39

else18:                                           ; preds = %else12
  %x19 = load i32, i32* %x1, align 4
  %cmpeq20 = icmp eq i32 %x19, 3
  br i1 %cmpeq20, label %if21, label %else28

if21:                                             ; preds = %else18
  %z22 = load i32, i32* %z3, align 4
  %cmpeq23 = icmp eq i32 %z22, 0
  br i1 %cmpeq23, label %if24, label %else25

if24:                                             ; preds = %if21
  br label %merge

else25:                                           ; preds = %if21
  %y26 = load i32, i32* %y2, align 4
  %z27 = load i32, i32* %z3, align 4
  %div = sdiv i32 %y26, %z27
  br label %merge

merge:                                            ; preds = %else25, %if24
  %phi = phi i32 [ 0, %if24 ], [ %div, %else25 ]
  br label %merge37

else28:                                           ; preds = %else18
  %z29 = load i32, i32* %z3, align 4
  %cmpeq30 = icmp eq i32 %z29, 0
  br i1 %cmpeq30, label %if31, label %else32

if31:                                             ; preds = %else28
  br label %merge35

else32:                                           ; preds = %else28
  %y33 = load i32, i32* %y2, align 4
  %z34 = load i32, i32* %z3, align 4
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

; Function Attrs: noinline nounwind optnone
define i32 @branchInExpression(i32 %x, i32 %y, i32 %z) #0 {
entry:
  %z3 = alloca i32, align 4
  %y2 = alloca i32, align 4
  %x1 = alloca i32, align 4
  store i32 %x, i32* %x1, align 4
  store i32 %y, i32* %y2, align 4
  store i32 %z, i32* %z3, align 4
  %x4 = load i32, i32* %x1, align 4
  %cmpeq = icmp eq i32 %x4, 0
  br i1 %cmpeq, label %if, label %else

if:                                               ; preds = %entry
  %y5 = load i32, i32* %y2, align 4
  br label %merge

else:                                             ; preds = %entry
  %z6 = load i32, i32* %z3, align 4
  br label %merge

merge:                                            ; preds = %else, %if
  %phi = phi i32 [ %y5, %if ], [ %z6, %else ]
  %add = add i32 %phi, 58
  ret i32 %add
}

; Function Attrs: noinline nounwind optnone
define i32 @branchWithLogicalOperators(i32 %x, i32 %y, i32 %z) #0 {
entry:
  %z3 = alloca i32, align 4
  %y2 = alloca i32, align 4
  %x1 = alloca i32, align 4
  store i32 %x, i32* %x1, align 4
  store i32 %y, i32* %y2, align 4
  store i32 %z, i32* %z3, align 4
  %x4 = load i32, i32* %x1, align 4
  %cmpeq = icmp eq i32 %x4, 0
  br i1 %cmpeq, label %andtrue, label %andmerge

andtrue:                                          ; preds = %entry
  %y5 = load i32, i32* %y2, align 4
  %cmpeq6 = icmp eq i32 %y5, 1
  br label %andmerge

andmerge:                                         ; preds = %andtrue, %entry
  %andphi = phi i1 [ %cmpeq6, %andtrue ], [ false, %entry ]
  br i1 %andphi, label %ormerge, label %orfalse

orfalse:                                          ; preds = %andmerge
  %z7 = load i32, i32* %z3, align 4
  %cmpeq8 = icmp eq i32 %z7, 1
  br label %ormerge

ormerge:                                          ; preds = %orfalse, %andmerge
  %orphi = phi i1 [ %cmpeq8, %orfalse ], [ true, %andmerge ]
  br i1 %orphi, label %if, label %else

if:                                               ; preds = %ormerge
  br label %merge

else:                                             ; preds = %ormerge
  br label %merge

merge:                                            ; preds = %else, %if
  %phi = phi i32 [ 100, %if ], [ 200, %else ]
  ret i32 %phi
}

; Function Attrs: noinline nounwind optnone
define i32 @assign(i32 %a, i32 %b, i32 %c) #0 {
entry:
  %c3 = alloca i32, align 4
  %b2 = alloca i32, align 4
  %a1 = alloca i32, align 4
  store i32 %a, i32* %a1, align 4
  store i32 %b, i32* %b2, align 4
  store i32 %c, i32* %c3, align 4
  %b4 = load i32, i32* %b2, align 4
  %c5 = load i32, i32* %c3, align 4
  %mul = mul i32 %b4, %c5
  store i32 %mul, i32* %a1, align 4
  %a6 = load i32, i32* %a1, align 4
  %b7 = load i32, i32* %b2, align 4
  %mul8 = mul i32 3, %b7
  %add = add i32 %a6, %mul8
  %c9 = load i32, i32* %c3, align 4
  %add10 = add i32 %add, %c9
  store i32 %add10, i32* %b2, align 4
  %c11 = load i32, i32* %c3, align 4
  %add12 = add i32 %c11, 42
  store i32 %add12, i32* %c3, align 4
  %c13 = load i32, i32* %c3, align 4
  ret i32 %c13
}

; Function Attrs: noinline nounwind optnone
define i64 @assignWithBranch(i32 %a, i32 %b, i64 %c) #0 {
entry:
  %c3 = alloca i64, align 8
  %b2 = alloca i32, align 4
  %a1 = alloca i32, align 4
  store i32 %a, i32* %a1, align 4
  store i32 %b, i32* %b2, align 4
  store i64 %c, i64* %c3, align 8
  %b4 = load i32, i32* %b2, align 4
  %cmple = icmp sle i32 %b4, 0
  br i1 %cmple, label %if, label %else

if:                                               ; preds = %entry
  %a5 = load i32, i32* %a1, align 4
  %b6 = load i32, i32* %b2, align 4
  %mul = mul i32 %a5, %b6
  %signext = sext i32 %mul to i64
  %c7 = load i64, i64* %c3, align 8
  %add = add i64 %signext, %c7
  br label %merge

else:                                             ; preds = %entry
  %a8 = load i32, i32* %a1, align 4
  %b9 = load i32, i32* %b2, align 4
  %div = sdiv i32 %a8, %b9
  %signext10 = sext i32 %div to i64
  %c11 = load i64, i64* %c3, align 8
  %sub = sub i64 %signext10, %c11
  br label %merge

merge:                                            ; preds = %else, %if
  %phi = phi i64 [ %add, %if ], [ %sub, %else ]
  store i64 %phi, i64* %c3, align 8
  %c12 = load i64, i64* %c3, align 8
  %mul13 = mul i64 %c12, 2
  store i64 %mul13, i64* %c3, align 8
  %c14 = load i64, i64* %c3, align 8
  ret i64 %c14
}

; Function Attrs: noinline nounwind optnone
define i32 @assignInBranch(i32 %a, i32 %b, i32 %c) #0 {
entry:
  %c3 = alloca i32, align 4
  %b2 = alloca i32, align 4
  %a1 = alloca i32, align 4
  store i32 %a, i32* %a1, align 4
  store i32 %b, i32* %b2, align 4
  store i32 %c, i32* %c3, align 4
  %a4 = load i32, i32* %a1, align 4
  %cmple = icmp sle i32 %a4, 0
  br i1 %cmple, label %if, label %else

if:                                               ; preds = %entry
  store i32 10, i32* %b2, align 4
  %c5 = load i32, i32* %c3, align 4
  %add = add i32 %c5, 1
  br label %merge

else:                                             ; preds = %entry
  store i32 10000, i32* %b2, align 4
  %c6 = load i32, i32* %c3, align 4
  %add7 = add i32 %c6, 2
  br label %merge

merge:                                            ; preds = %else, %if
  %phi = phi i32 [ %add, %if ], [ %add7, %else ]
  store i32 %phi, i32* %c3, align 4
  %b8 = load i32, i32* %b2, align 4
  %c9 = load i32, i32* %c3, align 4
  %mul = mul i32 %b8, %c9
  store i32 %mul, i32* %c3, align 4
  %c10 = load i32, i32* %c3, align 4
  ret i32 %c10
}

; Function Attrs: noinline nounwind optnone
define i64 @createVars(i32 %num) #0 {
entry:
  %temp2 = alloca i64, align 8
  %temp1 = alloca i32, align 4
  %rv = alloca i64, align 8
  %num1 = alloca i32, align 4
  store i32 %num, i32* %num1, align 4
  %num2 = load i32, i32* %num1, align 4
  %cmplt = icmp slt i32 %num2, 0
  br i1 %cmplt, label %if, label %else

if:                                               ; preds = %entry
  %num3 = load i32, i32* %num1, align 4
  %mul = mul i32 %num3, 10
  store i32 %mul, i32* %temp1, align 4
  %temp14 = load i32, i32* %temp1, align 4
  %signext = sext i32 %temp14 to i64
  br label %merge

else:                                             ; preds = %entry
  %num5 = load i32, i32* %num1, align 4
  %div = sdiv i32 %num5, 10
  %signext6 = sext i32 %div to i64
  store i64 %signext6, i64* %temp2, align 8
  %temp27 = load i64, i64* %temp2, align 8
  br label %merge

merge:                                            ; preds = %else, %if
  %phi = phi i64 [ %signext, %if ], [ %temp27, %else ]
  store i64 %phi, i64* %rv, align 8
  %rv8 = load i64, i64* %rv, align 8
  ret i64 %rv8
}

; Function Attrs: noinline nounwind optnone
define i32 @inferTypes(i32 %a, i32 %b) #0 {
entry:
  %quotient = alloca i32, align 4
  %bIsZero = alloca i1, align 1
  %unaryOps = alloca i16, align 2
  %n64 = alloca i64, align 8
  %n32 = alloca i32, align 4
  %n16 = alloca i16, align 2
  %n8 = alloca i8, align 1
  %b2 = alloca i32, align 4
  %a1 = alloca i32, align 4
  store i32 %a, i32* %a1, align 4
  store i32 %b, i32* %b2, align 4
  store i8 0, i8* %n8, align 1
  store i16 200, i16* %n16, align 2
  store i32 1000000, i32* %n32, align 4
  store i64 3000000000, i64* %n64, align 8
  store i16 200, i16* %unaryOps, align 2
  %b3 = load i32, i32* %b2, align 4
  %cmpeq = icmp eq i32 %b3, 0
  store i1 %cmpeq, i1* %bIsZero, align 1
  %bIsZero4 = load i1, i1* %bIsZero, align 1
  br i1 %bIsZero4, label %if, label %else

if:                                               ; preds = %entry
  br label %merge

else:                                             ; preds = %entry
  %a5 = load i32, i32* %a1, align 4
  %b6 = load i32, i32* %b2, align 4
  %div = sdiv i32 %a5, %b6
  br label %merge

merge:                                            ; preds = %else, %if
  %phi = phi i32 [ 0, %if ], [ %div, %else ]
  store i32 %phi, i32* %quotient, align 4
  %quotient7 = load i32, i32* %quotient, align 4
  ret i32 %quotient7
}

; Function Attrs: noinline nounwind optnone
define i32 @whileLoop(i32 %num) #0 {
entry:
  %rv = alloca i32, align 4
  %i = alloca i32, align 4
  %num1 = alloca i32, align 4
  store i32 %num, i32* %num1, align 4
  store i32 0, i32* %i, align 4
  store i32 1, i32* %rv, align 4
  br label %whileCond

whileCond:                                        ; preds = %whileBody, %entry
  %i2 = load i32, i32* %i, align 4
  %num3 = load i32, i32* %num1, align 4
  %cmplt = icmp slt i32 %i2, %num3
  br i1 %cmplt, label %whileBody, label %whileExit

whileBody:                                        ; preds = %whileCond
  %rv4 = load i32, i32* %rv, align 4
  %mul = mul i32 %rv4, 2
  store i32 %mul, i32* %rv, align 4
  %i5 = load i32, i32* %i, align 4
  %add = add i32 %i5, 1
  store i32 %add, i32* %i, align 4
  br label %whileCond

whileExit:                                        ; preds = %whileCond
  %rv6 = load i32, i32* %rv, align 4
  ret i32 %rv6
}

; Function Attrs: noinline nounwind optnone
define i32 @nestedLoop(i32 %num) #0 {
entry:
  %j = alloca i32, align 4
  %rv = alloca i32, align 4
  %i = alloca i32, align 4
  %num1 = alloca i32, align 4
  store i32 %num, i32* %num1, align 4
  store i32 0, i32* %i, align 4
  store i32 1, i32* %rv, align 4
  br label %whileCond

whileCond:                                        ; preds = %whileExit, %entry
  %i2 = load i32, i32* %i, align 4
  %num3 = load i32, i32* %num1, align 4
  %cmplt = icmp slt i32 %i2, %num3
  br i1 %cmplt, label %whileBody, label %whileExit18

whileBody:                                        ; preds = %whileCond
  %i4 = load i32, i32* %i, align 4
  %rem = srem i32 %i4, 2
  %cmpeq = icmp eq i32 %rem, 0
  br i1 %cmpeq, label %if, label %else

if:                                               ; preds = %whileBody
  %rv5 = load i32, i32* %rv, align 4
  %mul = mul i32 %rv5, 2
  br label %merge

else:                                             ; preds = %whileBody
  %rv6 = load i32, i32* %rv, align 4
  %mul7 = mul i32 %rv6, 3
  br label %merge

merge:                                            ; preds = %else, %if
  %phi = phi i32 [ %mul, %if ], [ %mul7, %else ]
  store i32 %phi, i32* %rv, align 4
  store i32 0, i32* %j, align 4
  br label %whileCond8

whileCond8:                                       ; preds = %whileBody12, %merge
  %j9 = load i32, i32* %j, align 4
  %i10 = load i32, i32* %i, align 4
  %cmplt11 = icmp slt i32 %j9, %i10
  br i1 %cmplt11, label %whileBody12, label %whileExit

whileBody12:                                      ; preds = %whileCond8
  %rv13 = load i32, i32* %rv, align 4
  %add = add i32 %rv13, 1
  store i32 %add, i32* %rv, align 4
  %j14 = load i32, i32* %j, align 4
  %add15 = add i32 %j14, 1
  store i32 %add15, i32* %j, align 4
  br label %whileCond8

whileExit:                                        ; preds = %whileCond8
  %i16 = load i32, i32* %i, align 4
  %add17 = add i32 %i16, 1
  store i32 %add17, i32* %i, align 4
  br label %whileCond

whileExit18:                                      ; preds = %whileCond
  %rv19 = load i32, i32* %rv, align 4
  ret i32 %rv19
}

; Function Attrs: noinline nounwind optnone
define i64 @whileComplexCondition(%str %s, i8 %c) #0 {
entry:
  %idx = alloca i64, align 8
  %c2 = alloca i8, align 1
  %s1 = alloca %str, align 8
  store %str %s, %str* %s1, align 8
  store i8 %c, i8* %c2, align 1
  store i64 0, i64* %idx, align 8
  br label %whileCond

whileCond:                                        ; preds = %whileBody, %entry
  %idx3 = load i64, i64* %idx, align 8
  %s4 = load %str, %str* %s1, align 8
  %mber = extractvalue %str %s4, 0
  %cmplt = icmp ult i64 %idx3, %mber
  br i1 %cmplt, label %andtrue, label %andmerge

andtrue:                                          ; preds = %whileCond
  %s5 = load %str, %str* %s1, align 8
  %idx6 = load i64, i64* %idx, align 8
  %size = extractvalue %str %s5, 0
  %check = icmp uge i64 %idx6, %size
  br i1 %check, label %failed, label %passed

failed:                                           ; preds = %andtrue
  call void @exit(i32 1)
  unreachable

passed:                                           ; preds = %andtrue
  %data = extractvalue %str %s5, 1
  %value = getelementptr inbounds i8, i8* %data, i64 %idx6
  %load = load i8, i8* %value, align 1
  %c7 = load i8, i8* %c2, align 1
  %cmpne = icmp ne i8 %load, %c7
  br label %andmerge

andmerge:                                         ; preds = %passed, %whileCond
  %andphi = phi i1 [ %cmpne, %passed ], [ false, %whileCond ]
  br i1 %andphi, label %whileBody, label %whileExit

whileBody:                                        ; preds = %andmerge
  %load8 = load i64, i64* %idx, align 8
  %add = add i64 %load8, 1
  store i64 %add, i64* %idx, align 8
  br label %whileCond

whileExit:                                        ; preds = %andmerge
  %idx9 = load i64, i64* %idx, align 8
  ret i64 %idx9
}

; Function Attrs: noinline nounwind optnone
define i32 @forLoop(i32 %start, i32 %end) #0 {
entry:
  %i = alloca i32, align 4
  %num = alloca i32, align 4
  %end2 = alloca i32, align 4
  %start1 = alloca i32, align 4
  store i32 %start, i32* %start1, align 4
  store i32 %end, i32* %end2, align 4
  store i32 1, i32* %num, align 4
  %start3 = load i32, i32* %start1, align 4
  %end4 = load i32, i32* %end2, align 4
  %rng = insertvalue %Range32 undef, i32 %start3, 0
  %rng5 = insertvalue %Range32 %rng, i32 %end4, 1
  %start6 = extractvalue %Range32 %rng5, 0
  %end7 = extractvalue %Range32 %rng5, 1
  br label %forCond

forCond:                                          ; preds = %forIter, %entry
  %iter = phi i32 [ %start6, %entry ], [ %inc, %forIter ]
  %cmp = icmp sle i32 %iter, %end7
  br i1 %cmp, label %forBody, label %forExit

forBody:                                          ; preds = %forCond
  store i32 %iter, i32* %i, align 4
  %i8 = load i32, i32* %i, align 4
  %load = load i32, i32* %num, align 4
  %mul = mul i32 %load, %i8
  store i32 %mul, i32* %num, align 4
  br label %forIter

forIter:                                          ; preds = %forBody
  %inc = add i32 %iter, 1
  br label %forCond

forExit:                                          ; preds = %forCond
  %num9 = load i32, i32* %num, align 4
  ret i32 %num9
}

; Function Attrs: noinline nounwind optnone
define i32 @forLoopArray(%"[i32]" %a) #0 {
entry:
  %x30 = alloca i32, align 4
  %x12 = alloca i16, align 2
  %array6 = alloca [3 x i8], align 1
  %x = alloca i8, align 1
  %array = alloca [3 x i8], align 1
  %sum = alloca i32, align 4
  %a1 = alloca %"[i32]", align 8
  store %"[i32]" %a, %"[i32]"* %a1, align 8
  store i32 0, i32* %sum, align 4
  %ptr = getelementptr inbounds [3 x i8], [3 x i8]* %array, i64 0, i64 0
  store i8 1, i8* %ptr, align 1
  %ptr2 = getelementptr inbounds [3 x i8], [3 x i8]* %array, i64 0, i64 1
  store i8 2, i8* %ptr2, align 1
  %ptr3 = getelementptr inbounds [3 x i8], [3 x i8]* %array, i64 0, i64 2
  store i8 3, i8* %ptr3, align 1
  %arrptr = bitcast [3 x i8]* %array to i8*
  %agg = insertvalue %"[i8]" { i64 3, i8* undef }, i8* %arrptr, 1
  %size = extractvalue %"[i8]" %agg, 0
  %data = extractvalue %"[i8]" %agg, 1
  br label %forCond

forCond:                                          ; preds = %forIter, %entry
  %iter = phi i64 [ 0, %entry ], [ %inc, %forIter ]
  %cmp = icmp ult i64 %iter, %size
  br i1 %cmp, label %forBody, label %forExit

forBody:                                          ; preds = %forCond
  %value = getelementptr inbounds i8, i8* %data, i64 %iter
  %load = load i8, i8* %value, align 1
  store i8 %load, i8* %x, align 1
  %x4 = load i8, i8* %x, align 1
  %signext = sext i8 %x4 to i32
  %load5 = load i32, i32* %sum, align 4
  %add = add i32 %load5, %signext
  store i32 %add, i32* %sum, align 4
  br label %forIter

forIter:                                          ; preds = %forBody
  %inc = add i64 %iter, 1
  br label %forCond

forExit:                                          ; preds = %forCond
  %ptr7 = getelementptr inbounds [3 x i8], [3 x i8]* %array6, i64 0, i64 0
  store i8 10, i8* %ptr7, align 1
  %ptr8 = getelementptr inbounds [3 x i8], [3 x i8]* %array6, i64 0, i64 1
  store i8 20, i8* %ptr8, align 1
  %ptr9 = getelementptr inbounds [3 x i8], [3 x i8]* %array6, i64 0, i64 2
  store i8 30, i8* %ptr9, align 1
  %arrptr10 = bitcast [3 x i8]* %array6 to i8*
  %agg11 = insertvalue %"[i8]" { i64 3, i8* undef }, i8* %arrptr10, 1
  %size13 = extractvalue %"[i8]" %agg11, 0
  %data14 = extractvalue %"[i8]" %agg11, 1
  br label %forCond15

forCond15:                                        ; preds = %forIter26, %forExit
  %iter16 = phi i64 [ 0, %forExit ], [ %inc27, %forIter26 ]
  %cmp17 = icmp ult i64 %iter16, %size13
  br i1 %cmp17, label %forBody18, label %forExit28

forBody18:                                        ; preds = %forCond15
  %value19 = getelementptr inbounds i8, i8* %data14, i64 %iter16
  %load20 = load i8, i8* %value19, align 1
  %signext21 = sext i8 %load20 to i16
  store i16 %signext21, i16* %x12, align 2
  %x22 = load i16, i16* %x12, align 2
  %signext23 = sext i16 %x22 to i32
  %load24 = load i32, i32* %sum, align 4
  %add25 = add i32 %load24, %signext23
  store i32 %add25, i32* %sum, align 4
  br label %forIter26

forIter26:                                        ; preds = %forBody18
  %inc27 = add i64 %iter16, 1
  br label %forCond15

forExit28:                                        ; preds = %forCond15
  %a29 = load %"[i32]", %"[i32]"* %a1, align 8
  %size31 = extractvalue %"[i32]" %a29, 0
  %data32 = extractvalue %"[i32]" %a29, 1
  br label %forCond33

forCond33:                                        ; preds = %forIter42, %forExit28
  %iter34 = phi i64 [ 0, %forExit28 ], [ %inc43, %forIter42 ]
  %cmp35 = icmp ult i64 %iter34, %size31
  br i1 %cmp35, label %forBody36, label %forExit44

forBody36:                                        ; preds = %forCond33
  %value37 = getelementptr inbounds i32, i32* %data32, i64 %iter34
  %load38 = load i32, i32* %value37, align 4
  store i32 %load38, i32* %x30, align 4
  %x39 = load i32, i32* %x30, align 4
  %load40 = load i32, i32* %sum, align 4
  %add41 = add i32 %load40, %x39
  store i32 %add41, i32* %sum, align 4
  br label %forIter42

forIter42:                                        ; preds = %forBody36
  %inc43 = add i64 %iter34, 1
  br label %forCond33

forExit44:                                        ; preds = %forCond33
  %sum45 = load i32, i32* %sum, align 4
  ret i32 %sum45
}

; Function Attrs: noinline nounwind optnone
define i32 @nestedForLoop() #0 {
entry:
  %j = alloca i32, align 4
  %i = alloca i32, align 4
  %x = alloca i32, align 4
  store i32 0, i32* %x, align 4
  br label %forCond

forCond:                                          ; preds = %forIter7, %entry
  %iter = phi i32 [ 0, %entry ], [ %inc8, %forIter7 ]
  %cmp = icmp ult i32 %iter, 5
  br i1 %cmp, label %forBody, label %forExit9

forBody:                                          ; preds = %forCond
  store i32 %iter, i32* %i, align 4
  br label %forCond1

forCond1:                                         ; preds = %forIter, %forBody
  %iter2 = phi i32 [ 1, %forBody ], [ %inc, %forIter ]
  %cmp3 = icmp ule i32 %iter2, 10
  br i1 %cmp3, label %forBody4, label %forExit

forBody4:                                         ; preds = %forCond1
  store i32 %iter2, i32* %j, align 4
  %i5 = load i32, i32* %i, align 4
  %j6 = load i32, i32* %j, align 4
  %mul = mul i32 %i5, %j6
  %load = load i32, i32* %x, align 4
  %add = add i32 %load, %mul
  store i32 %add, i32* %x, align 4
  br label %forIter

forIter:                                          ; preds = %forBody4
  %inc = add i32 %iter2, 1
  br label %forCond1

forExit:                                          ; preds = %forCond1
  br label %forIter7

forIter7:                                         ; preds = %forExit
  %inc8 = add i32 %iter, 1
  br label %forCond

forExit9:                                         ; preds = %forCond
  %x10 = load i32, i32* %x, align 4
  ret i32 %x10
}

; Function Attrs: noinline nounwind optnone
define i64 @forLoopIndex(%"[i32]" %array) #0 {
entry:
  %i10 = alloca i64, align 8
  %x9 = alloca i32, align 4
  %num = alloca i64, align 8
  %i = alloca i64, align 8
  %x = alloca i32, align 4
  %array1 = alloca %"[i32]", align 8
  store %"[i32]" %array, %"[i32]"* %array1, align 8
  %array2 = load %"[i32]", %"[i32]"* %array1, align 8
  %size = extractvalue %"[i32]" %array2, 0
  %data = extractvalue %"[i32]" %array2, 1
  br label %forCond

forCond:                                          ; preds = %forIter, %entry
  %iter = phi i64 [ 0, %entry ], [ %inc, %forIter ]
  %cmp = icmp ult i64 %iter, %size
  br i1 %cmp, label %forBody, label %forExit

forBody:                                          ; preds = %forCond
  %value = getelementptr inbounds i32, i32* %data, i64 %iter
  %load = load i32, i32* %value, align 4
  store i32 %load, i32* %x, align 4
  store i64 %iter, i64* %i, align 8
  %array3 = load %"[i32]", %"[i32]"* %array1, align 8
  %i4 = load i64, i64* %i, align 8
  %size5 = extractvalue %"[i32]" %array3, 0
  %check = icmp uge i64 %i4, %size5
  br i1 %check, label %failed, label %passed

failed:                                           ; preds = %forBody
  call void @exit(i32 1)
  unreachable

passed:                                           ; preds = %forBody
  %data6 = extractvalue %"[i32]" %array3, 1
  %value7 = getelementptr inbounds i32, i32* %data6, i64 %i4
  %x8 = load i32, i32* %x, align 4
  %mul = mul i32 %x8, 2
  store i32 %mul, i32* %value7, align 4
  br label %forIter

forIter:                                          ; preds = %passed
  %inc = add i64 %iter, 1
  br label %forCond

forExit:                                          ; preds = %forCond
  store i64 0, i64* %num, align 8
  br label %forCond11

forCond11:                                        ; preds = %forIter19, %forExit
  %iter12 = phi i32 [ 0, %forExit ], [ %inc20, %forIter19 ]
  %idx = phi i64 [ 0, %forExit ], [ %inc21, %forIter19 ]
  %cmp13 = icmp ult i32 %iter12, 10
  br i1 %cmp13, label %forBody14, label %forExit22

forBody14:                                        ; preds = %forCond11
  store i32 %iter12, i32* %x9, align 4
  store i64 %idx, i64* %i10, align 8
  %x15 = load i32, i32* %x9, align 4
  %zeroext = zext i32 %x15 to i64
  %i16 = load i64, i64* %i10, align 8
  %mul17 = mul i64 %zeroext, %i16
  %load18 = load i64, i64* %num, align 8
  %add = add i64 %load18, %mul17
  store i64 %add, i64* %num, align 8
  br label %forIter19

forIter19:                                        ; preds = %forBody14
  %inc20 = add i32 %iter12, 1
  %inc21 = add i64 %idx, 1
  br label %forCond11

forExit22:                                        ; preds = %forCond11
  %num23 = load i64, i64* %num, align 8
  ret i64 %num23
}

; Function Attrs: noinline nounwind optnone
define i32 @blockExpression(i32 %param) #0 {
entry:
  %x = alloca i32, align 4
  %param1 = alloca i32, align 4
  store i32 %param, i32* %param1, align 4
  %call = call %UnitType @types_unit(i64 100)
  %param2 = load i32, i32* %param1, align 4
  %cmpgt = icmp sgt i32 %param2, 0
  %param3 = load i32, i32* %param1, align 4
  %rem = srem i32 %param3, 12
  %cmpeq = icmp eq i32 %rem, 5
  %call4 = call i1 @types_bool(i1 %cmpgt, i1 %cmpeq)
  %param5 = load i32, i32* %param1, align 4
  %param6 = load i32, i32* %param1, align 4
  %call7 = call i32 @types_i32(i32 %param5, i32 %param6)
  store i32 %call7, i32* %x, align 4
  %x8 = load i32, i32* %x, align 4
  ret i32 %x8
}

; Function Attrs: noinline nounwind optnone
define i64 @externTest() #0 {
entry:
  %call = call %UnitType @extern1()
  %call1 = call i64 @extern2(i32 5, i1 true)
  ret i64 %call1
}

; Function Attrs: noinline nounwind optnone
define i32 @scopes(i32 %x) #0 {
entry:
  %y4 = alloca i16, align 2
  %y = alloca i32, align 4
  %rv = alloca i32, align 4
  %x1 = alloca i32, align 4
  store i32 %x, i32* %x1, align 4
  store i32 0, i32* %rv, align 4
  %x2 = load i32, i32* %x1, align 4
  %cmplt = icmp slt i32 %x2, 0
  br i1 %cmplt, label %if, label %else

if:                                               ; preds = %entry
  store i32 5, i32* %y, align 4
  %y3 = load i32, i32* %y, align 4
  store i32 %y3, i32* %rv, align 4
  br label %merge

else:                                             ; preds = %entry
  store i16 12, i16* %y4, align 2
  %y5 = load i16, i16* %y4, align 2
  %signext = sext i16 %y5 to i32
  store i32 %signext, i32* %rv, align 4
  br label %merge

merge:                                            ; preds = %else, %if
  %phi = phi %UnitType [ zeroinitializer, %if ], [ zeroinitializer, %else ]
  %rv6 = load i32, i32* %rv, align 4
  ret i32 %rv6
}

; Function Attrs: noinline nounwind optnone
define %UnitType @initTypes() #0 {
entry:
  %test2 = alloca %Test2, align 8
  %test1 = alloca %Test1, align 8
  %empty = alloca %EmptyType, align 8
  store %EmptyType undef, %EmptyType* %empty, align 1
  %load = load %str, %str* @strStruct3, align 8
  %agg = insertvalue %Test1 undef, %str %load, 2
  %call = call i16 @types_i16(i16 1, i16 2)
  %signext = sext i16 %call to i32
  %agg1 = insertvalue %Test1 %agg, i32 %signext, 0
  %agg2 = insertvalue %Test1 %agg1, i1 true, 1
  store %Test1 %agg2, %Test1* %test1, align 8
  %load3 = load %str, %str* @strStruct6, align 8
  %agg4 = insertvalue %Test1 { i32 1, i1 false, %str undef }, %str %load3, 2
  %agg5 = insertvalue %Test2 undef, %Test1 %agg4, 0
  %agg6 = insertvalue %Test2 %agg5, i32 12, 1
  store %Test2 %agg6, %Test2* %test2, align 8
  %mber = getelementptr inbounds %Test1, %Test1* %test1, i64 0, i32 1
  store i1 false, i1* %mber, align 1
  %mber7 = getelementptr inbounds %Test2, %Test2* %test2, i64 0, i32 0
  %mber8 = getelementptr inbounds %Test1, %Test1* %mber7, i64 0, i32 0
  %load9 = load i32, i32* %mber8, align 4
  %add = add i32 %load9, 7
  store i32 %add, i32* %mber8, align 4
  ret %UnitType zeroinitializer
}

; Function Attrs: noinline nounwind optnone
define %UnitType @sign_zero_extension(i1 %b, i8 %x8, i8 %y8, i32 %x32) #0 {
entry:
  %r9 = alloca %Range32, align 8
  %r8 = alloca i16, align 2
  %r7 = alloca i16, align 2
  %r6 = alloca i32, align 4
  %r5 = alloca i32, align 4
  %r4 = alloca i32, align 4
  %r3 = alloca i32, align 4
  %r2 = alloca i32, align 4
  %r1 = alloca i32, align 4
  %x324 = alloca i32, align 4
  %y83 = alloca i8, align 1
  %x82 = alloca i8, align 1
  %b1 = alloca i1, align 1
  store i1 %b, i1* %b1, align 1
  store i8 %x8, i8* %x82, align 1
  store i8 %y8, i8* %y83, align 1
  store i32 %x32, i32* %x324, align 4
  %b5 = load i1, i1* %b1, align 1
  br i1 %b5, label %if, label %else

if:                                               ; preds = %entry
  %x86 = load i8, i8* %x82, align 1
  %signext = sext i8 %x86 to i16
  br label %merge

else:                                             ; preds = %entry
  br label %merge

merge:                                            ; preds = %else, %if
  %phi = phi i16 [ %signext, %if ], [ 200, %else ]
  %signext7 = sext i16 %phi to i32
  store i32 %signext7, i32* %r1, align 4
  %b8 = load i1, i1* %b1, align 1
  br i1 %b8, label %if9, label %else11

if9:                                              ; preds = %merge
  %y810 = load i8, i8* %y83, align 1
  br label %merge12

else11:                                           ; preds = %merge
  br label %merge12

merge12:                                          ; preds = %else11, %if9
  %phi13 = phi i8 [ %y810, %if9 ], [ -56, %else11 ]
  %zeroext = zext i8 %phi13 to i32
  store i32 %zeroext, i32* %r2, align 4
  %x814 = load i8, i8* %x82, align 1
  %add = add i8 %x814, 20
  %signext15 = sext i8 %add to i32
  store i32 %signext15, i32* %r3, align 4
  %y816 = load i8, i8* %y83, align 1
  %add17 = add i8 %y816, 20
  %zeroext18 = zext i8 %add17 to i32
  store i32 %zeroext18, i32* %r4, align 4
  %x819 = load i8, i8* %x82, align 1
  %signext20 = sext i8 %x819 to i16
  %add21 = add i16 %signext20, 200
  %signext22 = sext i16 %add21 to i32
  store i32 %signext22, i32* %r5, align 4
  %y823 = load i8, i8* %y83, align 1
  %add24 = add i8 %y823, -56
  %zeroext25 = zext i8 %add24 to i32
  store i32 %zeroext25, i32* %r6, align 4
  store i16 201, i16* %r7, align 2
  store i16 201, i16* %r8, align 2
  %x3226 = load i32, i32* %x324, align 4
  %x3227 = load i32, i32* %x324, align 4
  %rng = insertvalue %Range32 undef, i32 %x3226, 0
  %rng28 = insertvalue %Range32 %rng, i32 %x3227, 1
  store %Range32 %rng28, %Range32* %r9, align 4
  %b29 = load i1, i1* %b1, align 1
  br i1 %b29, label %if30, label %else31

if30:                                             ; preds = %merge12
  br label %merge32

else31:                                           ; preds = %merge12
  br label %merge32

merge32:                                          ; preds = %else31, %if30
  %phi33 = phi %Range32 [ { i32 0, i32 1 }, %if30 ], [ { i32 1, i32 2 }, %else31 ]
  store %Range32 %phi33, %Range32* %r9, align 4
  %b34 = load i1, i1* %b1, align 1
  br i1 %b34, label %if35, label %else36

if35:                                             ; preds = %merge32
  br label %merge37

else36:                                           ; preds = %merge32
  br label %merge37

merge37:                                          ; preds = %else36, %if35
  %phi38 = phi %Range32 [ { i32 0, i32 1 }, %if35 ], [ { i32 1, i32 1000 }, %else36 ]
  store %Range32 %phi38, %Range32* %r9, align 4
  %b39 = load i1, i1* %b1, align 1
  br i1 %b39, label %if40, label %else41

if40:                                             ; preds = %merge37
  br label %merge46

else41:                                           ; preds = %merge37
  %x3242 = load i32, i32* %x324, align 4
  %x3243 = load i32, i32* %x324, align 4
  %rng44 = insertvalue %Range32 undef, i32 %x3242, 0
  %rng45 = insertvalue %Range32 %rng44, i32 %x3243, 1
  br label %merge46

merge46:                                          ; preds = %else41, %if40
  %phi47 = phi %Range32 [ { i32 0, i32 1 }, %if40 ], [ %rng45, %else41 ]
  store %Range32 %phi47, %Range32* %r9, align 4
  ret %UnitType zeroinitializer
}

; Function Attrs: noinline nounwind optnone
define %UnitType @sign_operator(i8 %x8, i8 %y8) #0 {
entry:
  %r2 = alloca i32, align 4
  %r1 = alloca i32, align 4
  %y82 = alloca i8, align 1
  %x81 = alloca i8, align 1
  store i8 %x8, i8* %x81, align 1
  store i8 %y8, i8* %y82, align 1
  %x83 = load i8, i8* %x81, align 1
  %signext = sext i8 %x83 to i16
  %div = sdiv i16 200, %signext
  %signext4 = sext i16 %div to i32
  store i32 %signext4, i32* %r1, align 4
  %y85 = load i8, i8* %y82, align 1
  %div6 = udiv i8 -56, %y85
  %zeroext = zext i8 %div6 to i32
  store i32 %zeroext, i32* %r2, align 4
  ret %UnitType zeroinitializer
}

; Function Attrs: noinline nounwind optnone
define %UnitType @unary_operator(i1 %b, i8 %x, i8 %y) #0 {
entry:
  %r16 = alloca i32, align 4
  %r15 = alloca i32, align 4
  %r14 = alloca i32, align 4
  %r13 = alloca i32, align 4
  %r12 = alloca i32, align 4
  %r11 = alloca i32, align 4
  %r10 = alloca i32, align 4
  %r9 = alloca i32, align 4
  %r8 = alloca i32, align 4
  %r7 = alloca i32, align 4
  %r6 = alloca i32, align 4
  %r5 = alloca i32, align 4
  %r4 = alloca i32, align 4
  %r3 = alloca i32, align 4
  %r2 = alloca i32, align 4
  %r1 = alloca i32, align 4
  %y3 = alloca i8, align 1
  %x2 = alloca i8, align 1
  %b1 = alloca i1, align 1
  store i1 %b, i1* %b1, align 1
  store i8 %x, i8* %x2, align 1
  store i8 %y, i8* %y3, align 1
  %b4 = load i1, i1* %b1, align 1
  br i1 %b4, label %if, label %else

if:                                               ; preds = %entry
  %x5 = load i8, i8* %x2, align 1
  %signext = sext i8 %x5 to i16
  br label %merge

else:                                             ; preds = %entry
  br label %merge

merge:                                            ; preds = %else, %if
  %phi = phi i16 [ %signext, %if ], [ -200, %else ]
  %signext6 = sext i16 %phi to i32
  store i32 %signext6, i32* %r1, align 4
  %x7 = load i8, i8* %x2, align 1
  %signext8 = sext i8 %x7 to i32
  %add = add i32 %signext8, -200
  store i32 %add, i32* %r2, align 4
  store i32 -20, i32* %r3, align 4
  store i32 20, i32* %r4, align 4
  store i32 -200, i32* %r5, align 4
  store i32 200, i32* %r6, align 4
  store i32 -21, i32* %r7, align 4
  store i32 20, i32* %r8, align 4
  store i32 -21, i32* %r9, align 4
  store i32 20, i32* %r10, align 4
  store i32 -201, i32* %r11, align 4
  store i32 200, i32* %r12, align 4
  store i32 -201, i32* %r13, align 4
  store i32 200, i32* %r14, align 4
  %x9 = load i8, i8* %x2, align 1
  %signext10 = sext i8 %x9 to i16
  %add11 = add i16 %signext10, 200
  %signext12 = sext i16 %add11 to i32
  store i32 %signext12, i32* %r15, align 4
  %y13 = load i8, i8* %y3, align 1
  %add14 = add i8 %y13, -56
  %zeroext = zext i8 %add14 to i32
  store i32 %zeroext, i32* %r16, align 4
  ret %UnitType zeroinitializer
}

; Function Attrs: noinline nounwind optnone
define i32* @pointers(i32* %p1) #0 {
entry:
  %b = alloca i1, align 1
  %bPtr2 = alloca i1**, align 8
  %bPtr1 = alloca i1*, align 8
  %test = alloca %Test1, align 8
  %xPtr = alloca i8*, align 8
  %x = alloca i8, align 1
  %equal = alloca i1, align 1
  %p2 = alloca i32*, align 8
  %p11 = alloca i32*, align 8
  store i32* %p1, i32** %p11, align 8
  %p12 = load i32*, i32** %p11, align 8
  store i32* %p12, i32** %p2, align 8
  %p13 = load i32*, i32** %p11, align 8
  %p24 = load i32*, i32** %p2, align 8
  %cmpeq = icmp eq i32* %p13, %p24
  store i1 %cmpeq, i1* %equal, align 1
  store i8 2, i8* %x, align 1
  store i8* %x, i8** %xPtr, align 8
  %xPtr5 = load i8*, i8** %xPtr, align 8
  %load = load i8, i8* %xPtr5, align 1
  %add = add i8 %load, 5
  store i8 %add, i8* %xPtr5, align 1
  %xPtr6 = load i8*, i8** %xPtr, align 8
  %load7 = load i8, i8* %xPtr6, align 1
  %signext = sext i8 %load7 to i32
  %agg = insertvalue %Test1 undef, i32 %signext, 0
  %agg8 = insertvalue %Test1 %agg, i1 true, 1
  %load9 = load %str, %str* @strStruct6, align 8
  %agg10 = insertvalue %Test1 %agg8, %str %load9, 2
  store %Test1 %agg10, %Test1* %test, align 8
  %mber = getelementptr inbounds %Test1, %Test1* %test, i64 0, i32 1
  store i1* %mber, i1** %bPtr1, align 8
  store i1** %bPtr1, i1*** %bPtr2, align 8
  %bPtr211 = load i1**, i1*** %bPtr2, align 8
  %load12 = load i1*, i1** %bPtr211, align 8
  %load13 = load i1, i1* %load12, align 1
  store i1 %load13, i1* %b, align 1
  %p114 = load i32*, i32** %p11, align 8
  ret i32* %p114
}

; Function Attrs: noinline nounwind optnone
define %UnitType @subscript(%str %s, %SubscriptTest1 %t) #0 {
entry:
  %y = alloca i8, align 1
  %x = alloca i8, align 1
  %t2 = alloca %SubscriptTest1, align 8
  %s1 = alloca %str, align 8
  store %str %s, %str* %s1, align 8
  store %SubscriptTest1 %t, %SubscriptTest1* %t2, align 8
  %s3 = load %str, %str* %s1, align 8
  %size = extractvalue %str %s3, 0
  %check = icmp uge i64 0, %size
  br i1 %check, label %failed, label %passed

failed:                                           ; preds = %entry
  call void @exit(i32 1)
  unreachable

passed:                                           ; preds = %entry
  %data = extractvalue %str %s3, 1
  %value = getelementptr inbounds i8, i8* %data, i64 0
  %load = load i8, i8* %value, align 1
  store i8 %load, i8* %x, align 1
  %t4 = load %SubscriptTest1, %SubscriptTest1* %t2, align 8
  %mber = extractvalue %SubscriptTest1 %t4, 0
  %mber5 = extractvalue %SubscriptTest2 %mber, 0
  %size6 = extractvalue %str %mber5, 0
  %check7 = icmp uge i64 5, %size6
  br i1 %check7, label %failed8, label %passed9

failed8:                                          ; preds = %passed
  call void @exit(i32 1)
  unreachable

passed9:                                          ; preds = %passed
  %data10 = extractvalue %str %mber5, 1
  %value11 = getelementptr inbounds i8, i8* %data10, i64 5
  %load12 = load i8, i8* %value11, align 1
  store i8 %load12, i8* %y, align 1
  ret %UnitType zeroinitializer
}

; Function Attrs: noinline nounwind optnone
define %UnitType @break_and_continue() #0 {
entry:
  %k = alloca i32, align 4
  %j = alloca i8, align 1
  %i = alloca i8, align 1
  br label %forCond

forCond:                                          ; preds = %forIter12, %entry
  %iter = phi i8 [ 0, %entry ], [ %inc13, %forIter12 ]
  %cmp = icmp sle i8 %iter, 10
  br i1 %cmp, label %forBody, label %forExit14

forBody:                                          ; preds = %forCond
  store i8 %iter, i8* %i, align 1
  br label %forCond1

forCond1:                                         ; preds = %forIter, %forBody
  %iter2 = phi i8 [ 0, %forBody ], [ %inc, %forIter ]
  %cmp3 = icmp sle i8 %iter2, 7
  br i1 %cmp3, label %forBody4, label %forExit

forBody4:                                         ; preds = %forCond1
  store i8 %iter2, i8* %j, align 1
  %j5 = load i8, i8* %j, align 1
  %cmpeq = icmp eq i8 %j5, 4
  br i1 %cmpeq, label %if, label %else

if:                                               ; preds = %forBody4
  br label %forIter

aftercontinue:                                    ; No predecessors!
  br label %merge

else:                                             ; preds = %forBody4
  br label %merge

merge:                                            ; preds = %else, %aftercontinue
  %phi = phi %UnitType [ zeroinitializer, %aftercontinue ], [ zeroinitializer, %else ]
  %call = call i32 @noArgs()
  br label %forIter

forIter:                                          ; preds = %merge, %if
  %inc = add i8 %iter2, 1
  br label %forCond1

forExit:                                          ; preds = %forCond1
  %i6 = load i8, i8* %i, align 1
  %cmpeq7 = icmp eq i8 %i6, 8
  br i1 %cmpeq7, label %if8, label %else9

if8:                                              ; preds = %forExit
  br label %forExit14

afterbreak:                                       ; No predecessors!
  br label %merge10

else9:                                            ; preds = %forExit
  br label %merge10

merge10:                                          ; preds = %else9, %afterbreak
  %phi11 = phi %UnitType [ zeroinitializer, %afterbreak ], [ zeroinitializer, %else9 ]
  br label %forIter12

forIter12:                                        ; preds = %merge10
  %inc13 = add i8 %iter, 1
  br label %forCond

forExit14:                                        ; preds = %if8, %forCond
  store i32 0, i32* %k, align 4
  br label %whileCond

whileCond:                                        ; preds = %merge28, %if18, %forExit14
  %k15 = load i32, i32* %k, align 4
  %cmplt = icmp slt i32 %k15, 17
  br i1 %cmplt, label %whileBody, label %whileExit

whileBody:                                        ; preds = %whileCond
  %k16 = load i32, i32* %k, align 4
  %cmpeq17 = icmp eq i32 %k16, 3
  br i1 %cmpeq17, label %if18, label %else20

if18:                                             ; preds = %whileBody
  br label %whileCond

aftercontinue19:                                  ; No predecessors!
  br label %merge28

else20:                                           ; preds = %whileBody
  %k21 = load i32, i32* %k, align 4
  %cmpeq22 = icmp eq i32 %k21, 10
  br i1 %cmpeq22, label %if23, label %else25

if23:                                             ; preds = %else20
  br label %whileExit

afterbreak24:                                     ; No predecessors!
  br label %merge26

else25:                                           ; preds = %else20
  br label %merge26

merge26:                                          ; preds = %else25, %afterbreak24
  %phi27 = phi %UnitType [ zeroinitializer, %afterbreak24 ], [ zeroinitializer, %else25 ]
  br label %merge28

merge28:                                          ; preds = %merge26, %aftercontinue19
  %phi29 = phi %UnitType [ zeroinitializer, %aftercontinue19 ], [ %phi27, %merge26 ]
  %call30 = call i32 @noArgs()
  %load = load i32, i32* %k, align 4
  %add = add i32 %load, 1
  store i32 %add, i32* %k, align 4
  br label %whileCond

whileExit:                                        ; preds = %if23, %whileCond
  ret %UnitType zeroinitializer
}

; Function Attrs: noinline nounwind optnone
define i32 @ret1(i32 %a, i32 %b) #0 {
entry:
  %b2 = alloca i32, align 4
  %a1 = alloca i32, align 4
  store i32 %a, i32* %a1, align 4
  store i32 %b, i32* %b2, align 4
  %a3 = load i32, i32* %a1, align 4
  %b4 = load i32, i32* %b2, align 4
  %cmplt = icmp slt i32 %a3, %b4
  br i1 %cmplt, label %if, label %else

if:                                               ; preds = %entry
  ret i32 -1

afterreturn:                                      ; No predecessors!
  br label %merge10

else:                                             ; preds = %entry
  %a5 = load i32, i32* %a1, align 4
  %b6 = load i32, i32* %b2, align 4
  %cmpgt = icmp sgt i32 %a5, %b6
  br i1 %cmpgt, label %if7, label %else9

if7:                                              ; preds = %else
  ret i32 1

afterreturn8:                                     ; No predecessors!
  br label %merge

else9:                                            ; preds = %else
  br label %merge

merge:                                            ; preds = %else9, %afterreturn8
  %phi = phi %UnitType [ zeroinitializer, %afterreturn8 ], [ zeroinitializer, %else9 ]
  br label %merge10

merge10:                                          ; preds = %merge, %afterreturn
  %phi11 = phi %UnitType [ zeroinitializer, %afterreturn ], [ %phi, %merge ]
  ret i32 0
}

; Function Attrs: noinline nounwind optnone
define i32 @ret2(i32 %a, i32 %b) #0 {
entry:
  %b2 = alloca i32, align 4
  %a1 = alloca i32, align 4
  store i32 %a, i32* %a1, align 4
  store i32 %b, i32* %b2, align 4
  %a3 = load i32, i32* %a1, align 4
  %b4 = load i32, i32* %b2, align 4
  %cmplt = icmp slt i32 %a3, %b4
  br i1 %cmplt, label %if, label %else

if:                                               ; preds = %entry
  ret i32 -1

afterreturn:                                      ; No predecessors!
  br label %merge10

else:                                             ; preds = %entry
  %a5 = load i32, i32* %a1, align 4
  %b6 = load i32, i32* %b2, align 4
  %cmpgt = icmp sgt i32 %a5, %b6
  br i1 %cmpgt, label %if7, label %else9

if7:                                              ; preds = %else
  ret i32 1

afterreturn8:                                     ; No predecessors!
  br label %merge

else9:                                            ; preds = %else
  br label %merge

merge:                                            ; preds = %else9, %afterreturn8
  %phi = phi %UnitType [ zeroinitializer, %afterreturn8 ], [ zeroinitializer, %else9 ]
  br label %merge10

merge10:                                          ; preds = %merge, %afterreturn
  %phi11 = phi %UnitType [ zeroinitializer, %afterreturn ], [ %phi, %merge ]
  ret i32 0
}

; Function Attrs: noinline nounwind optnone
define %UnitType @arrays1(%"[i32]" %a1) #0 {
entry:
  %n = alloca i32, align 4
  %a11 = alloca %"[i32]", align 8
  store %"[i32]" %a1, %"[i32]"* %a11, align 8
  %a12 = load %"[i32]", %"[i32]"* %a11, align 8
  %size = extractvalue %"[i32]" %a12, 0
  %check = icmp uge i64 0, %size
  br i1 %check, label %failed, label %passed

failed:                                           ; preds = %entry
  call void @exit(i32 1)
  unreachable

passed:                                           ; preds = %entry
  %data = extractvalue %"[i32]" %a12, 1
  %value = getelementptr inbounds i32, i32* %data, i64 0
  %load = load i32, i32* %value, align 4
  store i32 %load, i32* %n, align 4
  %a13 = load %"[i32]", %"[i32]"* %a11, align 8
  %size4 = extractvalue %"[i32]" %a13, 0
  %check5 = icmp uge i64 1, %size4
  br i1 %check5, label %failed6, label %passed7

failed6:                                          ; preds = %passed
  call void @exit(i32 1)
  unreachable

passed7:                                          ; preds = %passed
  %data8 = extractvalue %"[i32]" %a13, 1
  %value9 = getelementptr inbounds i32, i32* %data8, i64 1
  store i32 17, i32* %value9, align 4
  ret %UnitType zeroinitializer
}

; Function Attrs: noinline nounwind optnone
define %UnitType @arrays2() #0 {
entry:
  %array71 = alloca [5 x i8], align 1
  %r1 = alloca i8, align 1
  %array58 = alloca [12 x i32], align 4
  %array47 = alloca [3 x i32], align 4
  %s = alloca %ArrayTest, align 8
  %array37 = alloca [12 x i32], align 4
  %array27 = alloca [4 x i32], align 4
  %a5 = alloca %"[i32]", align 8
  %array17 = alloca [3 x i32], align 4
  %a4 = alloca %"[i32]", align 8
  %array6 = alloca [3 x i16], align 2
  %a3 = alloca %"[i16]", align 8
  %array3 = alloca [0 x i32], align 4
  %a2 = alloca %"[i32]", align 8
  %array = alloca [10 x i32], align 4
  %a1 = alloca %"[i32]", align 8
  %x = alloca i32, align 4
  store i32 12, i32* %x, align 4
  %x1 = load i32, i32* %x, align 4
  %startPtr = getelementptr inbounds [10 x i32], [10 x i32]* %array, i64 0, i64 0
  %endPtr = getelementptr inbounds [10 x i32], [10 x i32]* %array, i64 0, i64 10
  br label %fillBody

fillBody:                                         ; preds = %fillBody, %entry
  %phi = phi i32* [ %startPtr, %entry ], [ %nextPtr, %fillBody ]
  store i32 %x1, i32* %phi, align 4
  %nextPtr = getelementptr inbounds i32, i32* %phi, i64 1
  %atEnd = icmp eq i32* %nextPtr, %endPtr
  br i1 %atEnd, label %fillExit, label %fillBody

fillExit:                                         ; preds = %fillBody
  %arrptr = bitcast [10 x i32]* %array to i32*
  %agg = insertvalue %"[i32]" { i64 10, i32* undef }, i32* %arrptr, 1
  store %"[i32]" %agg, %"[i32]"* %a1, align 8
  %x2 = load i32, i32* %x, align 4
  %arrptr4 = bitcast [0 x i32]* %array3 to i32*
  %agg5 = insertvalue %"[i32]" { i64 0, i32* undef }, i32* %arrptr4, 1
  store %"[i32]" %agg5, %"[i32]"* %a2, align 8
  %startPtr8 = getelementptr inbounds [3 x i16], [3 x i16]* %array6, i64 0, i64 0
  %endPtr9 = getelementptr inbounds [3 x i16], [3 x i16]* %array6, i64 0, i64 3
  br label %fillBody7

fillBody7:                                        ; preds = %fillBody7, %fillExit
  %phi10 = phi i16* [ %startPtr8, %fillExit ], [ %nextPtr11, %fillBody7 ]
  store i16 201, i16* %phi10, align 2
  %nextPtr11 = getelementptr inbounds i16, i16* %phi10, i64 1
  %atEnd12 = icmp eq i16* %nextPtr11, %endPtr9
  br i1 %atEnd12, label %fillExit13, label %fillBody7

fillExit13:                                       ; preds = %fillBody7
  %arrptr14 = bitcast [3 x i16]* %array6 to i16*
  %agg15 = insertvalue %"[i16]" { i64 3, i16* undef }, i16* %arrptr14, 1
  store %"[i16]" %agg15, %"[i16]"* %a3, align 8
  %x16 = load i32, i32* %x, align 4
  %mul = mul i32 %x16, 2
  %add = add i32 1, %mul
  %startPtr19 = getelementptr inbounds [3 x i32], [3 x i32]* %array17, i64 0, i64 0
  %endPtr20 = getelementptr inbounds [3 x i32], [3 x i32]* %array17, i64 0, i64 3
  br label %fillBody18

fillBody18:                                       ; preds = %fillBody18, %fillExit13
  %phi21 = phi i32* [ %startPtr19, %fillExit13 ], [ %nextPtr22, %fillBody18 ]
  store i32 %add, i32* %phi21, align 4
  %nextPtr22 = getelementptr inbounds i32, i32* %phi21, i64 1
  %atEnd23 = icmp eq i32* %nextPtr22, %endPtr20
  br i1 %atEnd23, label %fillExit24, label %fillBody18

fillExit24:                                       ; preds = %fillBody18
  %arrptr25 = bitcast [3 x i32]* %array17 to i32*
  %agg26 = insertvalue %"[i32]" { i64 3, i32* undef }, i32* %arrptr25, 1
  store %"[i32]" %agg26, %"[i32]"* %a4, align 8
  %startPtr29 = getelementptr inbounds [4 x i32], [4 x i32]* %array27, i64 0, i64 0
  %endPtr30 = getelementptr inbounds [4 x i32], [4 x i32]* %array27, i64 0, i64 4
  br label %fillBody28

fillBody28:                                       ; preds = %fillBody28, %fillExit24
  %phi31 = phi i32* [ %startPtr29, %fillExit24 ], [ %nextPtr32, %fillBody28 ]
  store i32 0, i32* %phi31, align 4
  %nextPtr32 = getelementptr inbounds i32, i32* %phi31, i64 1
  %atEnd33 = icmp eq i32* %nextPtr32, %endPtr30
  br i1 %atEnd33, label %fillExit34, label %fillBody28

fillExit34:                                       ; preds = %fillBody28
  %arrptr35 = bitcast [4 x i32]* %array27 to i32*
  %agg36 = insertvalue %"[i32]" { i64 4, i32* undef }, i32* %arrptr35, 1
  store %"[i32]" %agg36, %"[i32]"* %a5, align 8
  %startPtr39 = getelementptr inbounds [12 x i32], [12 x i32]* %array37, i64 0, i64 0
  %endPtr40 = getelementptr inbounds [12 x i32], [12 x i32]* %array37, i64 0, i64 12
  br label %fillBody38

fillBody38:                                       ; preds = %fillBody38, %fillExit34
  %phi41 = phi i32* [ %startPtr39, %fillExit34 ], [ %nextPtr42, %fillBody38 ]
  store i32 1, i32* %phi41, align 4
  %nextPtr42 = getelementptr inbounds i32, i32* %phi41, i64 1
  %atEnd43 = icmp eq i32* %nextPtr42, %endPtr40
  br i1 %atEnd43, label %fillExit44, label %fillBody38

fillExit44:                                       ; preds = %fillBody38
  %arrptr45 = bitcast [12 x i32]* %array37 to i32*
  %agg46 = insertvalue %"[i32]" { i64 12, i32* undef }, i32* %arrptr45, 1
  %call = call %UnitType @arrays1(%"[i32]" %agg46)
  %startPtr49 = getelementptr inbounds [3 x i32], [3 x i32]* %array47, i64 0, i64 0
  %endPtr50 = getelementptr inbounds [3 x i32], [3 x i32]* %array47, i64 0, i64 3
  br label %fillBody48

fillBody48:                                       ; preds = %fillBody48, %fillExit44
  %phi51 = phi i32* [ %startPtr49, %fillExit44 ], [ %nextPtr52, %fillBody48 ]
  store i32 19, i32* %phi51, align 4
  %nextPtr52 = getelementptr inbounds i32, i32* %phi51, i64 1
  %atEnd53 = icmp eq i32* %nextPtr52, %endPtr50
  br i1 %atEnd53, label %fillExit54, label %fillBody48

fillExit54:                                       ; preds = %fillBody48
  %arrptr55 = bitcast [3 x i32]* %array47 to i32*
  %agg56 = insertvalue %"[i32]" { i64 3, i32* undef }, i32* %arrptr55, 1
  %agg57 = insertvalue %ArrayTest undef, %"[i32]" %agg56, 0
  store %ArrayTest %agg57, %ArrayTest* %s, align 8
  %startPtr60 = getelementptr inbounds [12 x i32], [12 x i32]* %array58, i64 0, i64 0
  %endPtr61 = getelementptr inbounds [12 x i32], [12 x i32]* %array58, i64 0, i64 12
  br label %fillBody59

fillBody59:                                       ; preds = %fillBody59, %fillExit54
  %phi62 = phi i32* [ %startPtr60, %fillExit54 ], [ %nextPtr63, %fillBody59 ]
  store i32 123, i32* %phi62, align 4
  %nextPtr63 = getelementptr inbounds i32, i32* %phi62, i64 1
  %atEnd64 = icmp eq i32* %nextPtr63, %endPtr61
  br i1 %atEnd64, label %fillExit65, label %fillBody59

fillExit65:                                       ; preds = %fillBody59
  %arrptr66 = bitcast [12 x i32]* %array58 to i32*
  %agg67 = insertvalue %"[i32]" { i64 12, i32* undef }, i32* %arrptr66, 1
  %size = extractvalue %"[i32]" %agg67, 0
  %data = extractvalue %"[i32]" %agg67, 1
  %endok = icmp ult i64 7, %size
  %checkend = select i1 %endok, i64 7, i64 %size
  %startok = icmp ule i64 4, %checkend
  %checkstart = select i1 %startok, i64 4, i64 %checkend
  %sub = sub i64 %checkend, %checkstart
  %ptr = getelementptr inbounds i32, i32* %data, i64 %checkstart
  %agg68 = insertvalue %"[i32]" undef, i64 %sub, 0
  %agg69 = insertvalue %"[i32]" %agg68, i32* %ptr, 1
  %call70 = call %UnitType @arrays1(%"[i32]" %agg69)
  %startPtr73 = getelementptr inbounds [5 x i8], [5 x i8]* %array71, i64 0, i64 0
  %endPtr74 = getelementptr inbounds [5 x i8], [5 x i8]* %array71, i64 0, i64 5
  br label %fillBody72

fillBody72:                                       ; preds = %fillBody72, %fillExit65
  %phi75 = phi i8* [ %startPtr73, %fillExit65 ], [ %nextPtr76, %fillBody72 ]
  store i8 7, i8* %phi75, align 1
  %nextPtr76 = getelementptr inbounds i8, i8* %phi75, i64 1
  %atEnd77 = icmp eq i8* %nextPtr76, %endPtr74
  br i1 %atEnd77, label %fillExit78, label %fillBody72

fillExit78:                                       ; preds = %fillBody72
  %arrptr79 = bitcast [5 x i8]* %array71 to i8*
  %agg80 = insertvalue %"[i8]" { i64 5, i8* undef }, i8* %arrptr79, 1
  %size81 = extractvalue %"[i8]" %agg80, 0
  %check = icmp uge i64 3, %size81
  br i1 %check, label %failed, label %passed

failed:                                           ; preds = %fillExit78
  call void @exit(i32 1)
  unreachable

passed:                                           ; preds = %fillExit78
  %data82 = extractvalue %"[i8]" %agg80, 1
  %value = getelementptr inbounds i8, i8* %data82, i64 3
  %load = load i8, i8* %value, align 1
  store i8 %load, i8* %r1, align 1
  ret %UnitType zeroinitializer
}

; Function Attrs: noinline nounwind optnone
define %UnitType @arrays3() #0 {
entry:
  %array72 = alloca [3 x i32], align 4
  %array68 = alloca [2 x i32], align 4
  %array67 = alloca [2 x %"[i32]"], align 8
  %a7 = alloca %"[[i32]]", align 8
  %array59 = alloca [2 x i32], align 4
  %array53 = alloca [2 x i32], align 4
  %array52 = alloca [2 x %"[i32]"], align 8
  %a6 = alloca %"[[i32]]", align 8
  %array45 = alloca [2 x i16], align 2
  %r1 = alloca i16, align 2
  %array33 = alloca [5 x i32], align 4
  %array26 = alloca [3 x i32], align 4
  %s = alloca %ArrayTest, align 8
  %array20 = alloca [3 x i32], align 4
  %array13 = alloca [2 x i32], align 4
  %a4 = alloca %"[i32]", align 8
  %array7 = alloca [2 x i16], align 2
  %a3 = alloca %"[i16]", align 8
  %array2 = alloca [2 x i8], align 1
  %a2 = alloca %"[u8]", align 8
  %array = alloca [2 x i8], align 1
  %a1 = alloca %"[i8]", align 8
  %y = alloca i32, align 4
  %x = alloca i16, align 2
  store i16 12, i16* %x, align 2
  store i32 123, i32* %y, align 4
  %ptr = getelementptr inbounds [2 x i8], [2 x i8]* %array, i64 0, i64 0
  store i8 1, i8* %ptr, align 1
  %ptr1 = getelementptr inbounds [2 x i8], [2 x i8]* %array, i64 0, i64 1
  store i8 -2, i8* %ptr1, align 1
  %arrptr = bitcast [2 x i8]* %array to i8*
  %agg = insertvalue %"[i8]" { i64 2, i8* undef }, i8* %arrptr, 1
  store %"[i8]" %agg, %"[i8]"* %a1, align 8
  %ptr3 = getelementptr inbounds [2 x i8], [2 x i8]* %array2, i64 0, i64 0
  store i8 1, i8* %ptr3, align 1
  %ptr4 = getelementptr inbounds [2 x i8], [2 x i8]* %array2, i64 0, i64 1
  store i8 2, i8* %ptr4, align 1
  %arrptr5 = bitcast [2 x i8]* %array2 to i8*
  %agg6 = insertvalue %"[u8]" { i64 2, i8* undef }, i8* %arrptr5, 1
  store %"[u8]" %agg6, %"[u8]"* %a2, align 8
  %ptr8 = getelementptr inbounds [2 x i16], [2 x i16]* %array7, i64 0, i64 0
  store i16 20, i16* %ptr8, align 2
  %x9 = load i16, i16* %x, align 2
  %add = add i16 %x9, 1
  %ptr10 = getelementptr inbounds [2 x i16], [2 x i16]* %array7, i64 0, i64 1
  store i16 %add, i16* %ptr10, align 2
  %arrptr11 = bitcast [2 x i16]* %array7 to i16*
  %agg12 = insertvalue %"[i16]" { i64 2, i16* undef }, i16* %arrptr11, 1
  store %"[i16]" %agg12, %"[i16]"* %a3, align 8
  %ptr14 = getelementptr inbounds [2 x i32], [2 x i32]* %array13, i64 0, i64 0
  store i32 3, i32* %ptr14, align 4
  %y15 = load i32, i32* %y, align 4
  %mul = mul i32 %y15, 2
  %add16 = add i32 1, %mul
  %ptr17 = getelementptr inbounds [2 x i32], [2 x i32]* %array13, i64 0, i64 1
  store i32 %add16, i32* %ptr17, align 4
  %arrptr18 = bitcast [2 x i32]* %array13 to i32*
  %agg19 = insertvalue %"[i32]" { i64 2, i32* undef }, i32* %arrptr18, 1
  store %"[i32]" %agg19, %"[i32]"* %a4, align 8
  %ptr21 = getelementptr inbounds [3 x i32], [3 x i32]* %array20, i64 0, i64 0
  store i32 10, i32* %ptr21, align 4
  %ptr22 = getelementptr inbounds [3 x i32], [3 x i32]* %array20, i64 0, i64 1
  store i32 20, i32* %ptr22, align 4
  %ptr23 = getelementptr inbounds [3 x i32], [3 x i32]* %array20, i64 0, i64 2
  store i32 30, i32* %ptr23, align 4
  %arrptr24 = bitcast [3 x i32]* %array20 to i32*
  %agg25 = insertvalue %"[i32]" { i64 3, i32* undef }, i32* %arrptr24, 1
  %call = call %UnitType @arrays1(%"[i32]" %agg25)
  %ptr27 = getelementptr inbounds [3 x i32], [3 x i32]* %array26, i64 0, i64 0
  store i32 1, i32* %ptr27, align 4
  %ptr28 = getelementptr inbounds [3 x i32], [3 x i32]* %array26, i64 0, i64 1
  store i32 2, i32* %ptr28, align 4
  %ptr29 = getelementptr inbounds [3 x i32], [3 x i32]* %array26, i64 0, i64 2
  store i32 3, i32* %ptr29, align 4
  %arrptr30 = bitcast [3 x i32]* %array26 to i32*
  %agg31 = insertvalue %"[i32]" { i64 3, i32* undef }, i32* %arrptr30, 1
  %agg32 = insertvalue %ArrayTest undef, %"[i32]" %agg31, 0
  store %ArrayTest %agg32, %ArrayTest* %s, align 8
  %ptr34 = getelementptr inbounds [5 x i32], [5 x i32]* %array33, i64 0, i64 0
  store i32 10, i32* %ptr34, align 4
  %ptr35 = getelementptr inbounds [5 x i32], [5 x i32]* %array33, i64 0, i64 1
  store i32 20, i32* %ptr35, align 4
  %ptr36 = getelementptr inbounds [5 x i32], [5 x i32]* %array33, i64 0, i64 2
  store i32 30, i32* %ptr36, align 4
  %ptr37 = getelementptr inbounds [5 x i32], [5 x i32]* %array33, i64 0, i64 3
  store i32 40, i32* %ptr37, align 4
  %ptr38 = getelementptr inbounds [5 x i32], [5 x i32]* %array33, i64 0, i64 4
  store i32 50, i32* %ptr38, align 4
  %arrptr39 = bitcast [5 x i32]* %array33 to i32*
  %agg40 = insertvalue %"[i32]" { i64 5, i32* undef }, i32* %arrptr39, 1
  %size = extractvalue %"[i32]" %agg40, 0
  %data = extractvalue %"[i32]" %agg40, 1
  %endok = icmp ult i64 4, %size
  %checkend = select i1 %endok, i64 4, i64 %size
  %startok = icmp ule i64 1, %checkend
  %checkstart = select i1 %startok, i64 1, i64 %checkend
  %sub = sub i64 %checkend, %checkstart
  %ptr41 = getelementptr inbounds i32, i32* %data, i64 %checkstart
  %agg42 = insertvalue %"[i32]" undef, i64 %sub, 0
  %agg43 = insertvalue %"[i32]" %agg42, i32* %ptr41, 1
  %call44 = call %UnitType @arrays1(%"[i32]" %agg43)
  %ptr46 = getelementptr inbounds [2 x i16], [2 x i16]* %array45, i64 0, i64 0
  store i16 100, i16* %ptr46, align 2
  %ptr47 = getelementptr inbounds [2 x i16], [2 x i16]* %array45, i64 0, i64 1
  store i16 200, i16* %ptr47, align 2
  %arrptr48 = bitcast [2 x i16]* %array45 to i16*
  %agg49 = insertvalue %"[i16]" { i64 2, i16* undef }, i16* %arrptr48, 1
  %size50 = extractvalue %"[i16]" %agg49, 0
  %check = icmp uge i64 0, %size50
  br i1 %check, label %failed, label %passed

failed:                                           ; preds = %entry
  call void @exit(i32 1)
  unreachable

passed:                                           ; preds = %entry
  %data51 = extractvalue %"[i16]" %agg49, 1
  %value = getelementptr inbounds i16, i16* %data51, i64 0
  %load = load i16, i16* %value, align 2
  store i16 %load, i16* %r1, align 2
  %ptr54 = getelementptr inbounds [2 x i32], [2 x i32]* %array53, i64 0, i64 0
  store i32 10, i32* %ptr54, align 4
  %ptr55 = getelementptr inbounds [2 x i32], [2 x i32]* %array53, i64 0, i64 1
  store i32 20, i32* %ptr55, align 4
  %arrptr56 = bitcast [2 x i32]* %array53 to i32*
  %agg57 = insertvalue %"[i32]" { i64 2, i32* undef }, i32* %arrptr56, 1
  %ptr58 = getelementptr inbounds [2 x %"[i32]"], [2 x %"[i32]"]* %array52, i64 0, i64 0
  store %"[i32]" %agg57, %"[i32]"* %ptr58, align 8
  %ptr60 = getelementptr inbounds [2 x i32], [2 x i32]* %array59, i64 0, i64 0
  store i32 30, i32* %ptr60, align 4
  %ptr61 = getelementptr inbounds [2 x i32], [2 x i32]* %array59, i64 0, i64 1
  store i32 40, i32* %ptr61, align 4
  %arrptr62 = bitcast [2 x i32]* %array59 to i32*
  %agg63 = insertvalue %"[i32]" { i64 2, i32* undef }, i32* %arrptr62, 1
  %ptr64 = getelementptr inbounds [2 x %"[i32]"], [2 x %"[i32]"]* %array52, i64 0, i64 1
  store %"[i32]" %agg63, %"[i32]"* %ptr64, align 8
  %arrptr65 = bitcast [2 x %"[i32]"]* %array52 to %"[i32]"*
  %agg66 = insertvalue %"[[i32]]" { i64 2, %"[i32]"* undef }, %"[i32]"* %arrptr65, 1
  store %"[[i32]]" %agg66, %"[[i32]]"* %a6, align 8
  %startPtr = getelementptr inbounds [2 x i32], [2 x i32]* %array68, i64 0, i64 0
  %endPtr = getelementptr inbounds [2 x i32], [2 x i32]* %array68, i64 0, i64 2
  br label %fillBody

fillBody:                                         ; preds = %fillBody, %passed
  %phi = phi i32* [ %startPtr, %passed ], [ %nextPtr, %fillBody ]
  store i32 8, i32* %phi, align 4
  %nextPtr = getelementptr inbounds i32, i32* %phi, i64 1
  %atEnd = icmp eq i32* %nextPtr, %endPtr
  br i1 %atEnd, label %fillExit, label %fillBody

fillExit:                                         ; preds = %fillBody
  %arrptr69 = bitcast [2 x i32]* %array68 to i32*
  %agg70 = insertvalue %"[i32]" { i64 2, i32* undef }, i32* %arrptr69, 1
  %ptr71 = getelementptr inbounds [2 x %"[i32]"], [2 x %"[i32]"]* %array67, i64 0, i64 0
  store %"[i32]" %agg70, %"[i32]"* %ptr71, align 8
  %startPtr74 = getelementptr inbounds [3 x i32], [3 x i32]* %array72, i64 0, i64 0
  %endPtr75 = getelementptr inbounds [3 x i32], [3 x i32]* %array72, i64 0, i64 3
  br label %fillBody73

fillBody73:                                       ; preds = %fillBody73, %fillExit
  %phi76 = phi i32* [ %startPtr74, %fillExit ], [ %nextPtr77, %fillBody73 ]
  store i32 9, i32* %phi76, align 4
  %nextPtr77 = getelementptr inbounds i32, i32* %phi76, i64 1
  %atEnd78 = icmp eq i32* %nextPtr77, %endPtr75
  br i1 %atEnd78, label %fillExit79, label %fillBody73

fillExit79:                                       ; preds = %fillBody73
  %arrptr80 = bitcast [3 x i32]* %array72 to i32*
  %agg81 = insertvalue %"[i32]" { i64 3, i32* undef }, i32* %arrptr80, 1
  %ptr82 = getelementptr inbounds [2 x %"[i32]"], [2 x %"[i32]"]* %array67, i64 0, i64 1
  store %"[i32]" %agg81, %"[i32]"* %ptr82, align 8
  %arrptr83 = bitcast [2 x %"[i32]"]* %array67 to %"[i32]"*
  %agg84 = insertvalue %"[[i32]]" { i64 2, %"[i32]"* undef }, %"[i32]"* %arrptr83, 1
  store %"[[i32]]" %agg84, %"[[i32]]"* %a7, align 8
  ret %UnitType zeroinitializer
}

; Function Attrs: noinline nounwind optnone
define %UnitType @slices(%"[i32]" %array, %str %s, i32 %start, i32 %end) #0 {
entry:
  %s1 = alloca %str, align 8
  %a3 = alloca %"[i32]", align 8
  %a2 = alloca %"[i32]", align 8
  %a1 = alloca %"[i32]", align 8
  %end4 = alloca i32, align 4
  %start3 = alloca i32, align 4
  %s2 = alloca %str, align 8
  %array1 = alloca %"[i32]", align 8
  store %"[i32]" %array, %"[i32]"* %array1, align 8
  store %str %s, %str* %s2, align 8
  store i32 %start, i32* %start3, align 4
  store i32 %end, i32* %end4, align 4
  %array5 = load %"[i32]", %"[i32]"* %array1, align 8
  %start6 = load i32, i32* %start3, align 4
  %end7 = load i32, i32* %end4, align 4
  %rng = insertvalue %Range32 undef, i32 %start6, 0
  %rng8 = insertvalue %Range32 %rng, i32 %end7, 1
  %size = extractvalue %"[i32]" %array5, 0
  %data = extractvalue %"[i32]" %array5, 1
  %start9 = extractvalue %Range32 %rng8, 0
  %zeroext = zext i32 %start9 to i64
  %end10 = extractvalue %Range32 %rng8, 1
  %zeroext11 = zext i32 %end10 to i64
  %add = add i64 %zeroext11, 1
  %endok = icmp ult i64 %add, %size
  %checkend = select i1 %endok, i64 %add, i64 %size
  %startok = icmp ule i64 %zeroext, %checkend
  %checkstart = select i1 %startok, i64 %zeroext, i64 %checkend
  %sub = sub i64 %checkend, %checkstart
  %ptr = getelementptr inbounds i32, i32* %data, i64 %checkstart
  %agg = insertvalue %"[i32]" undef, i64 %sub, 0
  %agg12 = insertvalue %"[i32]" %agg, i32* %ptr, 1
  store %"[i32]" %agg12, %"[i32]"* %a1, align 8
  %array13 = load %"[i32]", %"[i32]"* %array1, align 8
  %start14 = load i32, i32* %start3, align 4
  %end15 = load i32, i32* %end4, align 4
  %rng16 = insertvalue %Range32 undef, i32 %start14, 0
  %rng17 = insertvalue %Range32 %rng16, i32 %end15, 1
  %size18 = extractvalue %"[i32]" %array13, 0
  %data19 = extractvalue %"[i32]" %array13, 1
  %start20 = extractvalue %Range32 %rng17, 0
  %zeroext21 = zext i32 %start20 to i64
  %end22 = extractvalue %Range32 %rng17, 1
  %zeroext23 = zext i32 %end22 to i64
  %endok24 = icmp ult i64 %zeroext23, %size18
  %checkend25 = select i1 %endok24, i64 %zeroext23, i64 %size18
  %startok26 = icmp ule i64 %zeroext21, %checkend25
  %checkstart27 = select i1 %startok26, i64 %zeroext21, i64 %checkend25
  %sub28 = sub i64 %checkend25, %checkstart27
  %ptr29 = getelementptr inbounds i32, i32* %data19, i64 %checkstart27
  %agg30 = insertvalue %"[i32]" undef, i64 %sub28, 0
  %agg31 = insertvalue %"[i32]" %agg30, i32* %ptr29, 1
  store %"[i32]" %agg31, %"[i32]"* %a2, align 8
  %a232 = load %"[i32]", %"[i32]"* %a2, align 8
  %start33 = load i32, i32* %start3, align 4
  %add34 = add i32 %start33, 1
  %end35 = load i32, i32* %end4, align 4
  %sub36 = sub i32 %end35, 1
  %rng37 = insertvalue %Range32 undef, i32 %add34, 0
  %rng38 = insertvalue %Range32 %rng37, i32 %sub36, 1
  %size39 = extractvalue %"[i32]" %a232, 0
  %data40 = extractvalue %"[i32]" %a232, 1
  %start41 = extractvalue %Range32 %rng38, 0
  %zeroext42 = zext i32 %start41 to i64
  %end43 = extractvalue %Range32 %rng38, 1
  %zeroext44 = zext i32 %end43 to i64
  %add45 = add i64 %zeroext44, 1
  %endok46 = icmp ult i64 %add45, %size39
  %checkend47 = select i1 %endok46, i64 %add45, i64 %size39
  %startok48 = icmp ule i64 %zeroext42, %checkend47
  %checkstart49 = select i1 %startok48, i64 %zeroext42, i64 %checkend47
  %sub50 = sub i64 %checkend47, %checkstart49
  %ptr51 = getelementptr inbounds i32, i32* %data40, i64 %checkstart49
  %agg52 = insertvalue %"[i32]" undef, i64 %sub50, 0
  %agg53 = insertvalue %"[i32]" %agg52, i32* %ptr51, 1
  store %"[i32]" %agg53, %"[i32]"* %a3, align 8
  %s54 = load %str, %str* %s2, align 8
  %start55 = load i32, i32* %start3, align 4
  %end56 = load i32, i32* %end4, align 4
  %rng57 = insertvalue %Range32 undef, i32 %start55, 0
  %rng58 = insertvalue %Range32 %rng57, i32 %end56, 1
  %size59 = extractvalue %str %s54, 0
  %data60 = extractvalue %str %s54, 1
  %start61 = extractvalue %Range32 %rng58, 0
  %zeroext62 = zext i32 %start61 to i64
  %end63 = extractvalue %Range32 %rng58, 1
  %zeroext64 = zext i32 %end63 to i64
  %add65 = add i64 %zeroext64, 1
  %endok66 = icmp ult i64 %add65, %size59
  %checkend67 = select i1 %endok66, i64 %add65, i64 %size59
  %startok68 = icmp ule i64 %zeroext62, %checkend67
  %checkstart69 = select i1 %startok68, i64 %zeroext62, i64 %checkend67
  %sub70 = sub i64 %checkend67, %checkstart69
  %ptr71 = getelementptr inbounds i8, i8* %data60, i64 %checkstart69
  %agg72 = insertvalue %str undef, i64 %sub70, 0
  %agg73 = insertvalue %str %agg72, i8* %ptr71, 1
  store %str %agg73, %str* %s1, align 8
  ret %UnitType zeroinitializer
}

; Function Attrs: noinline nounwind optnone
define %UnitType @casts(i16 %x, i16 %y, i1 %b) #0 {
entry:
  %c12 = alloca i8, align 1
  %c11 = alloca i8, align 1
  %c10 = alloca i8, align 1
  %c9 = alloca i1, align 1
  %c8 = alloca i1, align 1
  %c7 = alloca i1, align 1
  %c6 = alloca i32, align 4
  %c5 = alloca i32, align 4
  %c4 = alloca i32, align 4
  %c3 = alloca i32, align 4
  %c2 = alloca i32, align 4
  %c1 = alloca i32, align 4
  %b3 = alloca i1, align 1
  %y2 = alloca i16, align 2
  %x1 = alloca i16, align 2
  store i16 %x, i16* %x1, align 2
  store i16 %y, i16* %y2, align 2
  store i1 %b, i1* %b3, align 1
  %x4 = load i16, i16* %x1, align 2
  %cast = sext i16 %x4 to i32
  store i32 %cast, i32* %c1, align 4
  %y5 = load i16, i16* %y2, align 2
  %cast6 = zext i16 %y5 to i32
  store i32 %cast6, i32* %c2, align 4
  %b7 = load i1, i1* %b3, align 1
  %cast8 = zext i1 %b7 to i32
  store i32 %cast8, i32* %c3, align 4
  %x9 = load i16, i16* %x1, align 2
  %cast10 = sext i16 %x9 to i32
  store i32 %cast10, i32* %c4, align 4
  %y11 = load i16, i16* %y2, align 2
  %cast12 = zext i16 %y11 to i32
  store i32 %cast12, i32* %c5, align 4
  %b13 = load i1, i1* %b3, align 1
  %cast14 = zext i1 %b13 to i32
  store i32 %cast14, i32* %c6, align 4
  %x15 = load i16, i16* %x1, align 2
  %cast16 = icmp ne i16 %x15, 0
  store i1 %cast16, i1* %c7, align 1
  %y17 = load i16, i16* %y2, align 2
  %cast18 = icmp ne i16 %y17, 0
  store i1 %cast18, i1* %c8, align 1
  %b19 = load i1, i1* %b3, align 1
  store i1 %b19, i1* %c9, align 1
  %x20 = load i16, i16* %x1, align 2
  %cast21 = trunc i16 %x20 to i8
  store i8 %cast21, i8* %c10, align 1
  %y22 = load i16, i16* %y2, align 2
  %cast23 = trunc i16 %y22 to i8
  store i8 %cast23, i8* %c11, align 1
  %b24 = load i1, i1* %b3, align 1
  %cast25 = zext i1 %b24 to i8
  store i8 %cast25, i8* %c12, align 1
  ret %UnitType zeroinitializer
}

; Function Attrs: noinline nounwind optnone
define %UnitType @consts() #0 {
entry:
  %array29 = alloca [13 x i32], align 4
  %a1 = alloca %"[i32]", align 8
  %array19 = alloca [3 x i16], align 2
  %v35 = alloca %"[i16]", align 8
  %array14 = alloca [2 x i8], align 1
  %v34 = alloca %"[i8]", align 8
  %v33 = alloca %str, align 8
  %v32 = alloca %str, align 8
  %v31 = alloca %str, align 8
  %v30 = alloca %str, align 8
  %v29 = alloca i16, align 2
  %v28 = alloca i8, align 1
  %array7 = alloca [5 x i16], align 2
  %v27 = alloca %"[i16]", align 8
  %array = alloca [3 x i8], align 1
  %v26 = alloca %"[i8]", align 8
  %v25 = alloca i32, align 4
  %v24 = alloca i32, align 4
  %v23 = alloca %Range16, align 8
  %v22 = alloca %Range32, align 8
  %v21 = alloca i32, align 4
  %v20 = alloca i64, align 8
  %v19 = alloca %Test2, align 8
  %v18 = alloca i16, align 2
  %v17 = alloca i16, align 2
  %v16 = alloca i8, align 1
  %v15 = alloca i8, align 1
  %v14 = alloca i1, align 1
  %v13 = alloca i1, align 1
  %v12 = alloca i1, align 1
  %v11 = alloca i1, align 1
  %v10 = alloca i32, align 4
  %v9 = alloca i8, align 1
  %v8 = alloca i16, align 2
  %v7 = alloca %str, align 8
  %v6 = alloca i32, align 4
  %v5 = alloca i8, align 1
  %v4 = alloca i1, align 1
  %v3 = alloca i32, align 4
  %v2 = alloca i16, align 2
  %v1 = alloca i8, align 1
  store i8 12, i8* %v1, align 1
  store i16 1000, i16* %v2, align 2
  store i32 1000, i32* %v3, align 4
  store i1 true, i1* %v4, align 1
  store i8 9, i8* %v5, align 1
  store i32 122, i32* %v6, align 4
  %load = load %str, %str* @strStruct6, align 8
  store %str %load, %str* %v7, align 8
  store i16 7, i16* %v8, align 2
  store i8 29, i8* %v9, align 1
  store i32 8, i32* %v10, align 4
  store i1 true, i1* %v11, align 1
  store i1 true, i1* %v12, align 1
  store i1 false, i1* %v13, align 1
  store i1 true, i1* %v14, align 1
  store i8 1, i8* %v15, align 1
  store i8 -24, i8* %v16, align 1
  store i16 100, i16* %v17, align 2
  store i16 -1, i16* %v18, align 2
  %load1 = load %str, %str* @strStruct6, align 8
  %agg = insertvalue %Test1 { i32 7, i1 true, %str undef }, %str %load1, 2
  %agg2 = insertvalue %Test2 undef, %Test1 %agg, 0
  %agg3 = insertvalue %Test2 %agg2, i32 1012, 1
  store %Test2 %agg3, %Test2* %v19, align 8
  store i64 3, i64* %v20, align 8
  store i32 7, i32* %v21, align 4
  store %Range32 { i32 12, i32 1000 }, %Range32* %v22, align 4
  store %Range16 { i16 -4, i16 200 }, %Range16* %v23, align 2
  store i32 12, i32* %v24, align 4
  store i32 500, i32* %v25, align 4
  %ptr = getelementptr inbounds [3 x i8], [3 x i8]* %array, i64 0, i64 0
  store i8 10, i8* %ptr, align 1
  %ptr4 = getelementptr inbounds [3 x i8], [3 x i8]* %array, i64 0, i64 1
  store i8 11, i8* %ptr4, align 1
  %ptr5 = getelementptr inbounds [3 x i8], [3 x i8]* %array, i64 0, i64 2
  store i8 12, i8* %ptr5, align 1
  %arrptr = bitcast [3 x i8]* %array to i8*
  %agg6 = insertvalue %"[i8]" { i64 3, i8* undef }, i8* %arrptr, 1
  store %"[i8]" %agg6, %"[i8]"* %v26, align 8
  %startPtr = getelementptr inbounds [5 x i16], [5 x i16]* %array7, i64 0, i64 0
  %endPtr = getelementptr inbounds [5 x i16], [5 x i16]* %array7, i64 0, i64 5
  br label %fillBody

fillBody:                                         ; preds = %fillBody, %entry
  %phi = phi i16* [ %startPtr, %entry ], [ %nextPtr, %fillBody ]
  store i16 -100, i16* %phi, align 2
  %nextPtr = getelementptr inbounds i16, i16* %phi, i64 1
  %atEnd = icmp eq i16* %nextPtr, %endPtr
  br i1 %atEnd, label %fillExit, label %fillBody

fillExit:                                         ; preds = %fillBody
  %arrptr8 = bitcast [5 x i16]* %array7 to i16*
  %agg9 = insertvalue %"[i16]" { i64 5, i16* undef }, i16* %arrptr8, 1
  store %"[i16]" %agg9, %"[i16]"* %v27, align 8
  store i8 12, i8* %v28, align 1
  store i16 -100, i16* %v29, align 2
  %load10 = load %str, %str* @strStruct7, align 8
  store %str %load10, %str* %v30, align 8
  %load11 = load %str, %str* @strStruct8, align 8
  store %str %load11, %str* %v31, align 8
  %load12 = load %str, %str* @strStruct9, align 8
  store %str %load12, %str* %v32, align 8
  %load13 = load %str, %str* @strStruct0, align 8
  store %str %load13, %str* %v33, align 8
  %ptr15 = getelementptr inbounds [2 x i8], [2 x i8]* %array14, i64 0, i64 0
  store i8 11, i8* %ptr15, align 1
  %ptr16 = getelementptr inbounds [2 x i8], [2 x i8]* %array14, i64 0, i64 1
  store i8 12, i8* %ptr16, align 1
  %arrptr17 = bitcast [2 x i8]* %array14 to i8*
  %agg18 = insertvalue %"[i8]" { i64 2, i8* undef }, i8* %arrptr17, 1
  store %"[i8]" %agg18, %"[i8]"* %v34, align 8
  %startPtr21 = getelementptr inbounds [3 x i16], [3 x i16]* %array19, i64 0, i64 0
  %endPtr22 = getelementptr inbounds [3 x i16], [3 x i16]* %array19, i64 0, i64 3
  br label %fillBody20

fillBody20:                                       ; preds = %fillBody20, %fillExit
  %phi23 = phi i16* [ %startPtr21, %fillExit ], [ %nextPtr24, %fillBody20 ]
  store i16 -100, i16* %phi23, align 2
  %nextPtr24 = getelementptr inbounds i16, i16* %phi23, i64 1
  %atEnd25 = icmp eq i16* %nextPtr24, %endPtr22
  br i1 %atEnd25, label %fillExit26, label %fillBody20

fillExit26:                                       ; preds = %fillBody20
  %arrptr27 = bitcast [3 x i16]* %array19 to i16*
  %agg28 = insertvalue %"[i16]" { i64 3, i16* undef }, i16* %arrptr27, 1
  store %"[i16]" %agg28, %"[i16]"* %v35, align 8
  %startPtr31 = getelementptr inbounds [13 x i32], [13 x i32]* %array29, i64 0, i64 0
  %endPtr32 = getelementptr inbounds [13 x i32], [13 x i32]* %array29, i64 0, i64 13
  br label %fillBody30

fillBody30:                                       ; preds = %fillBody30, %fillExit26
  %phi33 = phi i32* [ %startPtr31, %fillExit26 ], [ %nextPtr34, %fillBody30 ]
  store i32 0, i32* %phi33, align 4
  %nextPtr34 = getelementptr inbounds i32, i32* %phi33, i64 1
  %atEnd35 = icmp eq i32* %nextPtr34, %endPtr32
  br i1 %atEnd35, label %fillExit36, label %fillBody30

fillExit36:                                       ; preds = %fillBody30
  %arrptr37 = bitcast [13 x i32]* %array29 to i32*
  %agg38 = insertvalue %"[i32]" { i64 13, i32* undef }, i32* %arrptr37, 1
  store %"[i32]" %agg38, %"[i32]"* %a1, align 8
  ret %UnitType zeroinitializer
}

declare void @exit(i32)

attributes #0 = { noinline nounwind optnone }
