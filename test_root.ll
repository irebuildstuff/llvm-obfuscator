; ModuleID = 'examples\test_new_features.ll'
source_filename = "examples\\simple_example.c"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc19.44.35220"

$sprintf = comdat any

$vsprintf = comdat any

$_snprintf = comdat any

$_vsnprintf = comdat any

$printf = comdat any

$_vsprintf_l = comdat any

$_vsnprintf_l = comdat any

$__local_stdio_printf_options = comdat any

$_vfprintf_l = comdat any

@"??_C@_0BM@FLBJKOEJ@Protected?5Application?5v1?40?6?$AA@" = internal unnamed_addr global [28 x i8] c"\D3Q\EEU\E2D\F1@\EF\0B\C8Y\FFC\E4N\F2G\F8^\F9\17\E3\04\B5\0B\939", align 1
@"??_C@_0P@CNIMNJLH@SECRET?9KEY?9123?$AA@" = internal unnamed_addr global [15 x i8] c"A\0DS\18S\189\05_\195s,w\1C", align 1
@"??_C@_0BG@BOEPDAFA@Invalid?5license?5key?$CB?6?$AA@" = internal unnamed_addr global [22 x i8] c"\1D\80\AF\F1<\83\B9\B40\8F\B2\FD6\91\B0\BC/\9B\B0\A1J\FA", align 1
@"??_C@_0CB@NDEEDFKP@License?5validated?5successfully?4?6@" = internal unnamed_addr global [33 x i8] c".,\03\22\082\01c\1C,\04&\0A(\18.\16u\03\22\152\11 \09;\0D3\12 RQB", align 1
@__const.main.data = private unnamed_addr constant [5 x i32] [i32 10, i32 20, i32 30, i32 40, i32 50], align 16
@"??_C@_0BG@PLBFEBCH@Processing?3?5?$CFd?5?9?$DO?5?$CFd?6?$AA@" = internal unnamed_addr global [22 x i8] c"\8BH\B6[\BAM\AEU\BDU\EB\10\F2R\F5\19\F5\0A\ECL\C5.", align 1
@"??_C@_0BG@INDCFGHJ@Processing?5complete?4?6?$AA@" = internal unnamed_addr global [22 x i8] c"\08\1D`{9\18xu>\00's;\0Esx-\0Bz&F{", align 1
@__local_stdio_printf_options._OptionsStorage = internal global i64 0, align 8
@__obf_key_6 = internal constant [2 x i8] c"\83\22"
@__obf_key_6.1 = internal constant [2 x i8] c"\12I"
@__obf_key_6.2 = internal constant [4 x i8] c"T\EF\DB\93"
@__obf_key_6.3 = internal constant [2 x i8] c"bD"
@__obf_key_6.4 = internal constant [2 x i8] c"\DB;"
@__obf_key_6.5 = internal constant [4 x i8] c"Xn\0D\1B"
@llvm.global_ctors = appending global [1 x { i32, ptr, ptr }] [{ i32, ptr, ptr } { i32 65535, ptr @__obf_decrypt_ctor, ptr null }]

; Function Attrs: noinline nounwind optnone uwtable
define linkonce_odr dso_local i32 @sprintf(ptr noundef %0, ptr noundef %1, ...) #0 comdat {
  %3 = alloca ptr, align 8
  %4 = alloca ptr, align 8
  %5 = alloca i32, align 4
  %6 = alloca ptr, align 8
  store ptr %1, ptr %3, align 8
  store ptr %0, ptr %4, align 8
  call void @llvm.va_start.p0(ptr %6)
  %7 = load ptr, ptr %6, align 8
  %8 = load ptr, ptr %3, align 8
  %opaque_var = alloca i32, align 4
  store i32 14, ptr %opaque_var, align 4
  %x = load i32, ptr %opaque_var, align 4
  %9 = xor i32 %x, %x
  %10 = icmp eq i32 %9, 0
  br i1 %10, label %obf_cont_0, label %obf_dead_0

obf_dead_0:                                       ; preds = %2
  %11 = alloca i32, align 4
  %12 = alloca i32, align 4
  %13 = load i32, ptr %11, align 4
  %14 = load i32, ptr %12, align 4
  %15 = add i32 %13, %14
  store i32 %15, ptr %11, align 4
  br label %obf_cont_0

obf_cont_0:                                       ; preds = %2, %obf_dead_0
  %16 = load ptr, ptr %4, align 8
  %17 = call i32 @_vsprintf_l(ptr noundef %16, ptr noundef %8, ptr noundef null, ptr noundef %7)
  store i32 %17, ptr %5, align 4
  call void @llvm.va_end.p0(ptr %6)
  %18 = load i32, ptr %5, align 4
  ret i32 %18
}

; Function Attrs: noinline nounwind optnone uwtable
define linkonce_odr dso_local i32 @vsprintf(ptr noundef %0, ptr noundef %1, ptr noundef %2) #0 comdat {
  %4 = alloca ptr, align 8
  %5 = alloca ptr, align 8
  %6 = alloca ptr, align 8
  store ptr %2, ptr %4, align 8
  store ptr %1, ptr %5, align 8
  store ptr %0, ptr %6, align 8
  %7 = load ptr, ptr %4, align 8
  %opaque_var = alloca i32, align 4
  store i32 72, ptr %opaque_var, align 4
  %x = load i32, ptr %opaque_var, align 4
  %8 = xor i32 %x, -1
  %9 = xor i32 %8, -1
  %10 = icmp eq i32 %9, %x
  br i1 %10, label %obf_cont_0, label %obf_dead_0

obf_dead_0:                                       ; preds = %3
  %11 = alloca i32, align 4
  %12 = alloca i32, align 4
  %13 = load i32, ptr %11, align 4
  %14 = load i32, ptr %12, align 4
  %15 = add i32 %13, %14
  store i32 %15, ptr %11, align 4
  br label %obf_cont_0

obf_cont_0:                                       ; preds = %3, %obf_dead_0
  %16 = load ptr, ptr %5, align 8
  %17 = load ptr, ptr %6, align 8
  %18 = call i32 @_vsnprintf_l(ptr noundef %17, i64 noundef -1, ptr noundef %16, ptr noundef null, ptr noundef %7)
  ret i32 %18
}

; Function Attrs: noinline nounwind optnone uwtable
define linkonce_odr dso_local i32 @_snprintf(ptr noundef %0, i64 noundef %1, ptr noundef %2, ...) #0 comdat {
  %4 = alloca ptr, align 8
  %5 = alloca i64, align 8
  %6 = alloca ptr, align 8
  %7 = alloca i32, align 4
  %8 = alloca ptr, align 8
  store ptr %2, ptr %4, align 8
  store i64 %1, ptr %5, align 8
  store ptr %0, ptr %6, align 8
  call void @llvm.va_start.p0(ptr %8)
  %9 = load ptr, ptr %8, align 8
  %10 = load ptr, ptr %4, align 8
  %opaque_var = alloca i32, align 4
  store i32 18, ptr %opaque_var, align 4
  %x = load i32, ptr %opaque_var, align 4
  %11 = mul i32 %x, %x
  %12 = icmp sge i32 %11, 0
  br i1 %12, label %obf_cont_0, label %obf_dead_0

obf_dead_0:                                       ; preds = %3
  %13 = alloca i32, align 4
  %14 = alloca i32, align 4
  %15 = load i32, ptr %13, align 4
  %16 = load i32, ptr %14, align 4
  %17 = add i32 %15, %16
  store i32 %17, ptr %13, align 4
  br label %obf_cont_0

obf_cont_0:                                       ; preds = %3, %obf_dead_0
  %18 = load i64, ptr %5, align 8
  %19 = load ptr, ptr %6, align 8
  %20 = call i32 @_vsnprintf(ptr noundef %19, i64 noundef %18, ptr noundef %10, ptr noundef %9)
  store i32 %20, ptr %7, align 4
  call void @llvm.va_end.p0(ptr %8)
  %21 = load i32, ptr %7, align 4
  ret i32 %21
}

