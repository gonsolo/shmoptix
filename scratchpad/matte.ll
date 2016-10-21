
@Cs = external global float
@Ci = external global float

define void @matte() {
entry:
	%0 = load float, float* @Cs
	store float %0, float* @Ci

	ret void
}

declare void @llvm.debugtrap() nounwind



