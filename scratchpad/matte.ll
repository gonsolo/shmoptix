; Copied from module dump

; ModuleID = 'Shmoptix'
source_filename = "Shmoptix"

@Ci = external global [3 x float]

define void @matte([3 x float] %Cs) {
entry:
	%0 = alloca [3 x float]
	store [3 x float] %Cs, [3 x float]* %0
	%1 = load [3 x float], [3 x float]* %0
	store [3 x float] %1, [3 x float]* @Ci
	ret void
}