; Function Attrs: noinline nounwind optnone uwtable
define linkonce_odr dso_local i32 @_vsnprintf(ptr noundef %0, i64 noundef %1, ptr noundef %2, ptr noundef %3) #0 comdat {
  %5 = alloca ptr, align 8
  %6 = alloca ptr, align 8
  %7 = alloca i64, align 8
  %8 = alloca ptr, align 8
  store ptr %3, ptr %5, align 8
  store ptr %2, ptr %6, align 8
  store i64 %1, ptr %7, align 8
  store ptr %0, ptr %8, align 8
  %9 = load ptr, ptr %5, align 8
  %opaque_var = alloca i32, align 4
  store i32 91, ptr %opaque_var, align 4
  %x = load i32, ptr %opaque_var, align 4
  %10 = shl i32 %x, 1
  %11 = lshr i32 %10, 1
  %12 = icmp eq i32 %11, %x
  br i1 %12, label %obf_cont_0, label %obf_dead_0

obf_dead_0:                                       ; preds = %4
  %13 = alloca i32, align 4
  %14 = alloca i32, align 4
  %15 = load i32, ptr %13, align 4
  %16 = load i32, ptr %14, align 4
  %17 = add i32 %15, %16
  store i32 %17, ptr %13, align 4
  br label %obf_cont_0

obf_cont_0:                                       ; preds = %4, %obf_dead_0
  %18 = load ptr, ptr %6, align 8
  %19 = load i64, ptr %7, align 8
  %20 = load ptr, ptr %8, align 8
  %21 = call i32 @_vsnprintf_l(ptr noundef %20, i64 noundef %19, ptr noundef %18, ptr noundef null, ptr noundef %9)
  ret i32 %21
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @secret_algorithm(i32 noundef %0) #0 {
  %2 = alloca i1, align 1
  %opaque_var7 = alloca i32, align 4
  store i32 28, ptr %opaque_var7, align 4
  %x8 = load i32, ptr %opaque_var7, align 4
  %3 = xor i32 %x8, -1
  %4 = and i32 %x8, %3
  %5 = icmp eq i32 %4, 0
  store i1 %5, ptr %2, align 1
  %6 = alloca i32, align 4
  %7 = alloca i32, align 4
  %8 = alloca i32, align 4
  %9 = alloca i32, align 4
  store i32 %0, ptr %6, align 4
  store i32 4919, ptr %7, align 4
  %10 = load i32, ptr %6, align 4
  %opaque_var = alloca i32, align 4
  store i32 93, ptr %opaque_var, align 4
  %x = load i32, ptr %opaque_var, align 4
  %11 = or i32 %x, %x
  %12 = icmp eq i32 %11, %x
  br i1 %12, label %obf_cont_0, label %obf_dead_0

obf_dead_0:                                       ; preds = %1
  %13 = alloca i1, align 1
  %opaque_var9 = alloca i32, align 4
  store i32 62, ptr %opaque_var9, align 4
  %x10 = load i32, ptr %opaque_var9, align 4
  %14 = xor i32 %x10, %x10
  %15 = icmp eq i32 %14, 0
  store i1 %15, ptr %13, align 1
  %16 = alloca i32, align 4
  %17 = alloca i32, align 4
  %18 = load i32, ptr %16, align 4
  %19 = load i32, ptr %17, align 4
  %20 = add i32 %18, %19
  store i32 %20, ptr %16, align 4
  br label %fake_loop_3

fake_loop_3:                                      ; preds = %obf_dead_0, %fake_loop_3
  %21 = alloca i32, align 4
  store i32 0, ptr %21, align 4
  %22 = load i32, ptr %21, align 4
  %23 = icmp slt i32 %22, 0
  br i1 %23, label %fake_loop_3, label %fake_exit_3

fake_exit_3:                                      ; preds = %fake_loop_3
  br label %obf_cont_0

obf_cont_0:                                       ; preds = %fake_exit_3, %1
  store i32 %10, ptr %8, align 4
  store i32 0, ptr %9, align 4
  br label %24

24:                                               ; preds = %obf_cont_3, %obf_cont_0
  %25 = alloca i1, align 1
  %opaque_var11 = alloca i32, align 4
  store i32 58, ptr %opaque_var11, align 4
  %x12 = load i32, ptr %opaque_var11, align 4
  %26 = xor i32 %x12, %x12
  %27 = icmp eq i32 %26, 0
  store i1 %27, ptr %25, align 1
  %28 = load i32, ptr %9, align 4
  %29 = icmp slt i32 %28, 5
  %opaque_var1 = alloca i32, align 4
  store i32 98, ptr %opaque_var1, align 4
  %x2 = load i32, ptr %opaque_var1, align 4
  %30 = shl i32 %x2, 1
  %31 = lshr i32 %30, 1
  %32 = icmp eq i32 %31, %x2
  %33 = and i1 %29, %32
  br i1 %33, label %fake_loop_1, label %97

fake_loop_1:                                      ; preds = %24, %fake_loop_1
  %34 = alloca i32, align 4
  store i32 0, ptr %34, align 4
  %35 = load i32, ptr %34, align 4
  %36 = icmp sge i32 %35, 2147483647
  br i1 %36, label %fake_loop_1, label %fake_exit_1

fake_exit_1:                                      ; preds = %fake_loop_1
  br label %37

37:                                               ; preds = %fake_exit_1
  %38 = alloca i1, align 1
  %opaque_var13 = alloca i32, align 4
  store i32 8, ptr %opaque_var13, align 4
  %x14 = load i32, ptr %opaque_var13, align 4
  %39 = mul i32 %x14, %x14
  %40 = mul i32 %39, 7
  %41 = add i32 %40, 11
  %42 = icmp ne i32 %41, 0
  store i1 %42, ptr %38, align 1
  %43 = load i32, ptr %8, align 4
  %44 = mul nsw i32 %43, 3
  %45 = load i32, ptr %7, align 4
  %46 = add nsw i32 %44, %45
  %47 = load i32, ptr %7, align 4
  %48 = load i32, ptr %9, align 4
  %49 = ashr i32 %47, %48
  %opaque_var3 = alloca i32, align 4
  store i32 76, ptr %opaque_var3, align 4
  %x4 = load i32, ptr %opaque_var3, align 4
  %50 = xor i32 %x4, %x4
  %51 = icmp eq i32 %50, 0
  br i1 %51, label %fake_loop_4, label %obf_dead_2

obf_dead_2:                                       ; preds = %37
  %52 = alloca i1, align 1
  %opaque_var15 = alloca i32, align 4
  store i32 60, ptr %opaque_var15, align 4
  %x16 = load i32, ptr %opaque_var15, align 4
  %53 = xor i32 %x16, -1
  %54 = xor i32 %53, -1
  %55 = icmp eq i32 %54, %x16
  store i1 %55, ptr %52, align 1
  %56 = alloca i32, align 4
  %57 = alloca i32, align 4
  %58 = load i32, ptr %56, align 4
  %59 = load i32, ptr %57, align 4
  %60 = add i32 %58, %59
  store i32 %60, ptr %56, align 4
  br label %obf_cont_2

fake_loop_4:                                      ; preds = %37, %fake_loop_4
  %61 = alloca i32, align 4
  store i32 0, ptr %61, align 4
  %62 = load i32, ptr %61, align 4
  %63 = icmp sge i32 %62, 2147483647
  br i1 %63, label %fake_loop_4, label %fake_exit_4

fake_exit_4:                                      ; preds = %fake_loop_4
  br label %obf_cont_2

obf_cont_2:                                       ; preds = %fake_exit_4, %obf_dead_2
  %64 = alloca i1, align 1
  %opaque_var17 = alloca i32, align 4
  store i32 67, ptr %opaque_var17, align 4
  %x18 = load i32, ptr %opaque_var17, align 4
  %65 = mul i32 %x18, %x18
  %66 = add i32 %65, %x18
  %67 = urem i32 %66, 2
  %68 = icmp eq i32 %67, 0
  store i1 %68, ptr %64, align 1
  %69 = xor i32 %46, %49
  store i32 %69, ptr %8, align 4
  %70 = load i32, ptr %7, align 4
  %71 = shl i32 %70, 1
  %72 = or i32 %71, 1
  store i32 %72, ptr %7, align 4
  br label %fake_loop_2

fake_loop_2:                                      ; preds = %obf_cont_2, %fake_loop_2
  %opaque_var23 = alloca i32, align 4
  store i32 84, ptr %opaque_var23, align 4
  %x24 = load i32, ptr %opaque_var23, align 4
  %73 = or i32 %x24, %x24
  %74 = icmp eq i32 %73, %x24
  %75 = xor i1 %74, true
  br i1 %75, label %fake_loop_2, label %fake_exit_2

