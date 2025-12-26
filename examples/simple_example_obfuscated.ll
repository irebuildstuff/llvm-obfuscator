; ModuleID = 'examples/simple_example.ll'
source_filename = "examples/simple_example.c"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc19.44.35217"

$sprintf = comdat any

$vsprintf = comdat any

$_snprintf = comdat any

$_vsnprintf = comdat any

$printf = comdat any

$_vsprintf_l = comdat any

$_vsnprintf_l = comdat any

$__local_stdio_printf_options = comdat any

$_vfprintf_l = comdat any

$"??_C@_0BM@FLBJKOEJ@Protected?5Application?5v1?40?6?$AA@" = comdat any

$"??_C@_0P@CNIMNJLH@SECRET?9KEY?9123?$AA@" = comdat any

$"??_C@_0BG@BOEPDAFA@Invalid?5license?5key?$CB?6?$AA@" = comdat any

$"??_C@_0CB@NDEEDFKP@License?5validated?5successfully?4?6@" = comdat any

$"??_C@_0BG@PLBFEBCH@Processing?3?5?$CFd?5?9?$DO?5?$CFd?6?$AA@" = comdat any

$"??_C@_0BG@INDCFGHJ@Processing?5complete?4?6?$AA@" = comdat any

@"??_C@_0BM@FLBJKOEJ@Protected?5Application?5v1?40?6?$AA@" = private unnamed_addr global [28 x i8] c"\120-6'!6'&b\0322.+!#6+-,b4slrHB\00", comdat, align 1
@"??_C@_0P@CNIMNJLH@SECRET?9KEY?9123?$AA@" = private unnamed_addr global [15 x i8] c"\11\07\01\10\07\16o\09\07\1BospqB\00", comdat, align 1
@"??_C@_0BG@BOEPDAFA@Invalid?5license?5key?$CB?6?$AA@" = private unnamed_addr global [22 x i8] c"\0B,4#.+&b.+!',1'b)';cHB\00", comdat, align 1
@"??_C@_0CB@NDEEDFKP@License?5validated?5successfully?4?6@" = private unnamed_addr global [33 x i8] c"\0E+!',1'b4#.+&#6'&b17!!'11$7..;lHB\00", comdat, align 1
@__const.main.data = private unnamed_addr constant [5 x i32] [i32 10, i32 20, i32 30, i32 40, i32 50], align 16
@"??_C@_0BG@PLBFEBCH@Processing?3?5?$CFd?5?9?$DO?5?$CFd?6?$AA@" = private unnamed_addr global [22 x i8] c"\120-!'11+,%xbg&bo|bg&HB\00", comdat, align 1
@"??_C@_0BG@INDCFGHJ@Processing?5complete?4?6?$AA@" = private unnamed_addr global [22 x i8] c"\120-!'11+,%b!-/2.'6'lHB\00", comdat, align 1
@__local_stdio_printf_options._OptionsStorage = internal global i64 0, align 8
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
  %9 = load ptr, ptr %4, align 8
  %10 = call i32 @_vsprintf_l(ptr noundef %9, ptr noundef %8, ptr noundef null, ptr noundef %7)
  store i32 %10, ptr %5, align 4
  call void @llvm.va_end.p0(ptr %6)
  %11 = load i32, ptr %5, align 4
  ret i32 %11
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
  %8 = load ptr, ptr %5, align 8
  %9 = load ptr, ptr %6, align 8
  %10 = call i32 @_vsnprintf_l(ptr noundef %9, i64 noundef -1, ptr noundef %8, ptr noundef null, ptr noundef %7)
  ret i32 %10
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
  %11 = load i64, ptr %5, align 8
  %12 = load ptr, ptr %6, align 8
  %13 = call i32 @_vsnprintf(ptr noundef %12, i64 noundef %11, ptr noundef %10, ptr noundef %9)
  store i32 %13, ptr %7, align 4
  call void @llvm.va_end.p0(ptr %8)
  %14 = load i32, ptr %7, align 4
  ret i32 %14
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
  %10 = load ptr, ptr %6, align 8
  %11 = load i64, ptr %7, align 8
  %12 = load ptr, ptr %8, align 8
  %13 = call i32 @_vsnprintf_l(ptr noundef %12, i64 noundef %11, ptr noundef %10, ptr noundef null, ptr noundef %9)
  ret i32 %13
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @secret_algorithm(i32 noundef %0) #0 {
  %2 = alloca i1, align 1
  %3 = alloca i32, align 4
  store i32 2, ptr %3, align 4
  %4 = load i32, ptr %3, align 4
  %5 = and i32 %4, 1
  %6 = icmp eq i32 %5, 0
  store i1 %6, ptr %2, align 1
  %7 = alloca i32, align 4
  %8 = alloca i32, align 4
  %9 = alloca i32, align 4
  %10 = alloca i32, align 4
  store i32 %0, ptr %7, align 4
  store i32 4919, ptr %8, align 4
  %11 = load i32, ptr %7, align 4
  store i32 %11, ptr %9, align 4
  store i32 0, ptr %10, align 4
  br label %fake_loop7

fake_loop7:                                       ; preds = %1, %fake_loop7
  %12 = alloca i32, align 4
  store i32 0, ptr %12, align 4
  %13 = load i32, ptr %12, align 4
  %14 = icmp slt i32 %13, 0
  br i1 %14, label %fake_loop7, label %fake_exit8

fake_exit8:                                       ; preds = %fake_loop7
  br label %fake_loop5

fake_loop5:                                       ; preds = %fake_exit8, %fake_loop5
  %15 = alloca i32, align 4
  store i32 0, ptr %15, align 4
  %16 = load i32, ptr %15, align 4
  %17 = icmp slt i32 %16, 0
  br i1 %17, label %fake_loop5, label %fake_exit6

fake_exit6:                                       ; preds = %fake_loop5
  br label %fake_loop3

fake_loop3:                                       ; preds = %fake_exit6, %fake_loop3
  %18 = alloca i32, align 4
  store i32 0, ptr %18, align 4
  %19 = load i32, ptr %18, align 4
  %20 = icmp slt i32 %19, 0
  br i1 %20, label %fake_loop3, label %fake_exit4

fake_exit4:                                       ; preds = %fake_loop3
  br label %fake_loop1

fake_loop1:                                       ; preds = %fake_exit4, %fake_loop1
  %21 = alloca i32, align 4
  store i32 0, ptr %21, align 4
  %22 = load i32, ptr %21, align 4
  %23 = icmp slt i32 %22, 0
  br i1 %23, label %fake_loop1, label %fake_exit2

fake_exit2:                                       ; preds = %fake_loop1
  br label %fake_loop

fake_loop:                                        ; preds = %fake_exit2, %fake_loop
  %24 = alloca i32, align 4
  store i32 0, ptr %24, align 4
  %25 = load i32, ptr %24, align 4
  %26 = icmp slt i32 %25, 0
  br i1 %26, label %fake_loop, label %fake_exit

fake_exit:                                        ; preds = %fake_loop
  br label %27

27:                                               ; preds = %fake_exit, %57
  %28 = alloca i1, align 1
  %29 = alloca i32, align 4
  store i32 2, ptr %29, align 4
  %30 = load i32, ptr %29, align 4
  %31 = and i32 %30, 1
  %32 = icmp eq i32 %31, 0
  store i1 %32, ptr %28, align 1
  %33 = load i32, ptr %10, align 4
  %34 = icmp slt i32 %33, 5
  %35 = alloca i32, align 4
  store i32 2, ptr %35, align 4
  %36 = load i32, ptr %35, align 4
  %37 = and i32 %36, 1
  %38 = icmp eq i32 %37, 0
  %39 = and i1 %34, %38
  br i1 %39, label %40, label %60

40:                                               ; preds = %27
  %41 = alloca i1, align 1
  %42 = alloca i32, align 4
  store i32 2, ptr %42, align 4
  %43 = load i32, ptr %42, align 4
  %44 = and i32 %43, 1
  %45 = icmp eq i32 %44, 0
  store i1 %45, ptr %41, align 1
  %46 = load i32, ptr %9, align 4
  %47 = mul nsw i32 %46, 3
  %48 = load i32, ptr %8, align 4
  %49 = add nsw i32 %47, %48
  %50 = load i32, ptr %8, align 4
  %51 = load i32, ptr %10, align 4
  %52 = ashr i32 %50, %51
  %53 = xor i32 %49, %52
  store i32 %53, ptr %9, align 4
  %54 = load i32, ptr %8, align 4
  %55 = shl i32 %54, 1
  %56 = or i32 %55, 1
  store i32 %56, ptr %8, align 4
  br label %57

57:                                               ; preds = %40
  %58 = load i32, ptr %10, align 4
  %59 = add nsw i32 %58, 1
  store i32 %59, ptr %10, align 4
  br label %27, !llvm.loop !8

