// NOTE: Assertions have been autogenerated by utils/generate-test-checks.py

// The script is designed to make adding checks to
// a test case fast, it is *not* designed to be authoritative
// about what constitutes a good test! The CHECK should be
// minimized and named to reflect the test intent.

// CHECK: #[[$ATTR_0:.+]] = #ttg.blocked<{sizePerThread = [1, 1], threadsPerWarp = [1, 32], warpsPerCTA = [4, 2], order = [1, 0]}>
// CHECK: #[[$ATTR_1:.+]] = #ttg.nvidia_mma<{versionMajor = 3, versionMinor = 0, warpsPerCTA = [8, 1], instrShape = [16, 256, 16]}>
// CHECK: #[[$ATTR_2:.+]] = #ttg.nvmma_shared<{swizzlingByteWidth = 128, transposed = false, elementBitWidth = 16}>
// CHECK: #[[$ATTR_3:.+]] = #ttg.nvmma_shared<{swizzlingByteWidth = 128, transposed = true, elementBitWidth = 16}>
// CHECK: #[[$ATTR_4:.+]] = #ttg.shared_memory
// To regenerate this test case, run `make golden-samples` in the triton root directory
// RUN: triton-opt %s -tritongpu-pipeline -canonicalize | FileCheck --dump-input-context=50 %s
#nvmma_128 = #ttg.nvmma_shared<{swizzlingByteWidth = 128, transposed = false, elementBitWidth = 16}>