fake_exit_2:                                      ; preds = %fake_loop_2
  br label %76

76:                                               ; preds = %fake_exit_2
  %77 = alloca i1, align 1
  %opaque_var19 = alloca i32, align 4
  store i32 36, ptr %opaque_var19, align 4
  %x20 = load i32, ptr %opaque_var19, align 4
  %78 = and i32 %x20, %x20
  %79 = icmp eq i32 %78, %x20
  store i1 %79, ptr %77, align 1
  %80 = load i32, ptr %9, align 4
  %81 = add nsw i32 %80, 1
  %opaque_var5 = alloca i32, align 4
  store i32 43, ptr %opaque_var5, align 4
  %x6 = load i32, ptr %opaque_var5, align 4
  %82 = mul i32 %x6, %x6
  %83 = icmp sge i32 %82, 0
  br i1 %83, label %fake_loop_0, label %obf_dead_3

obf_dead_3:                                       ; preds = %76
  %84 = alloca i1, align 1
  %opaque_var21 = alloca i32, align 4
  store i32 2, ptr %opaque_var21, align 4
  %x22 = load i32, ptr %opaque_var21, align 4
  %85 = mul i32 %x22, %x22
  %86 = mul i32 %85, 7
  %87 = add i32 %86, 11
  %88 = icmp ne i32 %87, 0
  store i1 %88, ptr %84, align 1
  %89 = alloca i32, align 4
  %90 = alloca i32, align 4
  %91 = load i32, ptr %89, align 4
  %92 = load i32, ptr %90, align 4
  %93 = add i32 %91, %92
  store i32 %93, ptr %89, align 4
  br label %obf_cont_3

fake_loop_0:                                      ; preds = %76, %fake_loop_0
  %94 = alloca i32, align 4
  store i32 0, ptr %94, align 4
  %95 = load i32, ptr %94, align 4
  %96 = icmp slt i32 %95, 0
  br i1 %96, label %fake_loop_0, label %fake_exit_0

fake_exit_0:                                      ; preds = %fake_loop_0
  br label %obf_cont_3

obf_cont_3:                                       ; preds = %fake_exit_0, %obf_dead_3
  store i32 %81, ptr %9, align 4
  br label %24, !llvm.loop !8

97:                                               ; preds = %24
  %98 = load i32, ptr %8, align 4
  ret i32 %98
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @validate_license(ptr noundef %0) #0 {
  %2 = alloca i1, align 1
  %opaque_var11 = alloca i32, align 4
  store i32 45, ptr %opaque_var11, align 4
  %x12 = load i32, ptr %opaque_var11, align 4
  %3 = and i32 %x12, %x12
  %4 = icmp eq i32 %3, %x12
  store i1 %4, ptr %2, align 1
  %5 = alloca i1, align 1
  %opaque_var13 = alloca i32, align 4
  store i32 1, ptr %opaque_var13, align 4
  %x14 = load i32, ptr %opaque_var13, align 4
  %6 = mul i32 %x14, %x14
  %7 = add i32 %6, %x14
  %8 = urem i32 %7, 2
  %9 = icmp eq i32 %8, 0
  store i1 %9, ptr %5, align 1
  %10 = alloca i32, align 4
  %11 = alloca ptr, align 8
  %12 = alloca i32, align 4
  %13 = alloca i32, align 4
  store ptr %0, ptr %11, align 8
  %14 = load ptr, ptr %11, align 8
  %15 = icmp eq ptr %14, null
  %opaque_var = alloca i32, align 4
  store i32 71, ptr %opaque_var, align 4
  %x = load i32, ptr %opaque_var, align 4
  %16 = xor i32 %x, -1
  %17 = and i32 %x, %16
  %18 = icmp eq i32 %17, 0
  %19 = and i1 %15, %18
  br i1 %19, label %20, label %21

20:                                               ; preds = %1
  store i32 0, ptr %10, align 4
  br label %144

21:                                               ; preds = %1
  %22 = alloca i1, align 1
  %opaque_var15 = alloca i32, align 4
  store i32 20, ptr %opaque_var15, align 4
  %x16 = load i32, ptr %opaque_var15, align 4
  %23 = sub i32 %x16, %x16
  %24 = add i32 %23, 1
  %25 = icmp sgt i32 %24, 0
  store i1 %25, ptr %22, align 1
  store i32 0, ptr %12, align 4
  %opaque_var1 = alloca i32, align 4
  store i32 38, ptr %opaque_var1, align 4
  %x2 = load i32, ptr %opaque_var1, align 4
  %26 = or i32 %x2, 1
  %27 = icmp ne i32 %26, 0
  br i1 %27, label %fake_loop_2, label %obf_dead_1

obf_dead_1:                                       ; preds = %21
  %28 = alloca i1, align 1
  %opaque_var17 = alloca i32, align 4
  store i32 7, ptr %opaque_var17, align 4
  %x18 = load i32, ptr %opaque_var17, align 4
  %29 = mul i32 %x18, %x18
  %30 = add i32 %29, %x18
  %31 = urem i32 %30, 2
  %32 = icmp eq i32 %31, 0
  store i1 %32, ptr %28, align 1
  %33 = alloca i32, align 4
  %34 = alloca i32, align 4
  %35 = load i32, ptr %33, align 4
  %36 = load i32, ptr %34, align 4
  %37 = add i32 %35, %36
  store i32 %37, ptr %33, align 4
  br label %obf_cont_1

fake_loop_2:                                      ; preds = %21, %fake_loop_2
  %opaque_var37 = alloca i32, align 4
  store i32 98, ptr %opaque_var37, align 4
  %x38 = load i32, ptr %opaque_var37, align 4
  %38 = and i32 %x38, %x38
  %39 = icmp eq i32 %38, %x38
  %40 = xor i1 %39, true
  br i1 %40, label %fake_loop_2, label %fake_exit_2

fake_exit_2:                                      ; preds = %fake_loop_2
  br label %obf_cont_1

obf_cont_1:                                       ; preds = %fake_exit_2, %obf_dead_1
  store i32 0, ptr %13, align 4
  br label %41

41:                                               ; preds = %obf_cont_4, %obf_cont_1
  %42 = alloca i1, align 1
  %opaque_var19 = alloca i32, align 4
  store i32 65, ptr %opaque_var19, align 4
  %x20 = load i32, ptr %opaque_var19, align 4
  %43 = mul i32 %x20, %x20
  %44 = add i32 %43, %x20
  %45 = urem i32 %44, 2
  %46 = icmp eq i32 %45, 0
  store i1 %46, ptr %42, align 1
  %47 = alloca i1, align 1
  %opaque_var21 = alloca i32, align 4
  store i32 20, ptr %opaque_var21, align 4
  %x22 = load i32, ptr %opaque_var21, align 4
  %48 = xor i32 %x22, %x22
  %49 = icmp eq i32 %48, 0
  store i1 %49, ptr %47, align 1
  %50 = load ptr, ptr %11, align 8
  %51 = load i32, ptr %13, align 4
  %52 = sext i32 %51 to i64
  %53 = getelementptr inbounds i8, ptr %50, i64 %52
  %54 = load i8, ptr %53, align 1
  %55 = sext i8 %54 to i32
  %56 = icmp ne i32 %55, 0
  %opaque_var3 = alloca i32, align 4
  store i32 34, ptr %opaque_var3, align 4
  %x4 = load i32, ptr %opaque_var3, align 4
  %57 = xor i32 %x4, -1
  %58 = xor i32 %57, -1
  %59 = icmp eq i32 %58, %x4
  %60 = and i1 %56, %59
  br i1 %60, label %fake_loop_1, label %119

fake_loop_1:                                      ; preds = %41, %fake_loop_1
  %61 = alloca i32, align 4
  store i32 0, ptr %61, align 4
  %62 = load i32, ptr %61, align 4
  %63 = icmp sge i32 %62, 2147483647
  br i1 %63, label %fake_loop_1, label %fake_exit_1

fake_exit_1:                                      ; preds = %fake_loop_1
  br label %64

