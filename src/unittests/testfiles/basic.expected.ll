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