// CHECK-LABEL:   tt.func public @matmul_kernel_descriptor_persistent(
// CHECK-SAME:  %[[VAL_0:.*]]: !tt.ptr<f16> {tt.divisibility = 16 : i32}, %[[VAL_1:.*]]: !tt.ptr<f16> {tt.divisibility = 16 : i32}, %[[VAL_2:.*]]: !tt.ptr<f16> {tt.divisibility = 16 : i32}, %[[VAL_3:.*]]: i32 {tt.divisibility = 16 : i32}, %[[VAL_4:.*]]: i32 {tt.divisibility = 16 : i32}, %[[VAL_5:.*]]: i32 {tt.divisibility = 16 : i32}) {
// CHECK:           %[[VAL_6:.*]] = arith.constant 2 : i64
// CHECK:           %[[VAL_7:.*]] = arith.constant 3 : i32
// CHECK:           %[[VAL_8:.*]] = arith.constant false
// CHECK:           %[[VAL_9:.*]] = arith.constant 1 : i32
// CHECK:           %[[VAL_10:.*]] = arith.constant 132 : i32
// CHECK:           %[[VAL_11:.*]] = arith.constant -1 : i32
// CHECK:           %[[VAL_12:.*]] = arith.constant 0 : i32
// CHECK:           %[[VAL_13:.*]] = arith.constant 8 : i32
// CHECK:           %[[VAL_14:.*]] = arith.constant 128 : i32
// CHECK:           %[[VAL_15:.*]] = arith.constant 256 : i32
// CHECK:           %[[VAL_16:.*]] = arith.constant 64 : i32
// CHECK:           %[[VAL_17:.*]] = arith.constant 1 : i64
// CHECK:           %[[VAL_18:.*]] = arith.constant 127 : i32
// CHECK:           %[[VAL_19:.*]] = arith.constant 255 : i32
// CHECK:           %[[VAL_20:.*]] = arith.constant 63 : i32
// CHECK:           %[[VAL_21:.*]] = arith.constant dense<0.000000e+00> : tensor<128x256xf32, #[[$ATTR_1]]>
// CHECK:           %[[VAL_22:.*]] = tt.get_program_id x : i32
// CHECK:           %[[VAL_23:.*]] = arith.addi %[[VAL_3]], %[[VAL_18]] : i32
// CHECK:           %[[VAL_24:.*]] = arith.divsi %[[VAL_23]], %[[VAL_14]] : i32
// CHECK:           %[[VAL_25:.*]] = arith.addi %[[VAL_4]], %[[VAL_19]] : i32
// CHECK:           %[[VAL_26:.*]] = arith.divsi %[[VAL_25]], %[[VAL_15]] : i32
// CHECK:           %[[VAL_27:.*]] = arith.addi %[[VAL_5]], %[[VAL_20]] : i32
// CHECK:           %[[VAL_28:.*]] = arith.divsi %[[VAL_27]], %[[VAL_16]] : i32
// CHECK:           %[[VAL_29:.*]] = arith.muli %[[VAL_24]], %[[VAL_26]] : i32
// CHECK:           %[[VAL_30:.*]] = arith.extsi %[[VAL_5]] : i32 to i64
// CHECK:           %[[VAL_31:.*]] = tt.make_tensor_descriptor %[[VAL_0]], {{\[}}%[[VAL_3]], %[[VAL_5]]], {{\[}}%[[VAL_30]], %[[VAL_17]]] : <f16>, <tensor<128x64xf16, #[[$ATTR_2]]>>
// CHECK:           %[[VAL_32:.*]] = tt.make_tensor_descriptor %[[VAL_1]], {{\[}}%[[VAL_4]], %[[VAL_5]]], {{\[}}%[[VAL_30]], %[[VAL_17]]] : <f16>, <tensor<256x64xf16, #[[$ATTR_2]]>>
// CHECK:           %[[VAL_33:.*]] = arith.extsi %[[VAL_4]] : i32 to i64
// CHECK:           %[[VAL_34:.*]] = tt.make_tensor_descriptor %[[VAL_2]], {{\[}}%[[VAL_3]], %[[VAL_4]]], {{\[}}%[[VAL_33]], %[[VAL_17]]] : <f16>, <tensor<128x256xf16, #[[$ATTR_2]]>>
// CHECK:           %[[VAL_35:.*]] = arith.divsi %[[VAL_29]], %[[VAL_10]] : i32
// CHECK:           %[[VAL_36:.*]] = arith.remsi %[[VAL_29]], %[[VAL_10]] : i32
// CHECK:           %[[VAL_37:.*]] = arith.cmpi slt, %[[VAL_22]], %[[VAL_36]] : i32
// CHECK:           %[[VAL_38:.*]] = scf.if %[[VAL_37]] -> (i32) {
// CHECK:             %[[VAL_39:.*]] = arith.addi %[[VAL_35]], %[[VAL_9]] : i32
// CHECK:             scf.yield %[[VAL_39]] : i32
// CHECK:           } else {
// CHECK:             scf.yield %[[VAL_35]] : i32
// CHECK:           }
// CHECK:           %[[VAL_40:.*]] = arith.subi %[[VAL_22]], %[[VAL_10]] : i32
// CHECK:           %[[VAL_41:.*]] = arith.muli %[[VAL_26]], %[[VAL_13]] : i32
// CHECK:           %[[VAL_42:.*]] = tt.elementwise_inline_asm "mov.b32 $0, 0;" {constraints = "=r", packed_element = 1 : i32, pure = true} -> i32
// CHECK:           %[[VAL_43:.*]] = arith.muli %[[VAL_28]], %[[VAL_38]] : i32
// CHECK:           %[[VAL_44:.*]] = arith.subi %[[VAL_28]], %[[VAL_9]] : i32
// CHECK:           %[[VAL_45:.*]] = ttg.local_alloc : () -> !ttg.memdesc<128x256xf16, #[[$ATTR_2]], #[[$ATTR_4]], mutable>
// CHECK:           %[[VAL_46:.*]] = ttg.global_scratch_alloc {alignment = 128 : i32, nbytes = 384 : i32} : !tt.ptr<i8>
// CHECK:           %[[VAL_47:.*]] = ttg.global_scratch_alloc {alignment = 128 : i32, nbytes = 384 : i32} : !tt.ptr<i8>
// CHECK:           %[[VAL_48:.*]] = ttg.global_scratch_alloc {alignment = 128 : i32, nbytes = 384 : i32} : !tt.ptr<i8>
// CHECK:           %[[VAL_49:.*]]:13 = scf.for %[[VAL_50:.*]] = %[[VAL_12]] to %[[VAL_43]] step %[[VAL_9]] iter_args(%[[VAL_51:.*]] = %[[VAL_11]], %[[VAL_52:.*]] = %[[VAL_31]], %[[VAL_53:.*]] = %[[VAL_32]], %[[VAL_54:.*]] = %[[VAL_34]], %[[VAL_55:.*]] = %[[VAL_40]], %[[VAL_56:.*]] = %[[VAL_11]], %[[VAL_57:.*]] = %[[VAL_12]], %[[VAL_58:.*]] = %[[VAL_12]], %[[VAL_59:.*]] = %[[VAL_21]], %[[VAL_60:.*]] = %[[VAL_8]], %[[VAL_61:.*]] = %[[VAL_12]], %[[VAL_62:.*]] = %[[VAL_12]], %[[VAL_63:.*]] = %[[VAL_12]]) -> (i32, !tt.tensordesc<tensor<128x64xf16, #[[$ATTR_2]]>>, !tt.tensordesc<tensor<256x64xf16, #[[$ATTR_2]]>>, !tt.tensordesc<tensor<128x256xf16, #[[$ATTR_2]]>>, i32, i32, i32, i32, tensor<128x256xf32, #[[$ATTR_1]]>, i1, i32, i32, i32)  : i32 {
// CHECK:             %[[VAL_64:.*]] = arith.cmpi eq, %[[VAL_51]], %[[VAL_44]] : i32
// CHECK:             %[[VAL_65:.*]] = arith.addi %[[VAL_51]], %[[VAL_9]] : i32
// CHECK:             %[[VAL_66:.*]] = arith.select %[[VAL_64]], %[[VAL_12]], %[[VAL_65]] : i32
// CHECK:             %[[VAL_67:.*]] = arith.cmpi eq, %[[VAL_66]], %[[VAL_12]] : i32
// CHECK:             %[[VAL_68:.*]]:10 = scf.if %[[VAL_67]] -> (!tt.tensordesc<tensor<128x64xf16, #[[$ATTR_2]]>>, !tt.tensordesc<tensor<256x64xf16, #[[$ATTR_2]]>>, !tt.tensordesc<tensor<128x256xf16, #[[$ATTR_2]]>>, i32, i32, i32, i32, i32, i32, i32) {
// CHECK:               %[[VAL_69:.*]] = arith.addi %[[VAL_56]], %[[VAL_9]] : i32
// CHECK:               %[[VAL_70:.*]] = arith.cmpi eq, %[[VAL_69]], %[[VAL_9]] : i32
// CHECK:               %[[VAL_71:.*]] = arith.select %[[VAL_70]], %[[VAL_12]], %[[VAL_69]] : i32
// CHECK:               %[[VAL_72:.*]]:6 = scf.if %[[VAL_70]] -> (!tt.tensordesc<tensor<128x64xf16, #[[$ATTR_2]]>>, !tt.tensordesc<tensor<256x64xf16, #[[$ATTR_2]]>>, !tt.tensordesc<tensor<128x256xf16, #[[$ATTR_2]]>>, i32, i32, i32) {
// CHECK:                 %[[VAL_73:.*]] = tt.addptr %[[VAL_0]], %[[VAL_42]] : !tt.ptr<f16>, i32
// CHECK:                 %[[VAL_74:.*]] = arith.muli %[[VAL_61]], %[[VAL_14]] : i32
// CHECK:                 %[[VAL_75:.*]] = tt.addptr %[[VAL_46]], %[[VAL_74]] : !tt.ptr<i8>, i32
// CHECK:                 %[[VAL_76:.*]] = arith.muli %[[VAL_30]], %[[VAL_6]] : i64
// CHECK:                 ttng.tensormap_create %[[VAL_75]], %[[VAL_73]], {{\[}}%[[VAL_16]], %[[VAL_14]]], {{\[}}%[[VAL_5]], %[[VAL_3]]], {{\[}}%[[VAL_76]]], {{\[}}%[[VAL_9]], %[[VAL_9]]] {elem_type = 6 : i32, fill_mode = 0 : i32, interleave_layout = 0 : i32, swizzle_mode = 3 : i32} : (!tt.ptr<i8>, !tt.ptr<f16>, i32, i32, i32, i32, i64, i32, i32) -> ()
// CHECK:                 ttng.tensormap_fenceproxy_acquire %[[VAL_75]] : !tt.ptr<i8>
// CHECK:                 %[[VAL_77:.*]] = ttng.reinterpret_tensor_descriptor %[[VAL_75]] : !tt.ptr<i8> to !tt.tensordesc<tensor<128x64xf16, #[[$ATTR_2]]>>
// CHECK:                 %[[VAL_78:.*]] = arith.addi %[[VAL_61]], %[[VAL_9]] : i32
// CHECK:                 %[[VAL_79:.*]] = arith.cmpi sge, %[[VAL_78]], %[[VAL_7]] : i32
// CHECK:                 %[[VAL_80:.*]] = arith.select %[[VAL_79]], %[[VAL_12]], %[[VAL_78]] : i32
// CHECK:                 %[[VAL_81:.*]] = tt.addptr %[[VAL_1]], %[[VAL_42]] : !tt.ptr<f16>, i32
// CHECK:                 %[[VAL_82:.*]] = arith.muli %[[VAL_62]], %[[VAL_14]] : i32
// CHECK:                 %[[VAL_83:.*]] = tt.addptr %[[VAL_47]], %[[VAL_82]] : !tt.ptr<i8>, i32
// CHECK:                 %[[VAL_84:.*]] = arith.muli %[[VAL_30]], %[[VAL_6]] : i64
// CHECK:                 ttng.tensormap_create %[[VAL_83]], %[[VAL_81]], {{\[}}%[[VAL_16]], %[[VAL_15]]], {{\[}}%[[VAL_5]], %[[VAL_4]]], {{\[}}%[[VAL_84]]], {{\[}}%[[VAL_9]], %[[VAL_9]]] {elem_type = 6 : i32, fill_mode = 0 : i32, interleave_layout = 0 : i32, swizzle_mode = 3 : i32} : (!tt.ptr<i8>, !tt.ptr<f16>, i32, i32, i32, i32, i64, i32, i32) -> ()
// CHECK:                 ttng.tensormap_fenceproxy_acquire %[[VAL_83]] : !tt.ptr<i8>
// CHECK:                 %[[VAL_85:.*]] = ttng.reinterpret_tensor_descriptor %[[VAL_83]] : !tt.ptr<i8> to !tt.tensordesc<tensor<256x64xf16, #[[$ATTR_2]]>>
// CHECK:                 %[[VAL_86:.*]] = arith.addi %[[VAL_62]], %[[VAL_9]] : i32
// CHECK:                 %[[VAL_87:.*]] = arith.cmpi sge, %[[VAL_86]], %[[VAL_7]] : i32
// CHECK:                 %[[VAL_88:.*]] = arith.select %[[VAL_87]], %[[VAL_12]], %[[VAL_86]] : i32
// CHECK:                 %[[VAL_89:.*]] = tt.addptr %[[VAL_2]], %[[VAL_42]] : !tt.ptr<f16>, i32
// CHECK:                 %[[VAL_90:.*]] = arith.muli %[[VAL_63]], %[[VAL_14]] : i32
// CHECK:                 %[[VAL_91:.*]] = tt.addptr %[[VAL_48]], %[[VAL_90]] : !tt.ptr<i8>, i32
// CHECK:                 %[[VAL_92:.*]] = arith.muli %[[VAL_33]], %[[VAL_6]] : i64
// CHECK:                 ttng.tensormap_create %[[VAL_91]], %[[VAL_89]], {{\[}}%[[VAL_16]], %[[VAL_14]]], {{\[}}%[[VAL_4]], %[[VAL_3]]], {{\[}}%[[VAL_92]]], {{\[}}%[[VAL_9]], %[[VAL_9]]] {elem_type = 6 : i32, fill_mode = 0 : i32, interleave_layout = 0 : i32, swizzle_mode = 3 : i32} : (!tt.ptr<i8>, !tt.ptr<f16>, i32, i32, i32, i32, i64, i32, i32) -> ()
// CHECK:                 ttng.tensormap_fenceproxy_acquire %[[VAL_91]] : !tt.ptr<i8>
// CHECK:                 %[[VAL_93:.*]] = ttng.reinterpret_tensor_descriptor %[[VAL_91]] : !tt.ptr<i8> to !tt.tensordesc<tensor<128x256xf16, #[[$ATTR_2]]>>
// CHECK:                 %[[VAL_94:.*]] = arith.addi %[[VAL_63]], %[[VAL_9]] : i32
// CHECK:                 %[[VAL_95:.*]] = arith.cmpi sge, %[[VAL_94]], %[[VAL_7]] : i32
// CHECK:                 %[[VAL_96:.*]] = arith.select %[[VAL_95]], %[[VAL_12]], %[[VAL_94]] : i32
// CHECK:                 scf.yield %[[VAL_77]], %[[VAL_85]], %[[VAL_93]], %[[VAL_80]], %[[VAL_88]], %[[VAL_96]] : !tt.tensordesc<tensor<128x64xf16, #[[$ATTR_2]]>>, !tt.tensordesc<tensor<256x64xf16, #[[$ATTR_2]]>>, !tt.tensordesc<tensor<128x256xf16, #[[$ATTR_2]]>>, i32, i32, i32
// CHECK:               } else {
// CHECK:                 scf.yield %[[VAL_52]], %[[VAL_53]], %[[VAL_54]], %[[VAL_61]], %[[VAL_62]], %[[VAL_63]] : !tt.tensordesc<tensor<128x64xf16, #[[$ATTR_2]]>>, !tt.tensordesc<tensor<256x64xf16, #[[$ATTR_2]]>>, !tt.tensordesc<tensor<128x256xf16, #[[$ATTR_2]]>>, i32, i32, i32
// CHECK:               }
// CHECK:               %[[VAL_97:.*]] = arith.addi %[[VAL_55]], %[[VAL_10]] : i32
// CHECK:               %[[VAL_98:.*]] = arith.divsi %[[VAL_97]], %[[VAL_41]] : i32
// CHECK:               %[[VAL_99:.*]] = arith.muli %[[VAL_98]], %[[VAL_13]] : i32
// CHECK:               %[[VAL_100:.*]] = arith.subi %[[VAL_24]], %[[VAL_99]] : i32
// CHECK:               %[[VAL_101:.*]] = arith.minsi %[[VAL_100]], %[[VAL_13]] : i32
// CHECK:               %[[VAL_102:.*]] = arith.remsi %[[VAL_97]], %[[VAL_101]] : i32
// CHECK:               %[[VAL_103:.*]] = arith.addi %[[VAL_99]], %[[VAL_102]] : i32
// CHECK:               %[[VAL_104:.*]] = arith.remsi %[[VAL_97]], %[[VAL_41]] : i32
// CHECK:               %[[VAL_105:.*]] = arith.divsi %[[VAL_104]], %[[VAL_101]] : i32
// CHECK:               %[[VAL_106:.*]] = arith.muli %[[VAL_103]], %[[VAL_14]] : i32
// CHECK:               %[[VAL_107:.*]] = arith.muli %[[VAL_105]], %[[VAL_15]] : i32
// CHECK:               scf.yield %[[VAL_108:.*]]#0, %[[VAL_108]]#1, %[[VAL_108]]#2, %[[VAL_97]], %[[VAL_71]], %[[VAL_106]], %[[VAL_107]], %[[VAL_108]]#3, %[[VAL_108]]#4, %[[VAL_108]]#5 : !tt.tensordesc<tensor<128x64xf16, #[[$ATTR_2]]>>, !tt.tensordesc<tensor<256x64xf16, #[[$ATTR_2]]>>, !tt.tensordesc<tensor<128x256xf16, #[[$ATTR_2]]>>, i32, i32, i32, i32, i32, i32, i32
// CHECK:             } else {
// CHECK:               scf.yield %[[VAL_52]], %[[VAL_53]], %[[VAL_54]], %[[VAL_55]], %[[VAL_56]], %[[VAL_57]], %[[VAL_58]], %[[VAL_61]], %[[VAL_62]], %[[VAL_63]] : !tt.tensordesc<tensor<128x64xf16, #[[$ATTR_2]]>>, !tt.tensordesc<tensor<256x64xf16, #[[$ATTR_2]]>>, !tt.tensordesc<tensor<128x256xf16, #[[$ATTR_2]]>>, i32, i32, i32, i32, i32, i32, i32
// CHECK:             }
// CHECK:             %[[VAL_109:.*]] = arith.muli %[[VAL_66]], %[[VAL_16]] : i32
// CHECK:             %[[VAL_110:.*]] = tt.descriptor_load %[[VAL_111:.*]]#0{{\[}}%[[VAL_111]]#5, %[[VAL_109]]] : !tt.tensordesc<tensor<128x64xf16, #[[$ATTR_2]]>> -> tensor<128x64xf16, #[[$ATTR_0]]>
// CHECK:             %[[VAL_112:.*]] = ttg.local_alloc %[[VAL_110]] : (tensor<128x64xf16, #[[$ATTR_0]]>) -> !ttg.memdesc<128x64xf16, #[[$ATTR_2]], #[[$ATTR_4]]>
// CHECK:             %[[VAL_113:.*]] = tt.descriptor_load %[[VAL_111]]#1{{\[}}%[[VAL_111]]#6, %[[VAL_109]]] : !tt.tensordesc<tensor<256x64xf16, #[[$ATTR_2]]>> -> tensor<256x64xf16, #[[$ATTR_0]]>
// CHECK:             %[[VAL_114:.*]] = ttg.local_alloc %[[VAL_113]] : (tensor<256x64xf16, #[[$ATTR_0]]>) -> !ttg.memdesc<256x64xf16, #[[$ATTR_2]], #[[$ATTR_4]]>
// CHECK:             %[[VAL_115:.*]] = ttg.memdesc_trans %[[VAL_114]] {order = array<i32: 1, 0>} : !ttg.memdesc<256x64xf16, #[[$ATTR_2]], #[[$ATTR_4]]> -> !ttg.memdesc<64x256xf16, #[[$ATTR_3]], #[[$ATTR_4]]>
// CHECK:             %[[VAL_116:.*]] = ttng.warp_group_dot %[[VAL_112]], %[[VAL_115]], %[[VAL_59]], %[[VAL_60]] {inputPrecision = 0 : i32, isAsync = true} : !ttg.memdesc<128x64xf16, #[[$ATTR_2]], #[[$ATTR_4]]> * !ttg.memdesc<64x256xf16, #[[$ATTR_3]], #[[$ATTR_4]]> -> tensor<128x256xf32, #[[$ATTR_1]]>
// CHECK:             %[[VAL_117:.*]]:3 = ttng.warp_group_dot_wait %[[VAL_116]], %[[VAL_112]], %[[VAL_115]] {pendings = 0 : i32} : tensor<128x256xf32, #[[$ATTR_1]]>, !ttg.memdesc<128x64xf16, #[[$ATTR_2]], #[[$ATTR_4]]>, !ttg.memdesc<64x256xf16, #[[$ATTR_3]], #[[$ATTR_4]]>
// CHECK:             %[[VAL_118:.*]] = arith.cmpi eq, %[[VAL_66]], %[[VAL_44]] : i32
// CHECK:             %[[VAL_119:.*]] = arith.cmpi ne, %[[VAL_66]], %[[VAL_44]] : i32
// CHECK:             scf.if %[[VAL_118]] {
// CHECK:               %[[VAL_120:.*]] = arith.truncf %[[VAL_117]]#0 : tensor<128x256xf32, #[[$ATTR_1]]> to tensor<128x256xf16, #[[$ATTR_1]]>
// CHECK:               ttng.async_tma_store_wait {pendings = 0 : i32}
// CHECK:               ttg.local_store %[[VAL_120]], %[[VAL_45]] : tensor<128x256xf16, #[[$ATTR_1]]> -> !ttg.memdesc<128x256xf16, #[[$ATTR_2]], #[[$ATTR_4]], mutable>
// CHECK:               ttng.fence_async_shared {bCluster = false}
// CHECK:               ttng.async_tma_copy_local_to_global %[[VAL_111]]#2{{\[}}%[[VAL_111]]#5, %[[VAL_111]]#6] %[[VAL_45]] : !tt.tensordesc<tensor<128x256xf16, #[[$ATTR_2]]>>, !ttg.memdesc<128x256xf16, #[[$ATTR_2]], #[[$ATTR_4]], mutable>
// CHECK:             }
// CHECK:             scf.yield %[[VAL_66]], %[[VAL_111]]#0, %[[VAL_111]]#1, %[[VAL_111]]#2, %[[VAL_111]]#3, %[[VAL_111]]#4, %[[VAL_111]]#5, %[[VAL_111]]#6, %[[VAL_117]]#0, %[[VAL_119]], %[[VAL_111]]#7, %[[VAL_111]]#8, %[[VAL_111]]#9 : i32, !tt.tensordesc<tensor<128x64xf16, #[[$ATTR_2]]>>, !tt.tensordesc<tensor<256x64xf16, #[[$ATTR_2]]>>, !tt.tensordesc<tensor<128x256xf16, #[[$ATTR_2]]>>, i32, i32, i32, i32, tensor<128x256xf32, #[[$ATTR_1]]>, i1, i32, i32, i32
// CHECK:           }
// CHECK:           ttng.async_tma_store_wait {pendings = 0 : i32}
// CHECK:           ttg.local_dealloc %[[VAL_45]] : !ttg.memdesc<128x256xf16, #[[$ATTR_2]], #[[$ATTR_4]], mutable>
// CHECK:           tt.return
// CHECK:         }
module attributes {"ttg.num-ctas" = 1 : i32, "ttg.num-warps" = 8 : i32, ttg.target = "cuda:90", "ttg.threads-per-warp" = 32 : i32} {
  tt.func public @matmul_kernel_descriptor_persistent(%arg0: !tt.ptr<f16> {tt.divisibility = 16 : i32}, %arg1: !tt.ptr<f16> {tt.divisibility = 16 : i32}, %arg2: !tt.ptr<f16> {tt.divisibility = 16 : i32}, %arg3: i32 {tt.divisibility = 16 : i32}, %arg4: i32 {tt.divisibility = 16 : i32}, %arg5: i32 {tt.divisibility = 16 : i32}) {
    %c1_i32 = arith.constant 1 : i32
    %c132_i32 = arith.constant 132 : i32
    %c-1_i32 = arith.constant -1 : i32
    %c0_i32 = arith.constant 0 : i32
    %c8_i32 = arith.constant 8 : i32
    %c128_i32 = arith.constant 128 : i32
    %c256_i32 = arith.constant 256 : i32
    %c64_i32 = arith.constant 64 : i32
    %c1_i64 = arith.constant 1 : i64
    %c127_i32 = arith.constant 127 : i32
    %c255_i32 = arith.constant 255 : i32
    %c63_i32 = arith.constant 63 : i32
    %cst = arith.constant dense<0.000000e+00> : tensor<128x256xf32, #ttg.nvidia_mma<{versionMajor = 3, versionMinor = 0, warpsPerCTA = [8, 1], instrShape = [16, 256, 16]}>>
    %0 = tt.get_program_id x : i32
    %1 = arith.addi %arg3, %c127_i32 : i32
    %2 = arith.divsi %1, %c128_i32 : i32
    %3 = arith.addi %arg4, %c255_i32 : i32
    %4 = arith.divsi %3, %c256_i32 : i32
    %5 = arith.addi %arg5, %c63_i32 : i32
    %6 = arith.divsi %5, %c64_i32 : i32
    %7 = arith.muli %2, %4 : i32
    %8 = arith.extsi %arg5 : i32 to i64
    %9 = tt.make_tensor_descriptor %arg0, [%arg3, %arg5], [%8, %c1_i64] : <f16>, <tensor<128x64xf16, #nvmma_128>>
    %10 = tt.make_tensor_descriptor %arg1, [%arg4, %arg5], [%8, %c1_i64] : <f16>, <tensor<256x64xf16, #nvmma_128>>
    %11 = arith.extsi %arg4 : i32 to i64
    %12 = tt.make_tensor_descriptor %arg2, [%arg3, %arg4], [%11, %c1_i64] : <f16>, <tensor<128x256xf16, #nvmma_128>>
    %13 = arith.divsi %7, %c132_i32 : i32
    %14 = arith.remsi %7, %c132_i32 : i32
    %15 = arith.cmpi slt, %0, %14 : i32
    %16 = scf.if %15 -> (i32) {
      %23 = arith.addi %13, %c1_i32 : i32
      scf.yield %23 : i32
    } else {
      scf.yield %13 : i32
    }
    %17 = arith.subi %0, %c132_i32 : i32
    %18 = arith.muli %4, %c8_i32 : i32
    %19 = tt.elementwise_inline_asm "mov.b32 $0, 0;" {constraints = "=r", packed_element = 1 : i32, pure = true} -> i32
    %20 = arith.muli %6, %16 : i32
    %21 = arith.subi %6, %c1_i32 : i32
    %true = arith.constant true
    %false = arith.constant false
    %22:10 = scf.for %arg6 = %c0_i32 to %20 step %c1_i32 iter_args(%arg7 = %c-1_i32, %arg8 = %9, %arg9 = %10, %arg10 = %12, %arg11 = %17, %arg12 = %c-1_i32, %arg13 = %c0_i32, %arg14 = %c0_i32, %arg15 = %cst, %arg16 = %false) -> (i32, !tt.tensordesc<tensor<128x64xf16, #nvmma_128>>, !tt.tensordesc<tensor<256x64xf16, #nvmma_128>>, !tt.tensordesc<tensor<128x256xf16, #nvmma_128>>, i32, i32, i32, i32, tensor<128x256xf32, #ttg.nvidia_mma<{versionMajor = 3, versionMinor = 0, warpsPerCTA = [8, 1], instrShape = [16, 256, 16]}>>, i1)  : i32 {
      %23 = arith.cmpi eq, %arg7, %21 {loop.cluster = 0 : i32, loop.stage = 0 : i32} : i32
      %24 = arith.addi %arg7, %c1_i32 {loop.cluster = 0 : i32, loop.stage = 0 : i32} : i32
      %25 = arith.select %23, %c0_i32, %24 {loop.cluster = 0 : i32, loop.stage = 0 : i32} : i32
      %26 = arith.cmpi eq, %25, %c0_i32 {loop.cluster = 0 : i32, loop.stage = 0 : i32} : i32
      %27:7 = scf.if %26 -> (!tt.tensordesc<tensor<128x64xf16, #nvmma_128>>, !tt.tensordesc<tensor<256x64xf16, #nvmma_128>>, !tt.tensordesc<tensor<128x256xf16, #nvmma_128>>, i32, i32, i32, i32) {
        %37 = arith.addi %arg12, %c1_i32 : i32
        %38 = arith.cmpi eq, %37, %c1_i32 : i32
        %39:4 = scf.if %38 -> (!tt.tensordesc<tensor<128x64xf16, #nvmma_128>>, !tt.tensordesc<tensor<256x64xf16, #nvmma_128>>, !tt.tensordesc<tensor<128x256xf16, #nvmma_128>>, i32) {
          %51 = tt.addptr %arg0, %19 : !tt.ptr<f16>, i32
          %52 = tt.make_tensor_descriptor %51, [%arg3, %arg5], [%8, %c1_i64] : <f16>, <tensor<128x64xf16, #nvmma_128>>
          %53 = tt.addptr %arg1, %19 : !tt.ptr<f16>, i32
          %54 = tt.make_tensor_descriptor %53, [%arg4, %arg5], [%8, %c1_i64] : <f16>, <tensor<256x64xf16, #nvmma_128>>
          %55 = tt.addptr %arg2, %19 : !tt.ptr<f16>, i32
          %56 = tt.make_tensor_descriptor %55, [%arg3, %arg4], [%11, %c1_i64] : <f16>, <tensor<128x256xf16, #nvmma_128>>
          scf.yield %52, %54, %56, %c0_i32 : !tt.tensordesc<tensor<128x64xf16, #nvmma_128>>, !tt.tensordesc<tensor<256x64xf16, #nvmma_128>>, !tt.tensordesc<tensor<128x256xf16, #nvmma_128>>, i32
        } else {
          scf.yield %arg8, %arg9, %arg10, %37 : !tt.tensordesc<tensor<128x64xf16, #nvmma_128>>, !tt.tensordesc<tensor<256x64xf16, #nvmma_128>>, !tt.tensordesc<tensor<128x256xf16, #nvmma_128>>, i32
        }
        %40 = arith.addi %arg11, %c132_i32 : i32
        %41 = arith.divsi %40, %18 : i32
        %42 = arith.muli %41, %c8_i32 : i32
        %43 = arith.subi %2, %42 : i32
        %44 = arith.minsi %43, %c8_i32 : i32
        %45 = arith.remsi %40, %44 : i32
        %46 = arith.addi %42, %45 : i32
        %47 = arith.remsi %40, %18 : i32
        %48 = arith.divsi %47, %44 : i32
        %49 = arith.muli %46, %c128_i32 : i32
        %50 = arith.muli %48, %c256_i32 : i32
        scf.yield %39#0, %39#1, %39#2, %40, %39#3, %49, %50 : !tt.tensordesc<tensor<128x64xf16, #nvmma_128>>, !tt.tensordesc<tensor<256x64xf16, #nvmma_128>>, !tt.tensordesc<tensor<128x256xf16, #nvmma_128>>, i32, i32, i32, i32
      } else {
        scf.yield %arg8, %arg9, %arg10, %arg11, %arg12, %arg13, %arg14 : !tt.tensordesc<tensor<128x64xf16, #nvmma_128>>, !tt.tensordesc<tensor<256x64xf16, #nvmma_128>>, !tt.tensordesc<tensor<128x256xf16, #nvmma_128>>, i32, i32, i32, i32
      } {loop.cluster = 0 : i32, loop.stage = 0 : i32}
      %28 = arith.muli %25, %c64_i32 {loop.cluster = 2 : i32, loop.stage = 0 : i32} : i32
      %29 = tt.descriptor_load %27#0[%27#5, %28] {loop.cluster = 2 : i32, loop.stage = 0 : i32} : !tt.tensordesc<tensor<128x64xf16, #nvmma_128>> -> tensor<128x64xf16, #ttg.blocked<{sizePerThread = [1, 1], threadsPerWarp = [1, 32], warpsPerCTA = [4, 2], order = [1, 0]}>>
      %30 = ttg.local_alloc %29 {loop.cluster = 1 : i32, loop.stage = 2 : i32} : (tensor<128x64xf16, #ttg.blocked<{sizePerThread = [1, 1], threadsPerWarp = [1, 32], warpsPerCTA = [4, 2], order = [1, 0]}>>) -> !ttg.memdesc<128x64xf16, #ttg.nvmma_shared<{swizzlingByteWidth = 128, transposed = false, elementBitWidth = 16}>, #ttg.shared_memory>
      %31 = tt.descriptor_load %27#1[%27#6, %28] {loop.cluster = 2 : i32, loop.stage = 0 : i32} : !tt.tensordesc<tensor<256x64xf16, #nvmma_128>> -> tensor<256x64xf16, #ttg.blocked<{sizePerThread = [1, 1], threadsPerWarp = [1, 32], warpsPerCTA = [4, 2], order = [1, 0]}>>
      %32 = ttg.local_alloc %31 {loop.cluster = 1 : i32, loop.stage = 2 : i32} : (tensor<256x64xf16, #ttg.blocked<{sizePerThread = [1, 1], threadsPerWarp = [1, 32], warpsPerCTA = [4, 2], order = [1, 0]}>>) -> !ttg.memdesc<256x64xf16, #ttg.nvmma_shared<{swizzlingByteWidth = 128, transposed = false, elementBitWidth = 16}>, #ttg.shared_memory>
      %33 = ttg.memdesc_trans %32 {loop.cluster = 1 : i32, loop.stage = 2 : i32, order = array<i32: 1, 0>} : !ttg.memdesc<256x64xf16, #ttg.nvmma_shared<{swizzlingByteWidth = 128, transposed = false, elementBitWidth = 16}>, #ttg.shared_memory> -> !ttg.memdesc<64x256xf16, #ttg.nvmma_shared<{swizzlingByteWidth = 128, transposed = true, elementBitWidth = 16}>, #ttg.shared_memory>
      %34 = ttng.warp_group_dot %30, %33, %arg15, %arg16 {inputPrecision = 0 : i32, loop.cluster = 1 : i32, loop.stage = 2 : i32} : !ttg.memdesc<128x64xf16, #ttg.nvmma_shared<{swizzlingByteWidth = 128, transposed = false, elementBitWidth = 16}>, #ttg.shared_memory> * !ttg.memdesc<64x256xf16, #ttg.nvmma_shared<{swizzlingByteWidth = 128, transposed = true, elementBitWidth = 16}>, #ttg.shared_memory> -> tensor<128x256xf32, #ttg.nvidia_mma<{versionMajor = 3, versionMinor = 0, warpsPerCTA = [8, 1], instrShape = [16, 256, 16]}>>
      %35 = arith.cmpi eq, %25, %21 {loop.cluster = 3 : i32, loop.stage = 2 : i32} : i32
      %36 = scf.if %35 -> (i1) {
        %37 = arith.truncf %34 : tensor<128x256xf32, #ttg.nvidia_mma<{versionMajor = 3, versionMinor = 0, warpsPerCTA = [8, 1], instrShape = [16, 256, 16]}>> to tensor<128x256xf16, #ttg.nvidia_mma<{versionMajor = 3, versionMinor = 0, warpsPerCTA = [8, 1], instrShape = [16, 256, 16]}>>
        %38 = ttg.convert_layout %37 : tensor<128x256xf16, #ttg.nvidia_mma<{versionMajor = 3, versionMinor = 0, warpsPerCTA = [8, 1], instrShape = [16, 256, 16]}>> -> tensor<128x256xf16, #ttg.blocked<{sizePerThread = [1, 1], threadsPerWarp = [1, 32], warpsPerCTA = [1, 8], order = [1, 0]}>>
        tt.descriptor_store %27#2[%27#5, %27#6], %38 : !tt.tensordesc<tensor<128x256xf16, #nvmma_128>>, tensor<128x256xf16, #ttg.blocked<{sizePerThread = [1, 1], threadsPerWarp = [1, 32], warpsPerCTA = [1, 8], order = [1, 0]}>>
        scf.yield %false : i1
      } else {
        scf.yield %true : i1
      } {loop.cluster = 3 : i32, loop.stage = 2 : i32}
      scf.yield %25, %27#0, %27#1, %27#2, %27#3, %27#4, %27#5, %27#6, %34, %36 : i32, !tt.tensordesc<tensor<128x64xf16, #nvmma_128>>, !tt.tensordesc<tensor<256x64xf16, #nvmma_128>>, !tt.tensordesc<tensor<128x256xf16, #nvmma_128>>, i32, i32, i32, i32, tensor<128x256xf32, #ttg.nvidia_mma<{versionMajor = 3, versionMinor = 0, warpsPerCTA = [8, 1], instrShape = [16, 256, 16]}>>, i1
    }
    tt.return
  }
}