64:                                               ; preds = %fake_exit_1
  %65 = alloca i1, align 1
  %opaque_var23 = alloca i32, align 4
  store i32 91, ptr %opaque_var23, align 4
  %x24 = load i32, ptr %opaque_var23, align 4
  %66 = mul i32 %x24, %x24
  %67 = mul i32 %66, 7
  %68 = add i32 %67, 11
  %69 = icmp ne i32 %68, 0
  store i1 %69, ptr %65, align 1
  %70 = load ptr, ptr %11, align 8
  %71 = load i32, ptr %13, align 4
  %72 = sext i32 %71 to i64
  %73 = getelementptr inbounds i8, ptr %70, i64 %72
  %74 = load i8, ptr %73, align 1
  %75 = sext i8 %74 to i32
  %opaque_var5 = alloca i32, align 4
  store i32 56, ptr %opaque_var5, align 4
  %x6 = load i32, ptr %opaque_var5, align 4
  %76 = shl i32 %x6, 1
  %77 = lshr i32 %76, 1
  %78 = icmp eq i32 %77, %x6
  br i1 %78, label %fake_loop_3, label %obf_dead_3

obf_dead_3:                                       ; preds = %64
  %79 = alloca i1, align 1
  %opaque_var25 = alloca i32, align 4
  store i32 13, ptr %opaque_var25, align 4
  %x26 = load i32, ptr %opaque_var25, align 4
  %80 = shl i32 %x26, 1
  %81 = lshr i32 %80, 1
  %82 = icmp eq i32 %81, %x26
  store i1 %82, ptr %79, align 1
  %83 = alloca i32, align 4
  %84 = alloca i32, align 4
  %85 = load i32, ptr %83, align 4
  %86 = load i32, ptr %84, align 4
  %87 = add i32 %85, %86
  store i32 %87, ptr %83, align 4
  br label %obf_cont_3

fake_loop_3:                                      ; preds = %64, %fake_loop_3
  %88 = alloca i32, align 4
  store i32 0, ptr %88, align 4
  %89 = load i32, ptr %88, align 4
  %90 = icmp slt i32 %89, 0
  br i1 %90, label %fake_loop_3, label %fake_exit_3

fake_exit_3:                                      ; preds = %fake_loop_3
  br label %obf_cont_3

obf_cont_3:                                       ; preds = %fake_exit_3, %obf_dead_3
  %91 = alloca i1, align 1
  %opaque_var27 = alloca i32, align 4
  store i32 3, ptr %opaque_var27, align 4
  %x28 = load i32, ptr %opaque_var27, align 4
  %92 = xor i32 %x28, %x28
  %93 = icmp eq i32 %92, 0
  store i1 %93, ptr %91, align 1
  %94 = load i32, ptr %13, align 4
  %95 = add nsw i32 %94, 1
  %96 = mul nsw i32 %75, %95
  %97 = load i32, ptr %12, align 4
  %98 = add nsw i32 %97, %96
  store i32 %98, ptr %12, align 4
  br label %99

99:                                               ; preds = %obf_cont_3
  %100 = alloca i1, align 1
  %opaque_var29 = alloca i32, align 4
  store i32 49, ptr %opaque_var29, align 4
  %x30 = load i32, ptr %opaque_var29, align 4
  %101 = shl i32 %x30, 1
  %102 = lshr i32 %101, 1
  %103 = icmp eq i32 %102, %x30
  store i1 %103, ptr %100, align 1
  %104 = load i32, ptr %13, align 4
  %105 = add nsw i32 %104, 1
  %opaque_var7 = alloca i32, align 4
  store i32 75, ptr %opaque_var7, align 4
  %x8 = load i32, ptr %opaque_var7, align 4
  %106 = mul i32 %x8, %x8
  %107 = icmp sge i32 %106, 0
  br i1 %107, label %fake_loop_4, label %obf_dead_4

obf_dead_4:                                       ; preds = %99
  %108 = alloca i1, align 1
  %opaque_var31 = alloca i32, align 4
  store i32 94, ptr %opaque_var31, align 4
  %x32 = load i32, ptr %opaque_var31, align 4
  %109 = and i32 %x32, %x32
  %110 = icmp eq i32 %109, %x32
  store i1 %110, ptr %108, align 1
  %111 = alloca i32, align 4
  %112 = alloca i32, align 4
  %113 = load i32, ptr %111, align 4
  %114 = load i32, ptr %112, align 4
  %115 = add i32 %113, %114
  store i32 %115, ptr %111, align 4
  br label %obf_cont_4

fake_loop_4:                                      ; preds = %99, %fake_loop_4
  %116 = alloca i32, align 4
  store i32 0, ptr %116, align 4
  %117 = load i32, ptr %116, align 4
  %118 = icmp sge i32 %117, 2147483647
  br i1 %118, label %fake_loop_4, label %fake_exit_4

fake_exit_4:                                      ; preds = %fake_loop_4
  br label %obf_cont_4

obf_cont_4:                                       ; preds = %fake_exit_4, %obf_dead_4
  store i32 %105, ptr %13, align 4
  br label %41, !llvm.loop !10

119:                                              ; preds = %41
  %120 = alloca i1, align 1
  %opaque_var33 = alloca i32, align 4
  store i32 93, ptr %opaque_var33, align 4
  %x34 = load i32, ptr %opaque_var33, align 4
  %121 = shl i32 %x34, 1
  %122 = lshr i32 %121, 1
  %123 = icmp eq i32 %122, %x34
  store i1 %123, ptr %120, align 1
  %124 = load i32, ptr %12, align 4
  %125 = srem i32 %124, 1337
  %126 = icmp eq i32 %125, 42
  %opaque_var9 = alloca i32, align 4
  store i32 65, ptr %opaque_var9, align 4
  %x10 = load i32, ptr %opaque_var9, align 4
  %127 = xor i32 %x10, -1
  %128 = and i32 %x10, %127
  %129 = icmp eq i32 %128, 0
  br i1 %129, label %fake_loop_0, label %obf_dead_5

obf_dead_5:                                       ; preds = %119
  %130 = alloca i1, align 1
  %opaque_var35 = alloca i32, align 4
  store i32 40, ptr %opaque_var35, align 4
  %x36 = load i32, ptr %opaque_var35, align 4
  %131 = mul i32 %x36, %x36
  %132 = add i32 %131, %x36
  %133 = urem i32 %132, 2
  %134 = icmp eq i32 %133, 0
  store i1 %134, ptr %130, align 1
  %135 = alloca i32, align 4
  %136 = alloca i32, align 4
  %137 = load i32, ptr %135, align 4
  %138 = load i32, ptr %136, align 4
  %139 = add i32 %137, %138
  store i32 %139, ptr %135, align 4
  br label %obf_cont_5

fake_loop_0:                                      ; preds = %119, %fake_loop_0
  %140 = alloca i32, align 4
  store i32 0, ptr %140, align 4
  %141 = load i32, ptr %140, align 4
  %142 = icmp slt i32 %141, 0
  br i1 %142, label %fake_loop_0, label %fake_exit_0

fake_exit_0:                                      ; preds = %fake_loop_0
  br label %obf_cont_5

obf_cont_5:                                       ; preds = %fake_exit_0, %obf_dead_5
  %143 = zext i1 %126 to i32
  store i32 %143, ptr %10, align 4
  br label %144