60:                                               ; preds = %27
  %61 = load i32, ptr %9, align 4
  ret i32 %61
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @validate_license(ptr noundef %0) #0 {
  %2 = alloca i1, align 1
  %3 = alloca i32, align 4
  store i32 2, ptr %3, align 4
  %4 = load i32, ptr %3, align 4
  %5 = and i32 %4, 1
  %6 = icmp eq i32 %5, 0
  store i1 %6, ptr %2, align 1
  %7 = alloca i32, align 4
  %8 = alloca ptr, align 8
  %9 = alloca i32, align 4
  %10 = alloca i32, align 4
  store ptr %0, ptr %8, align 8
  %11 = load ptr, ptr %8, align 8
  %12 = icmp eq ptr %11, null
  %13 = alloca i32, align 4
  store i32 2, ptr %13, align 4
  %14 = load i32, ptr %13, align 4
  %15 = and i32 %14, 1
  %16 = icmp eq i32 %15, 0
  %17 = and i1 %12, %16
  br i1 %17, label %fake_loop7, label %34

fake_loop7:                                       ; preds = %1, %fake_loop7
  %18 = alloca i32, align 4
  store i32 0, ptr %18, align 4
  %19 = load i32, ptr %18, align 4
  %20 = icmp slt i32 %19, 0
  br i1 %20, label %fake_loop7, label %fake_exit8

fake_exit8:                                       ; preds = %fake_loop7
  br label %fake_loop5

fake_loop5:                                       ; preds = %fake_exit8, %fake_loop5
  %21 = alloca i32, align 4
  store i32 0, ptr %21, align 4
  %22 = load i32, ptr %21, align 4
  %23 = icmp slt i32 %22, 0
  br i1 %23, label %fake_loop5, label %fake_exit6

fake_exit6:                                       ; preds = %fake_loop5
  br label %fake_loop3

fake_loop3:                                       ; preds = %fake_exit6, %fake_loop3
  %24 = alloca i32, align 4
  store i32 0, ptr %24, align 4
  %25 = load i32, ptr %24, align 4
  %26 = icmp slt i32 %25, 0
  br i1 %26, label %fake_loop3, label %fake_exit4

fake_exit4:                                       ; preds = %fake_loop3
  br label %fake_loop1

fake_loop1:                                       ; preds = %fake_exit4, %fake_loop1
  %27 = alloca i32, align 4
  store i32 0, ptr %27, align 4
  %28 = load i32, ptr %27, align 4
  %29 = icmp slt i32 %28, 0
  br i1 %29, label %fake_loop1, label %fake_exit2

fake_exit2:                                       ; preds = %fake_loop1
  br label %fake_loop

fake_loop:                                        ; preds = %fake_exit2, %fake_loop
  %30 = alloca i32, align 4
  store i32 0, ptr %30, align 4
  %31 = load i32, ptr %30, align 4
  %32 = icmp slt i32 %31, 0
  br i1 %32, label %fake_loop, label %fake_exit

fake_exit:                                        ; preds = %fake_loop
  br label %33

33:                                               ; preds = %fake_exit
  store i32 0, ptr %7, align 4
  br label %83

34:                                               ; preds = %1
  store i32 0, ptr %9, align 4
  store i32 0, ptr %10, align 4
  br label %35

35:                                               ; preds = %70, %34
  %36 = alloca i1, align 1
  %37 = alloca i32, align 4
  store i32 2, ptr %37, align 4
  %38 = load i32, ptr %37, align 4
  %39 = and i32 %38, 1
  %40 = icmp eq i32 %39, 0
  store i1 %40, ptr %36, align 1
  %41 = load ptr, ptr %8, align 8
  %42 = load i32, ptr %10, align 4
  %43 = sext i32 %42 to i64
  %44 = getelementptr inbounds i8, ptr %41, i64 %43
  %45 = load i8, ptr %44, align 1
  %46 = sext i8 %45 to i32
  %47 = icmp ne i32 %46, 0
  %48 = alloca i32, align 4
  store i32 2, ptr %48, align 4
  %49 = load i32, ptr %48, align 4
  %50 = and i32 %49, 1
  %51 = icmp eq i32 %50, 0
  %52 = and i1 %47, %51
  br i1 %52, label %53, label %73

53:                                               ; preds = %35
  %54 = alloca i1, align 1
  %55 = alloca i32, align 4
  store i32 2, ptr %55, align 4
  %56 = load i32, ptr %55, align 4
  %57 = and i32 %56, 1
  %58 = icmp eq i32 %57, 0
  store i1 %58, ptr %54, align 1
  %59 = load ptr, ptr %8, align 8
  %60 = load i32, ptr %10, align 4
  %61 = sext i32 %60 to i64
  %62 = getelementptr inbounds i8, ptr %59, i64 %61
  %63 = load i8, ptr %62, align 1
  %64 = sext i8 %63 to i32
  %65 = load i32, ptr %10, align 4
  %66 = add nsw i32 %65, 1
  %67 = mul nsw i32 %64, %66
  %68 = load i32, ptr %9, align 4
  %69 = add nsw i32 %68, %67
  store i32 %69, ptr %9, align 4
  br label %70

70:                                               ; preds = %53
  %71 = load i32, ptr %10, align 4
  %72 = add nsw i32 %71, 1
  store i32 %72, ptr %10, align 4
  br label %35, !llvm.loop !10

73:                                               ; preds = %35
  %74 = alloca i1, align 1
  %75 = alloca i32, align 4
  store i32 2, ptr %75, align 4
  %76 = load i32, ptr %75, align 4
  %77 = and i32 %76, 1
  %78 = icmp eq i32 %77, 0
  store i1 %78, ptr %74, align 1
  %79 = load i32, ptr %9, align 4
  %80 = srem i32 %79, 1337
  %81 = icmp eq i32 %80, 42
  %82 = zext i1 %81 to i32
  store i32 %82, ptr %7, align 4
  br label %83

83:                                               ; preds = %73, %33
  %84 = load i32, ptr %7, align 4
  ret i32 %84
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main(i32 noundef %0, ptr noundef %1) #0 {
  %3 = alloca i1, align 1
  %4 = alloca i32, align 4
  store i32 2, ptr %4, align 4
  %5 = load i32, ptr %4, align 4
  %6 = and i32 %5, 1
  %7 = icmp eq i32 %6, 0
  store i1 %7, ptr %3, align 1
  %8 = alloca i32, align 4
  %9 = alloca ptr, align 8
  %10 = alloca i32, align 4
  %11 = alloca ptr, align 8
  %12 = alloca [5 x i32], align 16
  %13 = alloca [5 x i32], align 16
  %14 = alloca i32, align 4
  store i32 0, ptr %8, align 4
  store ptr %1, ptr %9, align 8
  store i32 %0, ptr %10, align 4
  %15 = call i32 (ptr, ...) @printf(ptr noundef @"??_C@_0BM@FLBJKOEJ@Protected?5Application?5v1?40?6?$AA@")
  store ptr @"??_C@_0P@CNIMNJLH@SECRET?9KEY?9123?$AA@", ptr %11, align 8
  %16 = load ptr, ptr %11, align 8
  %17 = call i32 @validate_license(ptr noundef %16)
  %18 = icmp ne i32 %17, 0
  %19 = alloca i32, align 4
  store i32 2, ptr %19, align 4
  %20 = load i32, ptr %19, align 4
  %21 = and i32 %20, 1
  %22 = icmp eq i32 %21, 0
  %23 = and i1 %18, %22
  br i1 %23, label %fake_loop7, label %24

24:                                               ; preds = %2
  %25 = call i32 (ptr, ...) @printf(ptr noundef @"??_C@_0BG@BOEPDAFA@Invalid?5license?5key?$CB?6?$AA@")
  store i32 1, ptr %8, align 4
  br label %84

fake_loop7:                                       ; preds = %2, %fake_loop7
  %26 = alloca i32, align 4
  store i32 0, ptr %26, align 4
  %27 = load i32, ptr %26, align 4
  %28 = icmp slt i32 %27, 0
  br i1 %28, label %fake_loop7, label %fake_exit8

fake_exit8:                                       ; preds = %fake_loop7
  br label %fake_loop5

fake_loop5:                                       ; preds = %fake_exit8, %fake_loop5
  %29 = alloca i32, align 4
  store i32 0, ptr %29, align 4
  %30 = load i32, ptr %29, align 4
  %31 = icmp slt i32 %30, 0
  br i1 %31, label %fake_loop5, label %fake_exit6

fake_exit6:                                       ; preds = %fake_loop5
  br label %fake_loop3

fake_loop3:                                       ; preds = %fake_exit6, %fake_loop3
  %32 = alloca i32, align 4
  store i32 0, ptr %32, align 4
  %33 = load i32, ptr %32, align 4
  %34 = icmp slt i32 %33, 0
  br i1 %34, label %fake_loop3, label %fake_exit4

fake_exit4:                                       ; preds = %fake_loop3
  br label %fake_loop1

fake_loop1:                                       ; preds = %fake_exit4, %fake_loop1
  %35 = alloca i32, align 4
  store i32 0, ptr %35, align 4
  %36 = load i32, ptr %35, align 4
  %37 = icmp slt i32 %36, 0
  br i1 %37, label %fake_loop1, label %fake_exit2

