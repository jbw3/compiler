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

define i32 @nestedCall(i32 %x, i32 %y, i32 %z) {
entry:
  %call = call i32 @twoArgs(i32 0, i32 %x)
  %add = add i32 %y, 42
  %sub = sub i32 %z, 57
  %call1 = call i32 @twoArgs(i32 %add, i32 %sub)
  %add2 = add i32 %call, %call1
  ret i32 %add2
}

define i1 @call_function_defined_later() {
entry:
  %call = call i1 @returnBool()
  ret i1 %call
}

define i1 @returnBool() {
entry:
  ret i1 true
}