144:                                              ; preds = %obf_cont_5, %20
  %145 = load i32, ptr %10, align 4
  ret i32 %145
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main(i32 noundef %0, ptr noundef %1) #0 {
  %3 = alloca i1, align 1
  %opaque_var13 = alloca i32, align 4
  store i32 89, ptr %opaque_var13, align 4
  %x14 = load i32, ptr %opaque_var13, align 4
  %4 = shl i32 %x14, 1
  %5 = lshr i32 %4, 1
  %6 = icmp eq i32 %5, %x14
  store i1 %6, ptr %3, align 1
  %7 = alloca i1, align 1
  %opaque_var15 = alloca i32, align 4
  store i32 87, ptr %opaque_var15, align 4
  %x16 = load i32, ptr %opaque_var15, align 4
  %8 = mul i32 %x16, %x16
  %9 = mul i32 %8, 7
  %10 = add i32 %9, 11
  %11 = icmp ne i32 %10, 0
  store i1 %11, ptr %7, align 1
  %12 = alloca i1, align 1
  %opaque_var17 = alloca i32, align 4
  store i32 13, ptr %opaque_var17, align 4
  %x18 = load i32, ptr %opaque_var17, align 4
  %13 = add i32 %x18, 0
  %14 = icmp eq i32 %13, %x18
  store i1 %14, ptr %12, align 1
  %15 = alloca i32, align 4
  %16 = alloca ptr, align 8
  %17 = alloca i32, align 4
  %18 = alloca ptr, align 8
  %19 = alloca [5 x i32], align 16
  %20 = alloca [5 x i32], align 16
  %21 = alloca i32, align 4
  store i32 0, ptr %15, align 4
  store ptr %1, ptr %16, align 8
  store i32 %0, ptr %17, align 4
  %22 = call i32 (ptr, ...) @printf(ptr noundef @"??_C@_0BM@FLBJKOEJ@Protected?5Application?5v1?40?6?$AA@")
  store ptr @"??_C@_0P@CNIMNJLH@SECRET?9KEY?9123?$AA@", ptr %18, align 8
  %23 = load ptr, ptr %18, align 8
  %24 = call i32 @validate_license(ptr noundef %23)
  %25 = icmp ne i32 %24, 0
  %opaque_var = alloca i32, align 4
  store i32 54, ptr %opaque_var, align 4
  %x = load i32, ptr %opaque_var, align 4
  %26 = mul i32 %x, %x
  %27 = add i32 %26, %x
  %28 = urem i32 %27, 2
  %29 = icmp eq i32 %28, 0
  %30 = and i1 %25, %29
  br i1 %30, label %51, label %31

31:                                               ; preds = %2
  %32 = alloca i1, align 1
  %opaque_var19 = alloca i32, align 4
  store i32 88, ptr %opaque_var19, align 4
  %x20 = load i32, ptr %opaque_var19, align 4
  %33 = mul i32 %x20, %x20
  %34 = icmp sge i32 %33, 0
  store i1 %34, ptr %32, align 1
  %35 = call i32 (ptr, ...) @printf(ptr noundef @"??_C@_0BG@BOEPDAFA@Invalid?5license?5key?$CB?6?$AA@")
  %opaque_var1 = alloca i32, align 4
  store i32 56, ptr %opaque_var1, align 4
  %x2 = load i32, ptr %opaque_var1, align 4
  %36 = xor i32 %x2, -1
  %37 = and i32 %x2, %36
  %38 = icmp eq i32 %37, 0
  br i1 %38, label %obf_cont_1, label %obf_dead_1

obf_dead_1:                                       ; preds = %31
  %39 = alloca i1, align 1
  %opaque_var21 = alloca i32, align 4
  store i32 66, ptr %opaque_var21, align 4
  %x22 = load i32, ptr %opaque_var21, align 4
  %40 = sub i32 %x22, %x22
  %41 = add i32 %40, 1
  %42 = icmp sgt i32 %41, 0
  store i1 %42, ptr %39, align 1
  %43 = alloca i32, align 4
  %44 = alloca i32, align 4
  %45 = load i32, ptr %43, align 4
  %46 = load i32, ptr %44, align 4
  %47 = add i32 %45, %46
  store i32 %47, ptr %43, align 4
  br label %fake_loop_3

fake_loop_3:                                      ; preds = %obf_dead_1, %fake_loop_3
  %48 = alloca i32, align 4
  store i32 0, ptr %48, align 4
  %49 = load i32, ptr %48, align 4
  %50 = icmp slt i32 %49, 0
  br i1 %50, label %fake_loop_3, label %fake_exit_3

fake_exit_3:                                      ; preds = %fake_loop_3
  br label %obf_cont_1

obf_cont_1:                                       ; preds = %fake_exit_3, %31
  store i32 1, ptr %15, align 4
  br label %164

51:                                               ; preds = %2
  %52 = alloca i1, align 1
  %opaque_var23 = alloca i32, align 4
  store i32 59, ptr %opaque_var23, align 4
  %x24 = load i32, ptr %opaque_var23, align 4
  %53 = and i32 %x24, %x24
  %54 = icmp eq i32 %53, %x24
  store i1 %54, ptr %52, align 1
  %55 = call i32 (ptr, ...) @printf(ptr noundef @"??_C@_0CB@NDEEDFKP@License?5validated?5successfully?4?6@")
  call void @llvm.memcpy.p0.p0.i64(ptr align 16 %19, ptr align 16 @__const.main.data, i64 20, i1 false)
  %opaque_var3 = alloca i32, align 4
  store i32 54, ptr %opaque_var3, align 4
  %x4 = load i32, ptr %opaque_var3, align 4
  %56 = add i32 %x4, 0
  %57 = icmp eq i32 %56, %x4
  br i1 %57, label %obf_cont_2, label %obf_dead_2

obf_dead_2:                                       ; preds = %51
  %58 = alloca i1, align 1
  %opaque_var25 = alloca i32, align 4
  store i32 58, ptr %opaque_var25, align 4
  %x26 = load i32, ptr %opaque_var25, align 4
  %59 = or i32 %x26, %x26
  %60 = icmp eq i32 %59, %x26
  store i1 %60, ptr %58, align 1
  %61 = alloca i32, align 4
  %62 = alloca i32, align 4
  %63 = load i32, ptr %61, align 4
  %64 = load i32, ptr %62, align 4
  %65 = add i32 %63, %64
  store i32 %65, ptr %61, align 4
  br label %obf_cont_2

obf_cont_2:                                       ; preds = %51, %obf_dead_2
  store i32 0, ptr %21, align 4
  br label %66

66:                                               ; preds = %obf_cont_5, %obf_cont_2
  %67 = alloca i1, align 1
  %opaque_var27 = alloca i32, align 4
  store i32 50, ptr %opaque_var27, align 4
  %x28 = load i32, ptr %opaque_var27, align 4
  %68 = and i32 %x28, %x28
  %69 = icmp eq i32 %68, %x28
  store i1 %69, ptr %67, align 1
  %70 = load i32, ptr %21, align 4
  %71 = icmp slt i32 %70, 5
  %opaque_var5 = alloca i32, align 4
  store i32 100, ptr %opaque_var5, align 4
  %x6 = load i32, ptr %opaque_var5, align 4
  %72 = xor i32 %x6, -1
  %73 = xor i32 %72, -1
  %74 = icmp eq i32 %73, %x6
  %75 = and i1 %71, %74
  br i1 %75, label %76, label %147

76:                                               ; preds = %66
  %77 = alloca i1, align 1
  %opaque_var29 = alloca i32, align 4
  store i32 32, ptr %opaque_var29, align 4
  %x30 = load i32, ptr %opaque_var29, align 4
  %78 = shl i32 %x30, 1
  %79 = lshr i32 %78, 1
  %80 = icmp eq i32 %79, %x30
  store i1 %80, ptr %77, align 1
  %81 = alloca i1, align 1
  %opaque_var31 = alloca i32, align 4
  store i32 15, ptr %opaque_var31, align 4
  %x32 = load i32, ptr %opaque_var31, align 4
  %82 = or i32 %x32, 1
  %83 = icmp ne i32 %82, 0
  store i1 %83, ptr %81, align 1
  %84 = load i32, ptr %21, align 4
  %85 = sext i32 %84 to i64
  %86 = getelementptr inbounds [5 x i32], ptr %19, i64 0, i64 %85
  %87 = load i32, ptr %86, align 4
  %88 = call i32 @secret_algorithm(i32 noundef %87)
  %89 = load i32, ptr %21, align 4
  %90 = sext i32 %89 to i64
  %91 = getelementptr inbounds [5 x i32], ptr %20, i64 0, i64 %90
  store i32 %88, ptr %91, align 4
  %opaque_var7 = alloca i32, align 4
  store i32 20, ptr %opaque_var7, align 4
  %x8 = load i32, ptr %opaque_var7, align 4
  %92 = or i32 %x8, 1
  %93 = icmp ne i32 %92, 0
  br i1 %93, label %obf_cont_4, label %obf_dead_4