fake_exit2:                                       ; preds = %fake_loop1
  br label %fake_loop

fake_loop:                                        ; preds = %fake_exit2, %fake_loop
  %38 = alloca i32, align 4
  store i32 0, ptr %38, align 4
  %39 = load i32, ptr %38, align 4
  %40 = icmp slt i32 %39, 0
  br i1 %40, label %fake_loop, label %fake_exit

fake_exit:                                        ; preds = %fake_loop
  br label %41

41:                                               ; preds = %fake_exit
  %42 = call i32 (ptr, ...) @printf(ptr noundef @"??_C@_0CB@NDEEDFKP@License?5validated?5successfully?4?6@")
  call void @llvm.memcpy.p0.p0.i64(ptr align 16 %12, ptr align 16 @__const.main.data, i64 20, i1 false)
  store i32 0, ptr %14, align 4
  br label %43

43:                                               ; preds = %79, %41
  %44 = alloca i1, align 1
  %45 = alloca i32, align 4
  store i32 2, ptr %45, align 4
  %46 = load i32, ptr %45, align 4
  %47 = and i32 %46, 1
  %48 = icmp eq i32 %47, 0
  store i1 %48, ptr %44, align 1
  %49 = load i32, ptr %14, align 4
  %50 = icmp slt i32 %49, 5
  %51 = alloca i32, align 4
  store i32 2, ptr %51, align 4
  %52 = load i32, ptr %51, align 4
  %53 = and i32 %52, 1
  %54 = icmp eq i32 %53, 0
  %55 = and i1 %50, %54
  br i1 %55, label %56, label %82

56:                                               ; preds = %43
  %57 = alloca i1, align 1
  %58 = alloca i32, align 4
  store i32 2, ptr %58, align 4
  %59 = load i32, ptr %58, align 4
  %60 = and i32 %59, 1
  %61 = icmp eq i32 %60, 0
  store i1 %61, ptr %57, align 1
  %62 = load i32, ptr %14, align 4
  %63 = sext i32 %62 to i64
  %64 = getelementptr inbounds [5 x i32], ptr %12, i64 0, i64 %63
  %65 = load i32, ptr %64, align 4
  %66 = call i32 @secret_algorithm(i32 noundef %65)
  %67 = load i32, ptr %14, align 4
  %68 = sext i32 %67 to i64
  %69 = getelementptr inbounds [5 x i32], ptr %13, i64 0, i64 %68
  store i32 %66, ptr %69, align 4
  %70 = load i32, ptr %14, align 4
  %71 = sext i32 %70 to i64
  %72 = getelementptr inbounds [5 x i32], ptr %13, i64 0, i64 %71
  %73 = load i32, ptr %72, align 4
  %74 = load i32, ptr %14, align 4
  %75 = sext i32 %74 to i64
  %76 = getelementptr inbounds [5 x i32], ptr %12, i64 0, i64 %75
  %77 = load i32, ptr %76, align 4
  %78 = call i32 (ptr, ...) @printf(ptr noundef @"??_C@_0BG@PLBFEBCH@Processing?3?5?$CFd?5?9?$DO?5?$CFd?6?$AA@", i32 noundef %77, i32 noundef %73)
  br label %79

79:                                               ; preds = %56
  %80 = load i32, ptr %14, align 4
  %81 = add nsw i32 %80, 1
  store i32 %81, ptr %14, align 4
  br label %43, !llvm.loop !11

82:                                               ; preds = %43
  %83 = call i32 (ptr, ...) @printf(ptr noundef @"??_C@_0BG@INDCFGHJ@Processing?5complete?4?6?$AA@")
  store i32 0, ptr %8, align 4
  br label %84

84:                                               ; preds = %82, %24
  %85 = load i32, ptr %8, align 4
  ret i32 %85
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
  %8 = call i32 @_vfprintf_l(ptr noundef %7, ptr noundef %6, ptr noundef null, ptr noundef %5)
  store i32 %8, ptr %3, align 4
  call void @llvm.va_end.p0(ptr %4)
  %9 = load i32, ptr %3, align 4
  ret i32 %9
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
  %10 = load ptr, ptr %6, align 8
  %11 = load ptr, ptr %7, align 8
  %12 = load ptr, ptr %8, align 8
  %13 = call i32 @_vsnprintf_l(ptr noundef %12, i64 noundef -1, ptr noundef %11, ptr noundef %10, ptr noundef %9)
  ret i32 %13
}

; Function Attrs: nocallback nofree nosync nounwind willreturn
declare void @llvm.va_end.p0(ptr) #2

; Function Attrs: noinline nounwind optnone uwtable
define linkonce_odr dso_local i32 @_vsnprintf_l(ptr noundef %0, i64 noundef %1, ptr noundef %2, ptr noundef %3, ptr noundef %4) #0 comdat {
  %6 = alloca i1, align 1
  %7 = alloca i32, align 4
  store i32 2, ptr %7, align 4
  %8 = load i32, ptr %7, align 4
  %9 = and i32 %8, 1
  %10 = icmp eq i32 %9, 0
  store i1 %10, ptr %6, align 1
  %11 = alloca ptr, align 8
  %12 = alloca ptr, align 8
  %13 = alloca ptr, align 8
  %14 = alloca i64, align 8
  %15 = alloca ptr, align 8
  %16 = alloca i32, align 4
  store ptr %4, ptr %11, align 8
  store ptr %3, ptr %12, align 8
  store ptr %2, ptr %13, align 8
  store i64 %1, ptr %14, align 8
  store ptr %0, ptr %15, align 8
  %17 = load ptr, ptr %11, align 8
  %18 = load ptr, ptr %12, align 8
  %19 = load ptr, ptr %13, align 8
  %20 = load i64, ptr %14, align 8
  %21 = load ptr, ptr %15, align 8
  %22 = call ptr @__local_stdio_printf_options()
  %23 = load i64, ptr %22, align 8
  %24 = or i64 %23, 1
  %25 = call i32 @__stdio_common_vsprintf(i64 noundef %24, ptr noundef %21, i64 noundef %20, ptr noundef %19, ptr noundef %18, ptr noundef %17)
  store i32 %25, ptr %16, align 4
  %26 = load i32, ptr %16, align 4
  %27 = icmp slt i32 %26, 0
  %28 = alloca i32, align 4
  store i32 2, ptr %28, align 4
  %29 = load i32, ptr %28, align 4
  %30 = and i32 %29, 1
  %31 = icmp eq i32 %30, 0
  %32 = and i1 %27, %31
  br i1 %32, label %fake_loop7, label %49

fake_loop7:                                       ; preds = %5, %fake_loop7
  %33 = alloca i32, align 4
  store i32 0, ptr %33, align 4
  %34 = load i32, ptr %33, align 4
  %35 = icmp slt i32 %34, 0
  br i1 %35, label %fake_loop7, label %fake_exit8

fake_exit8:                                       ; preds = %fake_loop7
  br label %fake_loop5

fake_loop5:                                       ; preds = %fake_exit8, %fake_loop5
  %36 = alloca i32, align 4
  store i32 0, ptr %36, align 4
  %37 = load i32, ptr %36, align 4
  %38 = icmp slt i32 %37, 0
  br i1 %38, label %fake_loop5, label %fake_exit6

fake_exit6:                                       ; preds = %fake_loop5
  br label %fake_loop3

fake_loop3:                                       ; preds = %fake_exit6, %fake_loop3
  %39 = alloca i32, align 4
  store i32 0, ptr %39, align 4
  %40 = load i32, ptr %39, align 4
  %41 = icmp slt i32 %40, 0
  br i1 %41, label %fake_loop3, label %fake_exit4

fake_exit4:                                       ; preds = %fake_loop3
  br label %fake_loop1

fake_loop1:                                       ; preds = %fake_exit4, %fake_loop1
  %42 = alloca i32, align 4
  store i32 0, ptr %42, align 4
  %43 = load i32, ptr %42, align 4
  %44 = icmp slt i32 %43, 0
  br i1 %44, label %fake_loop1, label %fake_exit2

fake_exit2:                                       ; preds = %fake_loop1
  br label %fake_loop

fake_loop:                                        ; preds = %fake_exit2, %fake_loop
  %45 = alloca i32, align 4
  store i32 0, ptr %45, align 4
  %46 = load i32, ptr %45, align 4
  %47 = icmp slt i32 %46, 0
  br i1 %47, label %fake_loop, label %fake_exit

fake_exit:                                        ; preds = %fake_loop
  br label %48

48:                                               ; preds = %fake_exit
  br label %51

49:                                               ; preds = %5
  %50 = load i32, ptr %16, align 4
  br label %51

51:                                               ; preds = %49, %48
  %52 = phi i32 [ -1, %48 ], [ %50, %49 ]
  ret i32 %52
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
  %11 = load ptr, ptr %7, align 8
  %12 = load ptr, ptr %8, align 8
  %13 = call ptr @__local_stdio_printf_options()
  %14 = load i64, ptr %13, align 8
  %15 = call i32 @__stdio_common_vfprintf(i64 noundef %14, ptr noundef %12, ptr noundef %11, ptr noundef %10, ptr noundef %9)
  ret i32 %15
}

