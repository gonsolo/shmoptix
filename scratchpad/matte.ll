; Copied from module dump

; ModuleID = 'Shmoptix'
source_filename = "Shmoptix"

;@Ci = external global [3 x float]

;define void @matte([3 x float] %Cs) {
;entry:
;	%0 = alloca [3 x float]
;	store [3 x float] %Cs, [3 x float]* %0
;	%1 = load [3 x float], [3 x float]* %0
;	store [3 x float] %1, [3 x float]* @Ci
;	ret void
;}

@Ci = external global float
;@"\01?Ci@@3MA" = global float 0.000000e+00, align 4

define void @matte(float %Cs) {
entry:
;	%0 = alloca float
;	store float %Cs, float* %0
;	%1 = load float, float* %0
;	store float %1, float* @Ci

	%0 = alloca float, align 4
	store float %Cs, float* %0, align 4
	%1 = load float, float* %0, align 4
	store float %1, float* @Ci, align 4

	;store float 1.800000e+01, float* @Ci, align 4

	ret void
}