obf_dead_4:                                       ; preds = %76
  %94 = alloca i1, align 1
  %opaque_var33 = alloca i32, align 4
  store i32 10, ptr %opaque_var33, align 4
  %x34 = load i32, ptr %opaque_var33, align 4
  %95 = mul i32 %x34, %x34
  %96 = mul i32 %95, 7
  %97 = add i32 %96, 11
  %98 = icmp ne i32 %97, 0
  store i1 %98, ptr %94, align 1
  %99 = alloca i32, align 4
  %100 = alloca i32, align 4
  %101 = load i32, ptr %99, align 4
  %102 = load i32, ptr %100, align 4
  %103 = add i32 %101, %102
  store i32 %103, ptr %99, align 4
  br label %fake_loop_1

fake_loop_1:                                      ; preds = %obf_dead_4, %fake_loop_1
  %104 = alloca i32, align 4
  store i32 0, ptr %104, align 4
  %105 = load i32, ptr %104, align 4
  %106 = icmp sge i32 %105, 2147483647
  br i1 %106, label %fake_loop_1, label %fake_exit_1

fake_exit_1:                                      ; preds = %fake_loop_1
  br label %obf_cont_4

obf_cont_4:                                       ; preds = %fake_exit_1, %76
  %107 = alloca i1, align 1
  %opaque_var35 = alloca i32, align 4
  store i32 50, ptr %opaque_var35, align 4
  %x36 = load i32, ptr %opaque_var35, align 4
  %108 = or i32 %x36, 1
  %109 = icmp ne i32 %108, 0
  store i1 %109, ptr %107, align 1
  %110 = load i32, ptr %21, align 4
  %111 = sext i32 %110 to i64
  %112 = getelementptr inbounds [5 x i32], ptr %20, i64 0, i64 %111
  %113 = load i32, ptr %112, align 4
  %114 = load i32, ptr %21, align 4
  %115 = sext i32 %114 to i64
  %116 = getelementptr inbounds [5 x i32], ptr %19, i64 0, i64 %115
  %117 = load i32, ptr %116, align 4
  %118 = call i32 (ptr, ...) @printf(ptr noundef @"??_C@_0BG@PLBFEBCH@Processing?3?5?$CFd?5?9?$DO?5?$CFd?6?$AA@", i32 noundef %117, i32 noundef %113)
  br label %fake_loop_4

fake_loop_4:                                      ; preds = %obf_cont_4, %fake_loop_4
  %119 = alloca i32, align 4
  store i32 0, ptr %119, align 4
  %120 = load i32, ptr %119, align 4
  %121 = icmp sge i32 %120, 2147483647
  br i1 %121, label %fake_loop_4, label %fake_exit_4

fake_exit_4:                                      ; preds = %fake_loop_4
  br label %122

122:                                              ; preds = %fake_exit_4
  %123 = alloca i1, align 1
  %opaque_var37 = alloca i32, align 4
  store i32 79, ptr %opaque_var37, align 4
  %x38 = load i32, ptr %opaque_var37, align 4
  %124 = mul i32 %x38, %x38
  %125 = add i32 %124, %x38
  %126 = urem i32 %125, 2
  %127 = icmp eq i32 %126, 0
  store i1 %127, ptr %123, align 1
  %128 = load i32, ptr %21, align 4
  %129 = add nsw i32 %128, 1
  %opaque_var9 = alloca i32, align 4
  store i32 61, ptr %opaque_var9, align 4
  %x10 = load i32, ptr %opaque_var9, align 4
  %130 = and i32 %x10, %x10
  %131 = icmp eq i32 %130, %x10
  br i1 %131, label %fake_loop_2, label %obf_dead_5

obf_dead_5:                                       ; preds = %122
  %132 = alloca i1, align 1
  %opaque_var39 = alloca i32, align 4
  store i32 3, ptr %opaque_var39, align 4
  %x40 = load i32, ptr %opaque_var39, align 4
  %133 = xor i32 %x40, -1
  %134 = and i32 %x40, %133
  %135 = icmp eq i32 %134, 0
  store i1 %135, ptr %132, align 1
  %136 = alloca i32, align 4
  %137 = alloca i32, align 4
  %138 = load i32, ptr %136, align 4
  %139 = load i32, ptr %137, align 4
  %140 = add i32 %138, %139
  store i32 %140, ptr %136, align 4
  br label %fake_loop_0

fake_loop_0:                                      ; preds = %obf_dead_5, %fake_loop_0
  %141 = alloca i32, align 4
  store i32 0, ptr %141, align 4
  %142 = load i32, ptr %141, align 4
  %143 = icmp slt i32 %142, 0
  br i1 %143, label %fake_loop_0, label %fake_exit_0

fake_exit_0:                                      ; preds = %fake_loop_0
  br label %obf_cont_5

fake_loop_2:                                      ; preds = %122, %fake_loop_2
  %opaque_var45 = alloca i32, align 4
  store i32 68, ptr %opaque_var45, align 4
  %x46 = load i32, ptr %opaque_var45, align 4
  %144 = or i32 %x46, 1
  %145 = icmp ne i32 %144, 0
  %146 = xor i1 %145, true
  br i1 %146, label %fake_loop_2, label %fake_exit_2

fake_exit_2:                                      ; preds = %fake_loop_2
  br label %obf_cont_5

obf_cont_5:                                       ; preds = %fake_exit_2, %fake_exit_0
  store i32 %129, ptr %21, align 4
  br label %66, !llvm.loop !11

147:                                              ; preds = %66
  %148 = alloca i1, align 1
  %opaque_var41 = alloca i32, align 4
  store i32 99, ptr %opaque_var41, align 4
  %x42 = load i32, ptr %opaque_var41, align 4
  %149 = or i32 %x42, %x42
  %150 = icmp eq i32 %149, %x42
  store i1 %150, ptr %148, align 1
  %151 = call i32 (ptr, ...) @printf(ptr noundef @"??_C@_0BG@INDCFGHJ@Processing?5complete?4?6?$AA@")
  %opaque_var11 = alloca i32, align 4
  store i32 23, ptr %opaque_var11, align 4
  %x12 = load i32, ptr %opaque_var11, align 4
  %152 = or i32 %x12, 1
  %153 = icmp ne i32 %152, 0
  br i1 %153, label %obf_cont_6, label %obf_dead_6

obf_dead_6:                                       ; preds = %147
  %154 = alloca i1, align 1
  %opaque_var43 = alloca i32, align 4
  store i32 72, ptr %opaque_var43, align 4
  %x44 = load i32, ptr %opaque_var43, align 4
  %155 = mul i32 %x44, %x44
  %156 = mul i32 %155, 7
  %157 = add i32 %156, 11
  %158 = icmp ne i32 %157, 0
  store i1 %158, ptr %154, align 1
  %159 = alloca i32, align 4
  %160 = alloca i32, align 4
  %161 = load i32, ptr %159, align 4
  %162 = load i32, ptr %160, align 4
  %163 = add i32 %161, %162
  store i32 %163, ptr %159, align 4
  br label %obf_cont_6

obf_cont_6:                                       ; preds = %147, %obf_dead_6
  store i32 0, ptr %15, align 4
  br label %164

164:                                              ; preds = %obf_cont_6, %obf_cont_1
  %165 = load i32, ptr %15, align 4
  ret i32 %165
}

; Function Attrs: noinline nounwind optnone uwtable
define linkonce_odr dso_local i32 @printf(ptr noundef %0, ...) #0 comdat {
  %2 = alloca ptr, align 8
  %3 = alloca i32, align 4
  %4 = alloca ptr, align 8
  store ptr %0, ptr %2, align 8
  call void @llvm.va_start.p0(ptr %4)
  %5 = load ptr, ptr %4, align 8
  %6 = load ptr, ptr %2, align 8
  %7 = call ptr @__acrt_iob_func(i32 noundef 1)
  %opaque_var = alloca i32, align 4
  store i32 90, ptr %opaque_var, align 4
  %x = load i32, ptr %opaque_var, align 4
  %8 = add i32 %x, 0
  %9 = icmp eq i32 %8, %x
  br i1 %9, label %obf_cont_0, label %obf_dead_0

obf_dead_0:                                       ; preds = %1
  %10 = alloca i32, align 4
  %11 = alloca i32, align 4
  %12 = load i32, ptr %10, align 4
  %13 = load i32, ptr %11, align 4
  %14 = add i32 %12, %13
  store i32 %14, ptr %10, align 4
  br label %obf_cont_0