declare dso_local ptr @__acrt_iob_func(i32 noundef) #3

declare dso_local i32 @__stdio_common_vfprintf(i64 noundef, ptr noundef, ptr noundef, ptr noundef, ptr noundef) #3

define internal void @__obf_decrypt_ctor() {
entry:
  %0 = load i8, ptr @"??_C@_0BM@FLBJKOEJ@Protected?5Application?5v1?40?6?$AA@", align 1
  %1 = xor i8 %0, 66
  store i8 %1, ptr @"??_C@_0BM@FLBJKOEJ@Protected?5Application?5v1?40?6?$AA@", align 1
  %2 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0BM@FLBJKOEJ@Protected?5Application?5v1?40?6?$AA@", i64 1), align 1
  %3 = xor i8 %2, 66
  store i8 %3, ptr getelementptr inbounds (i8, ptr @"??_C@_0BM@FLBJKOEJ@Protected?5Application?5v1?40?6?$AA@", i64 1), align 1
  %4 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0BM@FLBJKOEJ@Protected?5Application?5v1?40?6?$AA@", i64 2), align 1
  %5 = xor i8 %4, 66
  store i8 %5, ptr getelementptr inbounds (i8, ptr @"??_C@_0BM@FLBJKOEJ@Protected?5Application?5v1?40?6?$AA@", i64 2), align 1
  %6 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0BM@FLBJKOEJ@Protected?5Application?5v1?40?6?$AA@", i64 3), align 1
  %7 = xor i8 %6, 66
  store i8 %7, ptr getelementptr inbounds (i8, ptr @"??_C@_0BM@FLBJKOEJ@Protected?5Application?5v1?40?6?$AA@", i64 3), align 1
  %8 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0BM@FLBJKOEJ@Protected?5Application?5v1?40?6?$AA@", i64 4), align 1
  %9 = xor i8 %8, 66
  store i8 %9, ptr getelementptr inbounds (i8, ptr @"??_C@_0BM@FLBJKOEJ@Protected?5Application?5v1?40?6?$AA@", i64 4), align 1
  %10 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0BM@FLBJKOEJ@Protected?5Application?5v1?40?6?$AA@", i64 5), align 1
  %11 = xor i8 %10, 66
  store i8 %11, ptr getelementptr inbounds (i8, ptr @"??_C@_0BM@FLBJKOEJ@Protected?5Application?5v1?40?6?$AA@", i64 5), align 1
  %12 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0BM@FLBJKOEJ@Protected?5Application?5v1?40?6?$AA@", i64 6), align 1
  %13 = xor i8 %12, 66
  store i8 %13, ptr getelementptr inbounds (i8, ptr @"??_C@_0BM@FLBJKOEJ@Protected?5Application?5v1?40?6?$AA@", i64 6), align 1
  %14 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0BM@FLBJKOEJ@Protected?5Application?5v1?40?6?$AA@", i64 7), align 1
  %15 = xor i8 %14, 66
  store i8 %15, ptr getelementptr inbounds (i8, ptr @"??_C@_0BM@FLBJKOEJ@Protected?5Application?5v1?40?6?$AA@", i64 7), align 1
  %16 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0BM@FLBJKOEJ@Protected?5Application?5v1?40?6?$AA@", i64 8), align 1
  %17 = xor i8 %16, 66
  store i8 %17, ptr getelementptr inbounds (i8, ptr @"??_C@_0BM@FLBJKOEJ@Protected?5Application?5v1?40?6?$AA@", i64 8), align 1
  %18 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0BM@FLBJKOEJ@Protected?5Application?5v1?40?6?$AA@", i64 9), align 1
  %19 = xor i8 %18, 66
  store i8 %19, ptr getelementptr inbounds (i8, ptr @"??_C@_0BM@FLBJKOEJ@Protected?5Application?5v1?40?6?$AA@", i64 9), align 1
  %20 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0BM@FLBJKOEJ@Protected?5Application?5v1?40?6?$AA@", i64 10), align 1
  %21 = xor i8 %20, 66
  store i8 %21, ptr getelementptr inbounds (i8, ptr @"??_C@_0BM@FLBJKOEJ@Protected?5Application?5v1?40?6?$AA@", i64 10), align 1
  %22 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0BM@FLBJKOEJ@Protected?5Application?5v1?40?6?$AA@", i64 11), align 1
  %23 = xor i8 %22, 66
  store i8 %23, ptr getelementptr inbounds (i8, ptr @"??_C@_0BM@FLBJKOEJ@Protected?5Application?5v1?40?6?$AA@", i64 11), align 1
  %24 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0BM@FLBJKOEJ@Protected?5Application?5v1?40?6?$AA@", i64 12), align 1
  %25 = xor i8 %24, 66
  store i8 %25, ptr getelementptr inbounds (i8, ptr @"??_C@_0BM@FLBJKOEJ@Protected?5Application?5v1?40?6?$AA@", i64 12), align 1
  %26 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0BM@FLBJKOEJ@Protected?5Application?5v1?40?6?$AA@", i64 13), align 1
  %27 = xor i8 %26, 66
  store i8 %27, ptr getelementptr inbounds (i8, ptr @"??_C@_0BM@FLBJKOEJ@Protected?5Application?5v1?40?6?$AA@", i64 13), align 1
  %28 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0BM@FLBJKOEJ@Protected?5Application?5v1?40?6?$AA@", i64 14), align 1
  %29 = xor i8 %28, 66
  store i8 %29, ptr getelementptr inbounds (i8, ptr @"??_C@_0BM@FLBJKOEJ@Protected?5Application?5v1?40?6?$AA@", i64 14), align 1
  %30 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0BM@FLBJKOEJ@Protected?5Application?5v1?40?6?$AA@", i64 15), align 1
  %31 = xor i8 %30, 66
  store i8 %31, ptr getelementptr inbounds (i8, ptr @"??_C@_0BM@FLBJKOEJ@Protected?5Application?5v1?40?6?$AA@", i64 15), align 1
  %32 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0BM@FLBJKOEJ@Protected?5Application?5v1?40?6?$AA@", i64 16), align 1
  %33 = xor i8 %32, 66
  store i8 %33, ptr getelementptr inbounds (i8, ptr @"??_C@_0BM@FLBJKOEJ@Protected?5Application?5v1?40?6?$AA@", i64 16), align 1
  %34 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0BM@FLBJKOEJ@Protected?5Application?5v1?40?6?$AA@", i64 17), align 1
  %35 = xor i8 %34, 66
  store i8 %35, ptr getelementptr inbounds (i8, ptr @"??_C@_0BM@FLBJKOEJ@Protected?5Application?5v1?40?6?$AA@", i64 17), align 1
  %36 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0BM@FLBJKOEJ@Protected?5Application?5v1?40?6?$AA@", i64 18), align 1
  %37 = xor i8 %36, 66
  store i8 %37, ptr getelementptr inbounds (i8, ptr @"??_C@_0BM@FLBJKOEJ@Protected?5Application?5v1?40?6?$AA@", i64 18), align 1
  %38 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0BM@FLBJKOEJ@Protected?5Application?5v1?40?6?$AA@", i64 19), align 1
  %39 = xor i8 %38, 66
  store i8 %39, ptr getelementptr inbounds (i8, ptr @"??_C@_0BM@FLBJKOEJ@Protected?5Application?5v1?40?6?$AA@", i64 19), align 1
  %40 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0BM@FLBJKOEJ@Protected?5Application?5v1?40?6?$AA@", i64 20), align 1
  %41 = xor i8 %40, 66
  store i8 %41, ptr getelementptr inbounds (i8, ptr @"??_C@_0BM@FLBJKOEJ@Protected?5Application?5v1?40?6?$AA@", i64 20), align 1
  %42 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0BM@FLBJKOEJ@Protected?5Application?5v1?40?6?$AA@", i64 21), align 1
  %43 = xor i8 %42, 66
  store i8 %43, ptr getelementptr inbounds (i8, ptr @"??_C@_0BM@FLBJKOEJ@Protected?5Application?5v1?40?6?$AA@", i64 21), align 1
  %44 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0BM@FLBJKOEJ@Protected?5Application?5v1?40?6?$AA@", i64 22), align 1
  %45 = xor i8 %44, 66
  store i8 %45, ptr getelementptr inbounds (i8, ptr @"??_C@_0BM@FLBJKOEJ@Protected?5Application?5v1?40?6?$AA@", i64 22), align 1
  %46 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0BM@FLBJKOEJ@Protected?5Application?5v1?40?6?$AA@", i64 23), align 1
  %47 = xor i8 %46, 66
  store i8 %47, ptr getelementptr inbounds (i8, ptr @"??_C@_0BM@FLBJKOEJ@Protected?5Application?5v1?40?6?$AA@", i64 23), align 1
  %48 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0BM@FLBJKOEJ@Protected?5Application?5v1?40?6?$AA@", i64 24), align 1
  %49 = xor i8 %48, 66
  store i8 %49, ptr getelementptr inbounds (i8, ptr @"??_C@_0BM@FLBJKOEJ@Protected?5Application?5v1?40?6?$AA@", i64 24), align 1
  %50 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0BM@FLBJKOEJ@Protected?5Application?5v1?40?6?$AA@", i64 25), align 1
  %51 = xor i8 %50, 66
  store i8 %51, ptr getelementptr inbounds (i8, ptr @"??_C@_0BM@FLBJKOEJ@Protected?5Application?5v1?40?6?$AA@", i64 25), align 1
  %52 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0BM@FLBJKOEJ@Protected?5Application?5v1?40?6?$AA@", i64 26), align 1
  %53 = xor i8 %52, 66
  store i8 %53, ptr getelementptr inbounds (i8, ptr @"??_C@_0BM@FLBJKOEJ@Protected?5Application?5v1?40?6?$AA@", i64 26), align 1
  %54 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0BM@FLBJKOEJ@Protected?5Application?5v1?40?6?$AA@", i64 27), align 1
  %55 = xor i8 %54, 66
  store i8 %55, ptr getelementptr inbounds (i8, ptr @"??_C@_0BM@FLBJKOEJ@Protected?5Application?5v1?40?6?$AA@", i64 27), align 1
  %56 = load i8, ptr @"??_C@_0P@CNIMNJLH@SECRET?9KEY?9123?$AA@", align 1
  %57 = xor i8 %56, 66
  store i8 %57, ptr @"??_C@_0P@CNIMNJLH@SECRET?9KEY?9123?$AA@", align 1
  %58 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0P@CNIMNJLH@SECRET?9KEY?9123?$AA@", i64 1), align 1
  %59 = xor i8 %58, 66
  store i8 %59, ptr getelementptr inbounds (i8, ptr @"??_C@_0P@CNIMNJLH@SECRET?9KEY?9123?$AA@", i64 1), align 1
  %60 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0P@CNIMNJLH@SECRET?9KEY?9123?$AA@", i64 2), align 1
  %61 = xor i8 %60, 66
  store i8 %61, ptr getelementptr inbounds (i8, ptr @"??_C@_0P@CNIMNJLH@SECRET?9KEY?9123?$AA@", i64 2), align 1
  %62 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0P@CNIMNJLH@SECRET?9KEY?9123?$AA@", i64 3), align 1
  %63 = xor i8 %62, 66
  store i8 %63, ptr getelementptr inbounds (i8, ptr @"??_C@_0P@CNIMNJLH@SECRET?9KEY?9123?$AA@", i64 3), align 1
  %64 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0P@CNIMNJLH@SECRET?9KEY?9123?$AA@", i64 4), align 1
  %65 = xor i8 %64, 66
  store i8 %65, ptr getelementptr inbounds (i8, ptr @"??_C@_0P@CNIMNJLH@SECRET?9KEY?9123?$AA@", i64 4), align 1
  %66 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0P@CNIMNJLH@SECRET?9KEY?9123?$AA@", i64 5), align 1
  %67 = xor i8 %66, 66
  store i8 %67, ptr getelementptr inbounds (i8, ptr @"??_C@_0P@CNIMNJLH@SECRET?9KEY?9123?$AA@", i64 5), align 1
  %68 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0P@CNIMNJLH@SECRET?9KEY?9123?$AA@", i64 6), align 1
  %69 = xor i8 %68, 66
  store i8 %69, ptr getelementptr inbounds (i8, ptr @"??_C@_0P@CNIMNJLH@SECRET?9KEY?9123?$AA@", i64 6), align 1
  %70 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0P@CNIMNJLH@SECRET?9KEY?9123?$AA@", i64 7), align 1
  %71 = xor i8 %70, 66
  store i8 %71, ptr getelementptr inbounds (i8, ptr @"??_C@_0P@CNIMNJLH@SECRET?9KEY?9123?$AA@", i64 7), align 1
  %72 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0P@CNIMNJLH@SECRET?9KEY?9123?$AA@", i64 8), align 1
  %73 = xor i8 %72, 66
  store i8 %73, ptr getelementptr inbounds (i8, ptr @"??_C@_0P@CNIMNJLH@SECRET?9KEY?9123?$AA@", i64 8), align 1
  %74 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0P@CNIMNJLH@SECRET?9KEY?9123?$AA@", i64 9), align 1
  %75 = xor i8 %74, 66
  store i8 %75, ptr getelementptr inbounds (i8, ptr @"??_C@_0P@CNIMNJLH@SECRET?9KEY?9123?$AA@", i64 9), align 1
  %76 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0P@CNIMNJLH@SECRET?9KEY?9123?$AA@", i64 10), align 1
  %77 = xor i8 %76, 66
  store i8 %77, ptr getelementptr inbounds (i8, ptr @"??_C@_0P@CNIMNJLH@SECRET?9KEY?9123?$AA@", i64 10), align 1
  %78 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0P@CNIMNJLH@SECRET?9KEY?9123?$AA@", i64 11), align 1
  %79 = xor i8 %78, 66
  store i8 %79, ptr getelementptr inbounds (i8, ptr @"??_C@_0P@CNIMNJLH@SECRET?9KEY?9123?$AA@", i64 11), align 1
  %80 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0P@CNIMNJLH@SECRET?9KEY?9123?$AA@", i64 12), align 1
  %81 = xor i8 %80, 66
  store i8 %81, ptr getelementptr inbounds (i8, ptr @"??_C@_0P@CNIMNJLH@SECRET?9KEY?9123?$AA@", i64 12), align 1
  %82 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0P@CNIMNJLH@SECRET?9KEY?9123?$AA@", i64 13), align 1
  %83 = xor i8 %82, 66
  store i8 %83, ptr getelementptr inbounds (i8, ptr @"??_C@_0P@CNIMNJLH@SECRET?9KEY?9123?$AA@", i64 13), align 1
  %84 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0P@CNIMNJLH@SECRET?9KEY?9123?$AA@", i64 14), align 1
  %85 = xor i8 %84, 66
  store i8 %85, ptr getelementptr inbounds (i8, ptr @"??_C@_0P@CNIMNJLH@SECRET?9KEY?9123?$AA@", i64 14), align 1
  %86 = load i8, ptr @"??_C@_0BG@BOEPDAFA@Invalid?5license?5key?$CB?6?$AA@", align 1
  %87 = xor i8 %86, 66
  store i8 %87, ptr @"??_C@_0BG@BOEPDAFA@Invalid?5license?5key?$CB?6?$AA@", align 1
  %88 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@BOEPDAFA@Invalid?5license?5key?$CB?6?$AA@", i64 1), align 1
  %89 = xor i8 %88, 66
  store i8 %89, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@BOEPDAFA@Invalid?5license?5key?$CB?6?$AA@", i64 1), align 1
  %90 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@BOEPDAFA@Invalid?5license?5key?$CB?6?$AA@", i64 2), align 1
  %91 = xor i8 %90, 66
  store i8 %91, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@BOEPDAFA@Invalid?5license?5key?$CB?6?$AA@", i64 2), align 1
  %92 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@BOEPDAFA@Invalid?5license?5key?$CB?6?$AA@", i64 3), align 1
  %93 = xor i8 %92, 66
  store i8 %93, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@BOEPDAFA@Invalid?5license?5key?$CB?6?$AA@", i64 3), align 1
  %94 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@BOEPDAFA@Invalid?5license?5key?$CB?6?$AA@", i64 4), align 1
  %95 = xor i8 %94, 66
  store i8 %95, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@BOEPDAFA@Invalid?5license?5key?$CB?6?$AA@", i64 4), align 1
  %96 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@BOEPDAFA@Invalid?5license?5key?$CB?6?$AA@", i64 5), align 1
  %97 = xor i8 %96, 66
  store i8 %97, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@BOEPDAFA@Invalid?5license?5key?$CB?6?$AA@", i64 5), align 1
  %98 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@BOEPDAFA@Invalid?5license?5key?$CB?6?$AA@", i64 6), align 1
  %99 = xor i8 %98, 66
  store i8 %99, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@BOEPDAFA@Invalid?5license?5key?$CB?6?$AA@", i64 6), align 1
  %100 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@BOEPDAFA@Invalid?5license?5key?$CB?6?$AA@", i64 7), align 1
  %101 = xor i8 %100, 66
  store i8 %101, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@BOEPDAFA@Invalid?5license?5key?$CB?6?$AA@", i64 7), align 1
  %102 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@BOEPDAFA@Invalid?5license?5key?$CB?6?$AA@", i64 8), align 1
  %103 = xor i8 %102, 66
  store i8 %103, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@BOEPDAFA@Invalid?5license?5key?$CB?6?$AA@", i64 8), align 1
  %104 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@BOEPDAFA@Invalid?5license?5key?$CB?6?$AA@", i64 9), align 1
  %105 = xor i8 %104, 66
  store i8 %105, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@BOEPDAFA@Invalid?5license?5key?$CB?6?$AA@", i64 9), align 1
  %106 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@BOEPDAFA@Invalid?5license?5key?$CB?6?$AA@", i64 10), align 1
  %107 = xor i8 %106, 66
  store i8 %107, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@BOEPDAFA@Invalid?5license?5key?$CB?6?$AA@", i64 10), align 1
  %108 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@BOEPDAFA@Invalid?5license?5key?$CB?6?$AA@", i64 11), align 1
  %109 = xor i8 %108, 66
  store i8 %109, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@BOEPDAFA@Invalid?5license?5key?$CB?6?$AA@", i64 11), align 1
  %110 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@BOEPDAFA@Invalid?5license?5key?$CB?6?$AA@", i64 12), align 1
  %111 = xor i8 %110, 66
  store i8 %111, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@BOEPDAFA@Invalid?5license?5key?$CB?6?$AA@", i64 12), align 1
  %112 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@BOEPDAFA@Invalid?5license?5key?$CB?6?$AA@", i64 13), align 1
  %113 = xor i8 %112, 66
  store i8 %113, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@BOEPDAFA@Invalid?5license?5key?$CB?6?$AA@", i64 13), align 1
  %114 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@BOEPDAFA@Invalid?5license?5key?$CB?6?$AA@", i64 14), align 1
  %115 = xor i8 %114, 66
  store i8 %115, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@BOEPDAFA@Invalid?5license?5key?$CB?6?$AA@", i64 14), align 1
  %116 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@BOEPDAFA@Invalid?5license?5key?$CB?6?$AA@", i64 15), align 1
  %117 = xor i8 %116, 66
  store i8 %117, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@BOEPDAFA@Invalid?5license?5key?$CB?6?$AA@", i64 15), align 1
  %118 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@BOEPDAFA@Invalid?5license?5key?$CB?6?$AA@", i64 16), align 1
  %119 = xor i8 %118, 66
  store i8 %119, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@BOEPDAFA@Invalid?5license?5key?$CB?6?$AA@", i64 16), align 1
  %120 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@BOEPDAFA@Invalid?5license?5key?$CB?6?$AA@", i64 17), align 1
  %121 = xor i8 %120, 66
  store i8 %121, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@BOEPDAFA@Invalid?5license?5key?$CB?6?$AA@", i64 17), align 1
  %122 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@BOEPDAFA@Invalid?5license?5key?$CB?6?$AA@", i64 18), align 1
  %123 = xor i8 %122, 66
  store i8 %123, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@BOEPDAFA@Invalid?5license?5key?$CB?6?$AA@", i64 18), align 1
  %124 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@BOEPDAFA@Invalid?5license?5key?$CB?6?$AA@", i64 19), align 1
  %125 = xor i8 %124, 66
  store i8 %125, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@BOEPDAFA@Invalid?5license?5key?$CB?6?$AA@", i64 19), align 1
  %126 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@BOEPDAFA@Invalid?5license?5key?$CB?6?$AA@", i64 20), align 1
  %127 = xor i8 %126, 66
  store i8 %127, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@BOEPDAFA@Invalid?5license?5key?$CB?6?$AA@", i64 20), align 1
  %128 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@BOEPDAFA@Invalid?5license?5key?$CB?6?$AA@", i64 21), align 1
  %129 = xor i8 %128, 66
  store i8 %129, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@BOEPDAFA@Invalid?5license?5key?$CB?6?$AA@", i64 21), align 1
  %130 = load i8, ptr @"??_C@_0CB@NDEEDFKP@License?5validated?5successfully?4?6@", align 1
  %131 = xor i8 %130, 66
  store i8 %131, ptr @"??_C@_0CB@NDEEDFKP@License?5validated?5successfully?4?6@", align 1
  %132 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0CB@NDEEDFKP@License?5validated?5successfully?4?6@", i64 1), align 1
  %133 = xor i8 %132, 66
  store i8 %133, ptr getelementptr inbounds (i8, ptr @"??_C@_0CB@NDEEDFKP@License?5validated?5successfully?4?6@", i64 1), align 1
  %134 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0CB@NDEEDFKP@License?5validated?5successfully?4?6@", i64 2), align 1
  %135 = xor i8 %134, 66
  store i8 %135, ptr getelementptr inbounds (i8, ptr @"??_C@_0CB@NDEEDFKP@License?5validated?5successfully?4?6@", i64 2), align 1
  %136 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0CB@NDEEDFKP@License?5validated?5successfully?4?6@", i64 3), align 1
  %137 = xor i8 %136, 66
  store i8 %137, ptr getelementptr inbounds (i8, ptr @"??_C@_0CB@NDEEDFKP@License?5validated?5successfully?4?6@", i64 3), align 1
  %138 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0CB@NDEEDFKP@License?5validated?5successfully?4?6@", i64 4), align 1
  %139 = xor i8 %138, 66
  store i8 %139, ptr getelementptr inbounds (i8, ptr @"??_C@_0CB@NDEEDFKP@License?5validated?5successfully?4?6@", i64 4), align 1
  %140 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0CB@NDEEDFKP@License?5validated?5successfully?4?6@", i64 5), align 1
  %141 = xor i8 %140, 66
  store i8 %141, ptr getelementptr inbounds (i8, ptr @"??_C@_0CB@NDEEDFKP@License?5validated?5successfully?4?6@", i64 5), align 1
  %142 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0CB@NDEEDFKP@License?5validated?5successfully?4?6@", i64 6), align 1
  %143 = xor i8 %142, 66
  store i8 %143, ptr getelementptr inbounds (i8, ptr @"??_C@_0CB@NDEEDFKP@License?5validated?5successfully?4?6@", i64 6), align 1
  %144 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0CB@NDEEDFKP@License?5validated?5successfully?4?6@", i64 7), align 1
  %145 = xor i8 %144, 66
  store i8 %145, ptr getelementptr inbounds (i8, ptr @"??_C@_0CB@NDEEDFKP@License?5validated?5successfully?4?6@", i64 7), align 1
  %146 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0CB@NDEEDFKP@License?5validated?5successfully?4?6@", i64 8), align 1
  %147 = xor i8 %146, 66
  store i8 %147, ptr getelementptr inbounds (i8, ptr @"??_C@_0CB@NDEEDFKP@License?5validated?5successfully?4?6@", i64 8), align 1
  %148 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0CB@NDEEDFKP@License?5validated?5successfully?4?6@", i64 9), align 1
  %149 = xor i8 %148, 66
  store i8 %149, ptr getelementptr inbounds (i8, ptr @"??_C@_0CB@NDEEDFKP@License?5validated?5successfully?4?6@", i64 9), align 1
  %150 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0CB@NDEEDFKP@License?5validated?5successfully?4?6@", i64 10), align 1
  %151 = xor i8 %150, 66
  store i8 %151, ptr getelementptr inbounds (i8, ptr @"??_C@_0CB@NDEEDFKP@License?5validated?5successfully?4?6@", i64 10), align 1
  %152 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0CB@NDEEDFKP@License?5validated?5successfully?4?6@", i64 11), align 1
  %153 = xor i8 %152, 66
  store i8 %153, ptr getelementptr inbounds (i8, ptr @"??_C@_0CB@NDEEDFKP@License?5validated?5successfully?4?6@", i64 11), align 1
  %154 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0CB@NDEEDFKP@License?5validated?5successfully?4?6@", i64 12), align 1
  %155 = xor i8 %154, 66
  store i8 %155, ptr getelementptr inbounds (i8, ptr @"??_C@_0CB@NDEEDFKP@License?5validated?5successfully?4?6@", i64 12), align 1
  %156 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0CB@NDEEDFKP@License?5validated?5successfully?4?6@", i64 13), align 1
  %157 = xor i8 %156, 66
  store i8 %157, ptr getelementptr inbounds (i8, ptr @"??_C@_0CB@NDEEDFKP@License?5validated?5successfully?4?6@", i64 13), align 1
  %158 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0CB@NDEEDFKP@License?5validated?5successfully?4?6@", i64 14), align 1
  %159 = xor i8 %158, 66
  store i8 %159, ptr getelementptr inbounds (i8, ptr @"??_C@_0CB@NDEEDFKP@License?5validated?5successfully?4?6@", i64 14), align 1
  %160 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0CB@NDEEDFKP@License?5validated?5successfully?4?6@", i64 15), align 1
  %161 = xor i8 %160, 66
  store i8 %161, ptr getelementptr inbounds (i8, ptr @"??_C@_0CB@NDEEDFKP@License?5validated?5successfully?4?6@", i64 15), align 1
  %162 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0CB@NDEEDFKP@License?5validated?5successfully?4?6@", i64 16), align 1
  %163 = xor i8 %162, 66
  store i8 %163, ptr getelementptr inbounds (i8, ptr @"??_C@_0CB@NDEEDFKP@License?5validated?5successfully?4?6@", i64 16), align 1
  %164 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0CB@NDEEDFKP@License?5validated?5successfully?4?6@", i64 17), align 1
  %165 = xor i8 %164, 66
  store i8 %165, ptr getelementptr inbounds (i8, ptr @"??_C@_0CB@NDEEDFKP@License?5validated?5successfully?4?6@", i64 17), align 1
  %166 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0CB@NDEEDFKP@License?5validated?5successfully?4?6@", i64 18), align 1
  %167 = xor i8 %166, 66
  store i8 %167, ptr getelementptr inbounds (i8, ptr @"??_C@_0CB@NDEEDFKP@License?5validated?5successfully?4?6@", i64 18), align 1
  %168 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0CB@NDEEDFKP@License?5validated?5successfully?4?6@", i64 19), align 1
  %169 = xor i8 %168, 66
  store i8 %169, ptr getelementptr inbounds (i8, ptr @"??_C@_0CB@NDEEDFKP@License?5validated?5successfully?4?6@", i64 19), align 1
  %170 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0CB@NDEEDFKP@License?5validated?5successfully?4?6@", i64 20), align 1
  %171 = xor i8 %170, 66
  store i8 %171, ptr getelementptr inbounds (i8, ptr @"??_C@_0CB@NDEEDFKP@License?5validated?5successfully?4?6@", i64 20), align 1
  %172 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0CB@NDEEDFKP@License?5validated?5successfully?4?6@", i64 21), align 1
  %173 = xor i8 %172, 66
  store i8 %173, ptr getelementptr inbounds (i8, ptr @"??_C@_0CB@NDEEDFKP@License?5validated?5successfully?4?6@", i64 21), align 1
  %174 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0CB@NDEEDFKP@License?5validated?5successfully?4?6@", i64 22), align 1
  %175 = xor i8 %174, 66
  store i8 %175, ptr getelementptr inbounds (i8, ptr @"??_C@_0CB@NDEEDFKP@License?5validated?5successfully?4?6@", i64 22), align 1
  %176 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0CB@NDEEDFKP@License?5validated?5successfully?4?6@", i64 23), align 1
  %177 = xor i8 %176, 66
  store i8 %177, ptr getelementptr inbounds (i8, ptr @"??_C@_0CB@NDEEDFKP@License?5validated?5successfully?4?6@", i64 23), align 1
  %178 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0CB@NDEEDFKP@License?5validated?5successfully?4?6@", i64 24), align 1
  %179 = xor i8 %178, 66
  store i8 %179, ptr getelementptr inbounds (i8, ptr @"??_C@_0CB@NDEEDFKP@License?5validated?5successfully?4?6@", i64 24), align 1
  %180 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0CB@NDEEDFKP@License?5validated?5successfully?4?6@", i64 25), align 1
  %181 = xor i8 %180, 66
  store i8 %181, ptr getelementptr inbounds (i8, ptr @"??_C@_0CB@NDEEDFKP@License?5validated?5successfully?4?6@", i64 25), align 1
  %182 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0CB@NDEEDFKP@License?5validated?5successfully?4?6@", i64 26), align 1
  %183 = xor i8 %182, 66
  store i8 %183, ptr getelementptr inbounds (i8, ptr @"??_C@_0CB@NDEEDFKP@License?5validated?5successfully?4?6@", i64 26), align 1
  %184 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0CB@NDEEDFKP@License?5validated?5successfully?4?6@", i64 27), align 1
  %185 = xor i8 %184, 66
  store i8 %185, ptr getelementptr inbounds (i8, ptr @"??_C@_0CB@NDEEDFKP@License?5validated?5successfully?4?6@", i64 27), align 1
  %186 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0CB@NDEEDFKP@License?5validated?5successfully?4?6@", i64 28), align 1
  %187 = xor i8 %186, 66
  store i8 %187, ptr getelementptr inbounds (i8, ptr @"??_C@_0CB@NDEEDFKP@License?5validated?5successfully?4?6@", i64 28), align 1
  %188 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0CB@NDEEDFKP@License?5validated?5successfully?4?6@", i64 29), align 1
  %189 = xor i8 %188, 66
  store i8 %189, ptr getelementptr inbounds (i8, ptr @"??_C@_0CB@NDEEDFKP@License?5validated?5successfully?4?6@", i64 29), align 1
  %190 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0CB@NDEEDFKP@License?5validated?5successfully?4?6@", i64 30), align 1
  %191 = xor i8 %190, 66
  store i8 %191, ptr getelementptr inbounds (i8, ptr @"??_C@_0CB@NDEEDFKP@License?5validated?5successfully?4?6@", i64 30), align 1
  %192 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0CB@NDEEDFKP@License?5validated?5successfully?4?6@", i64 31), align 1
  %193 = xor i8 %192, 66
  store i8 %193, ptr getelementptr inbounds (i8, ptr @"??_C@_0CB@NDEEDFKP@License?5validated?5successfully?4?6@", i64 31), align 1
  %194 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0CB@NDEEDFKP@License?5validated?5successfully?4?6@", i64 32), align 1
  %195 = xor i8 %194, 66
  store i8 %195, ptr getelementptr inbounds (i8, ptr @"??_C@_0CB@NDEEDFKP@License?5validated?5successfully?4?6@", i64 32), align 1
  %196 = load i8, ptr @"??_C@_0BG@PLBFEBCH@Processing?3?5?$CFd?5?9?$DO?5?$CFd?6?$AA@", align 1
  %197 = xor i8 %196, 66
  store i8 %197, ptr @"??_C@_0BG@PLBFEBCH@Processing?3?5?$CFd?5?9?$DO?5?$CFd?6?$AA@", align 1
  %198 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@PLBFEBCH@Processing?3?5?$CFd?5?9?$DO?5?$CFd?6?$AA@", i64 1), align 1
  %199 = xor i8 %198, 66
  store i8 %199, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@PLBFEBCH@Processing?3?5?$CFd?5?9?$DO?5?$CFd?6?$AA@", i64 1), align 1
  %200 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@PLBFEBCH@Processing?3?5?$CFd?5?9?$DO?5?$CFd?6?$AA@", i64 2), align 1
  %201 = xor i8 %200, 66
  store i8 %201, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@PLBFEBCH@Processing?3?5?$CFd?5?9?$DO?5?$CFd?6?$AA@", i64 2), align 1
  %202 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@PLBFEBCH@Processing?3?5?$CFd?5?9?$DO?5?$CFd?6?$AA@", i64 3), align 1
  %203 = xor i8 %202, 66
  store i8 %203, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@PLBFEBCH@Processing?3?5?$CFd?5?9?$DO?5?$CFd?6?$AA@", i64 3), align 1
  %204 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@PLBFEBCH@Processing?3?5?$CFd?5?9?$DO?5?$CFd?6?$AA@", i64 4), align 1
  %205 = xor i8 %204, 66
  store i8 %205, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@PLBFEBCH@Processing?3?5?$CFd?5?9?$DO?5?$CFd?6?$AA@", i64 4), align 1
  %206 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@PLBFEBCH@Processing?3?5?$CFd?5?9?$DO?5?$CFd?6?$AA@", i64 5), align 1
  %207 = xor i8 %206, 66
  store i8 %207, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@PLBFEBCH@Processing?3?5?$CFd?5?9?$DO?5?$CFd?6?$AA@", i64 5), align 1
  %208 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@PLBFEBCH@Processing?3?5?$CFd?5?9?$DO?5?$CFd?6?$AA@", i64 6), align 1
  %209 = xor i8 %208, 66
  store i8 %209, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@PLBFEBCH@Processing?3?5?$CFd?5?9?$DO?5?$CFd?6?$AA@", i64 6), align 1
  %210 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@PLBFEBCH@Processing?3?5?$CFd?5?9?$DO?5?$CFd?6?$AA@", i64 7), align 1
  %211 = xor i8 %210, 66
  store i8 %211, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@PLBFEBCH@Processing?3?5?$CFd?5?9?$DO?5?$CFd?6?$AA@", i64 7), align 1
  %212 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@PLBFEBCH@Processing?3?5?$CFd?5?9?$DO?5?$CFd?6?$AA@", i64 8), align 1
  %213 = xor i8 %212, 66
  store i8 %213, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@PLBFEBCH@Processing?3?5?$CFd?5?9?$DO?5?$CFd?6?$AA@", i64 8), align 1
  %214 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@PLBFEBCH@Processing?3?5?$CFd?5?9?$DO?5?$CFd?6?$AA@", i64 9), align 1
  %215 = xor i8 %214, 66
  store i8 %215, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@PLBFEBCH@Processing?3?5?$CFd?5?9?$DO?5?$CFd?6?$AA@", i64 9), align 1
  %216 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@PLBFEBCH@Processing?3?5?$CFd?5?9?$DO?5?$CFd?6?$AA@", i64 10), align 1
  %217 = xor i8 %216, 66
  store i8 %217, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@PLBFEBCH@Processing?3?5?$CFd?5?9?$DO?5?$CFd?6?$AA@", i64 10), align 1
  %218 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@PLBFEBCH@Processing?3?5?$CFd?5?9?$DO?5?$CFd?6?$AA@", i64 11), align 1
  %219 = xor i8 %218, 66
  store i8 %219, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@PLBFEBCH@Processing?3?5?$CFd?5?9?$DO?5?$CFd?6?$AA@", i64 11), align 1
  %220 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@PLBFEBCH@Processing?3?5?$CFd?5?9?$DO?5?$CFd?6?$AA@", i64 12), align 1
  %221 = xor i8 %220, 66
  store i8 %221, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@PLBFEBCH@Processing?3?5?$CFd?5?9?$DO?5?$CFd?6?$AA@", i64 12), align 1
  %222 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@PLBFEBCH@Processing?3?5?$CFd?5?9?$DO?5?$CFd?6?$AA@", i64 13), align 1
  %223 = xor i8 %222, 66
  store i8 %223, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@PLBFEBCH@Processing?3?5?$CFd?5?9?$DO?5?$CFd?6?$AA@", i64 13), align 1
  %224 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@PLBFEBCH@Processing?3?5?$CFd?5?9?$DO?5?$CFd?6?$AA@", i64 14), align 1
  %225 = xor i8 %224, 66
  store i8 %225, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@PLBFEBCH@Processing?3?5?$CFd?5?9?$DO?5?$CFd?6?$AA@", i64 14), align 1
  %226 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@PLBFEBCH@Processing?3?5?$CFd?5?9?$DO?5?$CFd?6?$AA@", i64 15), align 1
  %227 = xor i8 %226, 66
  store i8 %227, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@PLBFEBCH@Processing?3?5?$CFd?5?9?$DO?5?$CFd?6?$AA@", i64 15), align 1
  %228 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@PLBFEBCH@Processing?3?5?$CFd?5?9?$DO?5?$CFd?6?$AA@", i64 16), align 1
  %229 = xor i8 %228, 66
  store i8 %229, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@PLBFEBCH@Processing?3?5?$CFd?5?9?$DO?5?$CFd?6?$AA@", i64 16), align 1
  %230 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@PLBFEBCH@Processing?3?5?$CFd?5?9?$DO?5?$CFd?6?$AA@", i64 17), align 1
  %231 = xor i8 %230, 66
  store i8 %231, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@PLBFEBCH@Processing?3?5?$CFd?5?9?$DO?5?$CFd?6?$AA@", i64 17), align 1
  %232 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@PLBFEBCH@Processing?3?5?$CFd?5?9?$DO?5?$CFd?6?$AA@", i64 18), align 1
  %233 = xor i8 %232, 66
  store i8 %233, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@PLBFEBCH@Processing?3?5?$CFd?5?9?$DO?5?$CFd?6?$AA@", i64 18), align 1
  %234 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@PLBFEBCH@Processing?3?5?$CFd?5?9?$DO?5?$CFd?6?$AA@", i64 19), align 1
  %235 = xor i8 %234, 66
  store i8 %235, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@PLBFEBCH@Processing?3?5?$CFd?5?9?$DO?5?$CFd?6?$AA@", i64 19), align 1
  %236 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@PLBFEBCH@Processing?3?5?$CFd?5?9?$DO?5?$CFd?6?$AA@", i64 20), align 1
  %237 = xor i8 %236, 66
  store i8 %237, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@PLBFEBCH@Processing?3?5?$CFd?5?9?$DO?5?$CFd?6?$AA@", i64 20), align 1
  %238 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@PLBFEBCH@Processing?3?5?$CFd?5?9?$DO?5?$CFd?6?$AA@", i64 21), align 1
  %239 = xor i8 %238, 66
  store i8 %239, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@PLBFEBCH@Processing?3?5?$CFd?5?9?$DO?5?$CFd?6?$AA@", i64 21), align 1
  %240 = load i8, ptr @"??_C@_0BG@INDCFGHJ@Processing?5complete?4?6?$AA@", align 1
  %241 = xor i8 %240, 66
  store i8 %241, ptr @"??_C@_0BG@INDCFGHJ@Processing?5complete?4?6?$AA@", align 1
  %242 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@INDCFGHJ@Processing?5complete?4?6?$AA@", i64 1), align 1
  %243 = xor i8 %242, 66
  store i8 %243, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@INDCFGHJ@Processing?5complete?4?6?$AA@", i64 1), align 1
  %244 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@INDCFGHJ@Processing?5complete?4?6?$AA@", i64 2), align 1
  %245 = xor i8 %244, 66
  store i8 %245, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@INDCFGHJ@Processing?5complete?4?6?$AA@", i64 2), align 1
  %246 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@INDCFGHJ@Processing?5complete?4?6?$AA@", i64 3), align 1
  %247 = xor i8 %246, 66
  store i8 %247, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@INDCFGHJ@Processing?5complete?4?6?$AA@", i64 3), align 1
  %248 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@INDCFGHJ@Processing?5complete?4?6?$AA@", i64 4), align 1
  %249 = xor i8 %248, 66
  store i8 %249, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@INDCFGHJ@Processing?5complete?4?6?$AA@", i64 4), align 1
  %250 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@INDCFGHJ@Processing?5complete?4?6?$AA@", i64 5), align 1
  %251 = xor i8 %250, 66
  store i8 %251, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@INDCFGHJ@Processing?5complete?4?6?$AA@", i64 5), align 1
  %252 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@INDCFGHJ@Processing?5complete?4?6?$AA@", i64 6), align 1
  %253 = xor i8 %252, 66
  store i8 %253, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@INDCFGHJ@Processing?5complete?4?6?$AA@", i64 6), align 1
  %254 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@INDCFGHJ@Processing?5complete?4?6?$AA@", i64 7), align 1
  %255 = xor i8 %254, 66
  store i8 %255, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@INDCFGHJ@Processing?5complete?4?6?$AA@", i64 7), align 1
  %256 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@INDCFGHJ@Processing?5complete?4?6?$AA@", i64 8), align 1
  %257 = xor i8 %256, 66
  store i8 %257, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@INDCFGHJ@Processing?5complete?4?6?$AA@", i64 8), align 1
  %258 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@INDCFGHJ@Processing?5complete?4?6?$AA@", i64 9), align 1
  %259 = xor i8 %258, 66
  store i8 %259, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@INDCFGHJ@Processing?5complete?4?6?$AA@", i64 9), align 1
  %260 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@INDCFGHJ@Processing?5complete?4?6?$AA@", i64 10), align 1
  %261 = xor i8 %260, 66
  store i8 %261, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@INDCFGHJ@Processing?5complete?4?6?$AA@", i64 10), align 1
  %262 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@INDCFGHJ@Processing?5complete?4?6?$AA@", i64 11), align 1
  %263 = xor i8 %262, 66
  store i8 %263, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@INDCFGHJ@Processing?5complete?4?6?$AA@", i64 11), align 1
  %264 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@INDCFGHJ@Processing?5complete?4?6?$AA@", i64 12), align 1
  %265 = xor i8 %264, 66
  store i8 %265, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@INDCFGHJ@Processing?5complete?4?6?$AA@", i64 12), align 1
  %266 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@INDCFGHJ@Processing?5complete?4?6?$AA@", i64 13), align 1
  %267 = xor i8 %266, 66
  store i8 %267, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@INDCFGHJ@Processing?5complete?4?6?$AA@", i64 13), align 1
  %268 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@INDCFGHJ@Processing?5complete?4?6?$AA@", i64 14), align 1
  %269 = xor i8 %268, 66
  store i8 %269, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@INDCFGHJ@Processing?5complete?4?6?$AA@", i64 14), align 1
  %270 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@INDCFGHJ@Processing?5complete?4?6?$AA@", i64 15), align 1
  %271 = xor i8 %270, 66
  store i8 %271, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@INDCFGHJ@Processing?5complete?4?6?$AA@", i64 15), align 1
  %272 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@INDCFGHJ@Processing?5complete?4?6?$AA@", i64 16), align 1
  %273 = xor i8 %272, 66
  store i8 %273, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@INDCFGHJ@Processing?5complete?4?6?$AA@", i64 16), align 1
  %274 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@INDCFGHJ@Processing?5complete?4?6?$AA@", i64 17), align 1
  %275 = xor i8 %274, 66
  store i8 %275, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@INDCFGHJ@Processing?5complete?4?6?$AA@", i64 17), align 1
  %276 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@INDCFGHJ@Processing?5complete?4?6?$AA@", i64 18), align 1
  %277 = xor i8 %276, 66
  store i8 %277, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@INDCFGHJ@Processing?5complete?4?6?$AA@", i64 18), align 1
  %278 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@INDCFGHJ@Processing?5complete?4?6?$AA@", i64 19), align 1
  %279 = xor i8 %278, 66
  store i8 %279, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@INDCFGHJ@Processing?5complete?4?6?$AA@", i64 19), align 1
  %280 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@INDCFGHJ@Processing?5complete?4?6?$AA@", i64 20), align 1
  %281 = xor i8 %280, 66
  store i8 %281, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@INDCFGHJ@Processing?5complete?4?6?$AA@", i64 20), align 1
  %282 = load i8, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@INDCFGHJ@Processing?5complete?4?6?$AA@", i64 21), align 1
  %283 = xor i8 %282, 66
  store i8 %283, ptr getelementptr inbounds (i8, ptr @"??_C@_0BG@INDCFGHJ@Processing?5complete?4?6?$AA@", i64 21), align 1
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