obf_cont_0:                                       ; preds = %1, %obf_dead_0
  %15 = call i32 @_vfprintf_l(ptr noundef %7, ptr noundef %6, ptr noundef null, ptr noundef %5)
  store i32 %15, ptr %3, align 4
  call void @llvm.va_end.p0(ptr %4)
  %16 = load i32, ptr %3, align 4
  ret i32 %16
}

; Function Attrs: nocallback nofree nounwind willreturn memory(argmem: readwrite)
declare void @llvm.memcpy.p0.p0.i64(ptr noalias writeonly captures(none), ptr noalias readonly captures(none), i64, i1 immarg) #1

; Function Attrs: nocallback nofree nosync nounwind willreturn
declare void @llvm.va_start.p0(ptr) #2

; Function Attrs: noinline nounwind optnone uwtable
define linkonce_odr dso_local i32 @_vsprintf_l(ptr noundef %0, ptr noundef %1, ptr noundef %2, ptr noundef %3) #0 comdat {
  %5 = alloca ptr, align 8
  %6 = alloca ptr, align 8
  %7 = alloca ptr, align 8
  %8 = alloca ptr, align 8
  store ptr %3, ptr %5, align 8
  store ptr %2, ptr %6, align 8
  store ptr %1, ptr %7, align 8
  store ptr %0, ptr %8, align 8
  %9 = load ptr, ptr %5, align 8
  %opaque_var = alloca i32, align 4
  store i32 98, ptr %opaque_var, align 4
  %x = load i32, ptr %opaque_var, align 4
  %10 = shl i32 %x, 1
  %11 = lshr i32 %10, 1
  %12 = icmp eq i32 %11, %x
  br i1 %12, label %obf_cont_0, label %obf_dead_0

obf_dead_0:                                       ; preds = %4
  %13 = alloca i32, align 4
  %14 = alloca i32, align 4
  %15 = load i32, ptr %13, align 4
  %16 = load i32, ptr %14, align 4
  %17 = add i32 %15, %16
  store i32 %17, ptr %13, align 4
  br label %obf_cont_0

obf_cont_0:                                       ; preds = %4, %obf_dead_0
  %18 = load ptr, ptr %6, align 8
  %19 = load ptr, ptr %7, align 8
  %20 = load ptr, ptr %8, align 8
  %21 = call i32 @_vsnprintf_l(ptr noundef %20, i64 noundef -1, ptr noundef %19, ptr noundef %18, ptr noundef %9)
  ret i32 %21
}

; Function Attrs: nocallback nofree nosync nounwind willreturn
declare void @llvm.va_end.p0(ptr) #2

; Function Attrs: noinline nounwind optnone uwtable
define linkonce_odr dso_local i32 @_vsnprintf_l(ptr noundef %0, i64 noundef %1, ptr noundef %2, ptr noundef %3, ptr noundef %4) #0 comdat {
  %6 = alloca ptr, align 8
  %7 = alloca ptr, align 8
  %8 = alloca ptr, align 8
  %9 = alloca i64, align 8
  %10 = alloca ptr, align 8
  %11 = alloca i32, align 4
  store ptr %4, ptr %6, align 8
  store ptr %3, ptr %7, align 8
  store ptr %2, ptr %8, align 8
  store i64 %1, ptr %9, align 8
  store ptr %0, ptr %10, align 8
  %12 = load ptr, ptr %6, align 8
  %13 = load ptr, ptr %7, align 8
  %14 = load ptr, ptr %8, align 8
  %15 = load i64, ptr %9, align 8
  %16 = load ptr, ptr %10, align 8
  %17 = call ptr @__local_stdio_printf_options()
  %18 = load i64, ptr %17, align 8
  %19 = or i64 %18, 1
  %20 = call i32 @__stdio_common_vsprintf(i64 noundef %19, ptr noundef %16, i64 noundef %15, ptr noundef %14, ptr noundef %13, ptr noundef %12)
  store i32 %20, ptr %11, align 4
  %21 = load i32, ptr %11, align 4
  %22 = icmp slt i32 %21, 0
  %opaque_var = alloca i32, align 4
  store i32 60, ptr %opaque_var, align 4
  %x = load i32, ptr %opaque_var, align 4
  %23 = and i32 %x, %x
  %24 = icmp eq i32 %23, %x
  %25 = and i1 %22, %24
  br i1 %25, label %26, label %27

26:                                               ; preds = %5
  br label %29

27:                                               ; preds = %5
  %28 = load i32, ptr %11, align 4
  br label %29

29:                                               ; preds = %27, %26
  %30 = phi i32 [ -1, %26 ], [ %28, %27 ]
  ret i32 %30
}

declare dso_local i32 @__stdio_common_vsprintf(i64 noundef, ptr noundef, i64 noundef, ptr noundef, ptr noundef, ptr noundef) #3

; Function Attrs: noinline nounwind optnone uwtable
define linkonce_odr dso_local ptr @__local_stdio_printf_options() #0 comdat {
  ret ptr @__local_stdio_printf_options._OptionsStorage
}

; Function Attrs: noinline nounwind optnone uwtable
define linkonce_odr dso_local i32 @_vfprintf_l(ptr noundef %0, ptr noundef %1, ptr noundef %2, ptr noundef %3) #0 comdat {
  %5 = alloca ptr, align 8
  %6 = alloca ptr, align 8
  %7 = alloca ptr, align 8
  %8 = alloca ptr, align 8
  store ptr %3, ptr %5, align 8
  store ptr %2, ptr %6, align 8
  store ptr %1, ptr %7, align 8
  store ptr %0, ptr %8, align 8
  %9 = load ptr, ptr %5, align 8
  %10 = load ptr, ptr %6, align 8
  %opaque_var = alloca i32, align 4
  store i32 36, ptr %opaque_var, align 4
  %x = load i32, ptr %opaque_var, align 4
  %11 = xor i32 %x, -1
  %12 = and i32 %x, %11
  %13 = icmp eq i32 %12, 0
  br i1 %13, label %obf_cont_0, label %obf_dead_0

obf_dead_0:                                       ; preds = %4
  %14 = alloca i32, align 4
  %15 = alloca i32, align 4
  %16 = load i32, ptr %14, align 4
  %17 = load i32, ptr %15, align 4
  %18 = add i32 %16, %17
  store i32 %18, ptr %14, align 4
  br label %obf_cont_0

obf_cont_0:                                       ; preds = %4, %obf_dead_0
  %19 = load ptr, ptr %7, align 8
  %20 = load ptr, ptr %8, align 8
  %21 = call ptr @__local_stdio_printf_options()
  %22 = load i64, ptr %21, align 8
  %23 = call i32 @__stdio_common_vfprintf(i64 noundef %22, ptr noundef %20, ptr noundef %19, ptr noundef %10, ptr noundef %9)
  ret i32 %23
}

declare dso_local ptr @__acrt_iob_func(i32 noundef) #3

declare dso_local i32 @__stdio_common_vfprintf(i64 noundef, ptr noundef, ptr noundef, ptr noundef, ptr noundef) #3

define internal void @__obf_decrypt_ctor() {
entry:
  %0 = alloca i32, align 4
  store i32 0, ptr %0, align 4
  br label %decrypt_loop

decrypt_loop:                                     ; preds = %decrypt_body, %entry
  %1 = load i32, ptr %0, align 4
  %2 = icmp ult i32 %1, 28
  br i1 %2, label %decrypt_body, label %decrypt_exit

decrypt_body:                                     ; preds = %decrypt_loop
  %3 = load i32, ptr %0, align 4
  %4 = zext i32 %3 to i64
  %5 = getelementptr inbounds i8, ptr @"??_C@_0BM@FLBJKOEJ@Protected?5Application?5v1?40?6?$AA@", i64 %4
  %6 = load i8, ptr %5, align 1
  %7 = urem i32 %3, 2
  %8 = zext i32 %7 to i64
  %9 = getelementptr inbounds i8, ptr @__obf_key_6, i64 %8
  %10 = load i8, ptr %9, align 1
  %11 = xor i8 %10, 66
  %12 = trunc i32 %3 to i8
  %13 = xor i8 %11, %12
  %14 = xor i8 %13, 66
  %15 = xor i8 %6, %14
  store i8 %15, ptr %5, align 1
  %16 = add i32 %3, 1
  store i32 %16, ptr %0, align 4
  br label %decrypt_loop

decrypt_exit:                                     ; preds = %decrypt_loop
  %17 = alloca i32, align 4
  store i32 0, ptr %17, align 4
  br label %decrypt_loop1

decrypt_loop1:                                    ; preds = %decrypt_body2, %decrypt_exit
  %18 = load i32, ptr %17, align 4
  %19 = icmp ult i32 %18, 15
  br i1 %19, label %decrypt_body2, label %decrypt_exit3

decrypt_body2:                                    ; preds = %decrypt_loop1
  %20 = load i32, ptr %17, align 4
  %21 = zext i32 %20 to i64
  %22 = getelementptr inbounds i8, ptr @"??_C@_0P@CNIMNJLH@SECRET?9KEY?9123?$AA@", i64 %21
  %23 = load i8, ptr %22, align 1
  %24 = urem i32 %20, 2
  %25 = zext i32 %24 to i64
  %26 = getelementptr inbounds i8, ptr @__obf_key_6.1, i64 %25
  %27 = load i8, ptr %26, align 1
  %28 = xor i8 %27, 66
  %29 = trunc i32 %20 to i8
  %30 = xor i8 %28, %29
  %31 = xor i8 %30, 66
  %32 = xor i8 %23, %31
  store i8 %32, ptr %22, align 1
  %33 = add i32 %20, 1
  store i32 %33, ptr %17, align 4
  br label %decrypt_loop1

decrypt_exit3:                                    ; preds = %decrypt_loop1
  %34 = alloca i32, align 4
  store i32 0, ptr %34, align 4
  br label %decrypt_loop4

decrypt_loop4:                                    ; preds = %decrypt_body5, %decrypt_exit3
  %35 = load i32, ptr %34, align 4
  %36 = icmp ult i32 %35, 22
  br i1 %36, label %decrypt_body5, label %decrypt_exit6

decrypt_body5:                                    ; preds = %decrypt_loop4
  %37 = load i32, ptr %34, align 4
  %38 = zext i32 %37 to i64
  %39 = getelementptr inbounds i8, ptr @"??_C@_0BG@BOEPDAFA@Invalid?5license?5key?$CB?6?$AA@", i64 %38
  %40 = load i8, ptr %39, align 1
  %41 = urem i32 %37, 4
  %42 = zext i32 %41 to i64
  %43 = getelementptr inbounds i8, ptr @__obf_key_6.2, i64 %42
  %44 = load i8, ptr %43, align 1
  %45 = xor i8 %44, 66
  %46 = trunc i32 %37 to i8
  %47 = xor i8 %45, %46
  %48 = xor i8 %47, 66
  %49 = xor i8 %40, %48
  store i8 %49, ptr %39, align 1
  %50 = add i32 %37, 1
  store i32 %50, ptr %34, align 4
  br label %decrypt_loop4

decrypt_exit6:                                    ; preds = %decrypt_loop4
  %51 = alloca i32, align 4
  store i32 0, ptr %51, align 4
  br label %decrypt_loop7

decrypt_loop7:                                    ; preds = %decrypt_body8, %decrypt_exit6
  %52 = load i32, ptr %51, align 4
  %53 = icmp ult i32 %52, 33
  br i1 %53, label %decrypt_body8, label %decrypt_exit9

decrypt_body8:                                    ; preds = %decrypt_loop7
  %54 = load i32, ptr %51, align 4
  %55 = zext i32 %54 to i64
  %56 = getelementptr inbounds i8, ptr @"??_C@_0CB@NDEEDFKP@License?5validated?5successfully?4?6@", i64 %55
  %57 = load i8, ptr %56, align 1
  %58 = urem i32 %54, 2
  %59 = zext i32 %58 to i64
  %60 = getelementptr inbounds i8, ptr @__obf_key_6.3, i64 %59
  %61 = load i8, ptr %60, align 1
  %62 = xor i8 %61, 66
  %63 = trunc i32 %54 to i8
  %64 = xor i8 %62, %63
  %65 = xor i8 %64, 66
  %66 = xor i8 %57, %65
  store i8 %66, ptr %56, align 1
  %67 = add i32 %54, 1
  store i32 %67, ptr %51, align 4
  br label %decrypt_loop7

decrypt_exit9:                                    ; preds = %decrypt_loop7
  %68 = alloca i32, align 4
  store i32 0, ptr %68, align 4
  br label %decrypt_loop10

decrypt_loop10:                                   ; preds = %decrypt_body11, %decrypt_exit9
  %69 = load i32, ptr %68, align 4
  %70 = icmp ult i32 %69, 22
  br i1 %70, label %decrypt_body11, label %decrypt_exit12

decrypt_body11:                                   ; preds = %decrypt_loop10
  %71 = load i32, ptr %68, align 4
  %72 = zext i32 %71 to i64
  %73 = getelementptr inbounds i8, ptr @"??_C@_0BG@PLBFEBCH@Processing?3?5?$CFd?5?9?$DO?5?$CFd?6?$AA@", i64 %72
  %74 = load i8, ptr %73, align 1
  %75 = urem i32 %71, 2
  %76 = zext i32 %75 to i64
  %77 = getelementptr inbounds i8, ptr @__obf_key_6.4, i64 %76
  %78 = load i8, ptr %77, align 1
  %79 = xor i8 %78, 66
  %80 = trunc i32 %71 to i8
  %81 = xor i8 %79, %80
  %82 = xor i8 %81, 66
  %83 = xor i8 %74, %82
  store i8 %83, ptr %73, align 1
  %84 = add i32 %71, 1
  store i32 %84, ptr %68, align 4
  br label %decrypt_loop10

decrypt_exit12:                                   ; preds = %decrypt_loop10
  %85 = alloca i32, align 4
  store i32 0, ptr %85, align 4
  br label %decrypt_loop13

decrypt_loop13:                                   ; preds = %decrypt_body14, %decrypt_exit12
  %86 = load i32, ptr %85, align 4
  %87 = icmp ult i32 %86, 22
  br i1 %87, label %decrypt_body14, label %decrypt_exit15

decrypt_body14:                                   ; preds = %decrypt_loop13
  %88 = load i32, ptr %85, align 4
  %89 = zext i32 %88 to i64
  %90 = getelementptr inbounds i8, ptr @"??_C@_0BG@INDCFGHJ@Processing?5complete?4?6?$AA@", i64 %89
  %91 = load i8, ptr %90, align 1
  %92 = urem i32 %88, 4
  %93 = zext i32 %92 to i64
  %94 = getelementptr inbounds i8, ptr @__obf_key_6.5, i64 %93
  %95 = load i8, ptr %94, align 1
  %96 = xor i8 %95, 66
  %97 = trunc i32 %88 to i8
  %98 = xor i8 %96, %97
  %99 = xor i8 %98, 66
  %100 = xor i8 %91, %99
  store i8 %100, ptr %90, align 1
  %101 = add i32 %88, 1
  store i32 %101, ptr %85, align 4
  br label %decrypt_loop13

decrypt_exit15:                                   ; preds = %decrypt_loop13
  ret void
}

attributes #0 = { noinline nounwind optnone uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { nocallback nofree nounwind willreturn memory(argmem: readwrite) }
attributes #2 = { nocallback nofree nosync nounwind willreturn }
attributes #3 = { "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!2, !3, !4, !5, !6}
!llvm.ident = !{!7}

!0 = distinct !DICompileUnit(language: DW_LANG_C11, file: !1, producer: "clang version 21.1.2", isOptimized: false, runtimeVersion: 0, emissionKind: NoDebug, splitDebugInlining: false, nameTableKind: None)
!1 = !DIFile(filename: "examples\\simple_example.c", directory: "C:\\Users\\Akash\\Desktop\\New folder (8)")
!2 = !{i32 2, !"Debug Info Version", i32 3}
!3 = !{i32 1, !"wchar_size", i32 2}
!4 = !{i32 8, !"PIC Level", i32 2}
!5 = !{i32 7, !"uwtable", i32 2}
!6 = !{i32 1, !"MaxTLSAlign", i32 65536}
!7 = !{!"clang version 21.1.2"}
!8 = distinct !{!8, !9}
!9 = !{!"llvm.loop.mustprogress"}
!10 = distinct !{!10, !9}
!11 = distinct !{!11, !9}
