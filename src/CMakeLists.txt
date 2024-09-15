cmake_minimum_required(VERSION 3.4.3)

enable_testing()

project(wip)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(LLVM REQUIRED CONFIG)
add_definitions(${LLVM_DEFINITION})

if(WIN32)
    set(llvm_libs
        ${LLVM_LIBRARY_DIRS}/LLVMAArch64AsmParser.lib
        ${LLVM_LIBRARY_DIRS}/LLVMAArch64CodeGen.lib
        ${LLVM_LIBRARY_DIRS}/LLVMAArch64Desc.lib
        ${LLVM_LIBRARY_DIRS}/LLVMAArch64Info.lib
        ${LLVM_LIBRARY_DIRS}/LLVMAArch64Utils.lib
        ${LLVM_LIBRARY_DIRS}/LLVMAggressiveInstCombine.lib
        ${LLVM_LIBRARY_DIRS}/LLVMAMDGPUAsmParser.lib
        ${LLVM_LIBRARY_DIRS}/LLVMAMDGPUCodeGen.lib
        ${LLVM_LIBRARY_DIRS}/LLVMAMDGPUDesc.lib
        ${LLVM_LIBRARY_DIRS}/LLVMAMDGPUInfo.lib
        ${LLVM_LIBRARY_DIRS}/LLVMAMDGPUUtils.lib
        ${LLVM_LIBRARY_DIRS}/LLVMAnalysis.lib
        ${LLVM_LIBRARY_DIRS}/LLVMARMAsmParser.lib
        ${LLVM_LIBRARY_DIRS}/LLVMARMCodeGen.lib
        ${LLVM_LIBRARY_DIRS}/LLVMARMDesc.lib
        ${LLVM_LIBRARY_DIRS}/LLVMARMInfo.lib
        ${LLVM_LIBRARY_DIRS}/LLVMARMUtils.lib
        ${LLVM_LIBRARY_DIRS}/LLVMAsmParser.lib
        ${LLVM_LIBRARY_DIRS}/LLVMAsmPrinter.lib
        ${LLVM_LIBRARY_DIRS}/LLVMAVRAsmParser.lib
        ${LLVM_LIBRARY_DIRS}/LLVMAVRCodeGen.lib
        ${LLVM_LIBRARY_DIRS}/LLVMAVRDesc.lib
        ${LLVM_LIBRARY_DIRS}/LLVMAVRInfo.lib
        ${LLVM_LIBRARY_DIRS}/LLVMBinaryFormat.lib
        ${LLVM_LIBRARY_DIRS}/LLVMBitReader.lib
        ${LLVM_LIBRARY_DIRS}/LLVMBitstreamReader.lib
        ${LLVM_LIBRARY_DIRS}/LLVMBitWriter.lib
        ${LLVM_LIBRARY_DIRS}/LLVMBPFAsmParser.lib
        ${LLVM_LIBRARY_DIRS}/LLVMBPFCodeGen.lib
        ${LLVM_LIBRARY_DIRS}/LLVMBPFDesc.lib
        ${LLVM_LIBRARY_DIRS}/LLVMBPFInfo.lib
        ${LLVM_LIBRARY_DIRS}/LLVMCFGuard.lib
        ${LLVM_LIBRARY_DIRS}/LLVMCodeGen.lib
        ${LLVM_LIBRARY_DIRS}/LLVMCodeGenTypes.lib
        ${LLVM_LIBRARY_DIRS}/LLVMCore.lib
        ${LLVM_LIBRARY_DIRS}/LLVMCoroutines.lib
        ${LLVM_LIBRARY_DIRS}/LLVMCoverage.lib
        ${LLVM_LIBRARY_DIRS}/LLVMDebugInfoBTF.lib
        ${LLVM_LIBRARY_DIRS}/LLVMDebugInfoCodeView.lib
        ${LLVM_LIBRARY_DIRS}/LLVMDebugInfoDWARF.lib
        ${LLVM_LIBRARY_DIRS}/LLVMDebugInfoGSYM.lib
        ${LLVM_LIBRARY_DIRS}/LLVMDebugInfoLogicalView.lib
        ${LLVM_LIBRARY_DIRS}/LLVMDebugInfoMSF.lib
        ${LLVM_LIBRARY_DIRS}/LLVMDebugInfoPDB.lib
        ${LLVM_LIBRARY_DIRS}/LLVMDemangle.lib
        ${LLVM_LIBRARY_DIRS}/LLVMDlltoolDriver.lib
        ${LLVM_LIBRARY_DIRS}/LLVMDWARFLinker.lib
        ${LLVM_LIBRARY_DIRS}/LLVMDWARFLinkerClassic.lib
        ${LLVM_LIBRARY_DIRS}/LLVMDWARFLinkerParallel.lib
        ${LLVM_LIBRARY_DIRS}/LLVMDWP.lib
        ${LLVM_LIBRARY_DIRS}/LLVMExecutionEngine.lib
        ${LLVM_LIBRARY_DIRS}/LLVMExtensions.lib
        ${LLVM_LIBRARY_DIRS}/LLVMFileCheck.lib
        ${LLVM_LIBRARY_DIRS}/LLVMFrontendDriver.lib
        ${LLVM_LIBRARY_DIRS}/LLVMFrontendHLSL.lib
        ${LLVM_LIBRARY_DIRS}/LLVMFrontendOffloading.lib
        ${LLVM_LIBRARY_DIRS}/LLVMFrontendOpenACC.lib
        ${LLVM_LIBRARY_DIRS}/LLVMFrontendOpenMP.lib
        ${LLVM_LIBRARY_DIRS}/LLVMFuzzerCLI.lib
        ${LLVM_LIBRARY_DIRS}/LLVMFuzzMutate.lib
        ${LLVM_LIBRARY_DIRS}/LLVMGlobalISel.lib
        ${LLVM_LIBRARY_DIRS}/LLVMHexagonAsmParser.lib
        ${LLVM_LIBRARY_DIRS}/LLVMHexagonCodeGen.lib
        ${LLVM_LIBRARY_DIRS}/LLVMHexagonDesc.lib
        ${LLVM_LIBRARY_DIRS}/LLVMHexagonInfo.lib
        ${LLVM_LIBRARY_DIRS}/LLVMHipStdPar.lib
        ${LLVM_LIBRARY_DIRS}/LLVMInstCombine.lib
        ${LLVM_LIBRARY_DIRS}/LLVMInstrumentation.lib
        ${LLVM_LIBRARY_DIRS}/LLVMInterfaceStub.lib
        ${LLVM_LIBRARY_DIRS}/LLVMInterpreter.lib
        ${LLVM_LIBRARY_DIRS}/LLVMipo.lib
        ${LLVM_LIBRARY_DIRS}/LLVMIRPrinter.lib
        ${LLVM_LIBRARY_DIRS}/LLVMIRReader.lib
        ${LLVM_LIBRARY_DIRS}/LLVMJITLink.lib
        ${LLVM_LIBRARY_DIRS}/LLVMLanaiAsmParser.lib
        ${LLVM_LIBRARY_DIRS}/LLVMLanaiCodeGen.lib
        ${LLVM_LIBRARY_DIRS}/LLVMLanaiDesc.lib
        ${LLVM_LIBRARY_DIRS}/LLVMLanaiInfo.lib
        ${LLVM_LIBRARY_DIRS}/LLVMLibDriver.lib
        ${LLVM_LIBRARY_DIRS}/LLVMLineEditor.lib
        ${LLVM_LIBRARY_DIRS}/LLVMLinker.lib
        ${LLVM_LIBRARY_DIRS}/LLVMLoongArchAsmParser.lib
        ${LLVM_LIBRARY_DIRS}/LLVMLoongArchCodeGen.lib
        ${LLVM_LIBRARY_DIRS}/LLVMLoongArchDesc.lib
        ${LLVM_LIBRARY_DIRS}/LLVMLoongArchInfo.lib
        ${LLVM_LIBRARY_DIRS}/LLVMLTO.lib
        ${LLVM_LIBRARY_DIRS}/LLVMMC.lib
        ${LLVM_LIBRARY_DIRS}/LLVMMCA.lib
        ${LLVM_LIBRARY_DIRS}/LLVMMCDisassembler.lib
        ${LLVM_LIBRARY_DIRS}/LLVMMCJIT.lib
        ${LLVM_LIBRARY_DIRS}/LLVMMCParser.lib
        ${LLVM_LIBRARY_DIRS}/LLVMMipsAsmParser.lib
        ${LLVM_LIBRARY_DIRS}/LLVMMipsCodeGen.lib
        ${LLVM_LIBRARY_DIRS}/LLVMMipsDesc.lib
        ${LLVM_LIBRARY_DIRS}/LLVMMipsInfo.lib
        ${LLVM_LIBRARY_DIRS}/LLVMMIRParser.lib
        ${LLVM_LIBRARY_DIRS}/LLVMMSP430AsmParser.lib
        ${LLVM_LIBRARY_DIRS}/LLVMMSP430CodeGen.lib
        ${LLVM_LIBRARY_DIRS}/LLVMMSP430Desc.lib
        ${LLVM_LIBRARY_DIRS}/LLVMMSP430Info.lib
        ${LLVM_LIBRARY_DIRS}/LLVMNVPTXCodeGen.lib
        ${LLVM_LIBRARY_DIRS}/LLVMNVPTXDesc.lib
        ${LLVM_LIBRARY_DIRS}/LLVMNVPTXInfo.lib
        ${LLVM_LIBRARY_DIRS}/LLVMObjCARCOpts.lib
        ${LLVM_LIBRARY_DIRS}/LLVMObjCopy.lib
        ${LLVM_LIBRARY_DIRS}/LLVMObject.lib
        ${LLVM_LIBRARY_DIRS}/LLVMObjectYAML.lib
        ${LLVM_LIBRARY_DIRS}/LLVMOption.lib
        ${LLVM_LIBRARY_DIRS}/LLVMOrcDebugging.lib
        ${LLVM_LIBRARY_DIRS}/LLVMOrcJIT.lib
        ${LLVM_LIBRARY_DIRS}/LLVMOrcShared.lib
        ${LLVM_LIBRARY_DIRS}/LLVMOrcTargetProcess.lib
        ${LLVM_LIBRARY_DIRS}/LLVMPasses.lib
        ${LLVM_LIBRARY_DIRS}/LLVMPowerPCAsmParser.lib
        ${LLVM_LIBRARY_DIRS}/LLVMPowerPCCodeGen.lib
        ${LLVM_LIBRARY_DIRS}/LLVMPowerPCDesc.lib
        ${LLVM_LIBRARY_DIRS}/LLVMPowerPCInfo.lib
        ${LLVM_LIBRARY_DIRS}/LLVMProfileData.lib
        ${LLVM_LIBRARY_DIRS}/LLVMRemarks.lib
        ${LLVM_LIBRARY_DIRS}/LLVMRISCVAsmParser.lib
        ${LLVM_LIBRARY_DIRS}/LLVMRISCVCodeGen.lib
        ${LLVM_LIBRARY_DIRS}/LLVMRISCVDesc.lib
        ${LLVM_LIBRARY_DIRS}/LLVMRISCVInfo.lib
        ${LLVM_LIBRARY_DIRS}/LLVMRISCVTargetMCA.lib
        ${LLVM_LIBRARY_DIRS}/LLVMRuntimeDyld.lib
        ${LLVM_LIBRARY_DIRS}/LLVMScalarOpts.lib
        ${LLVM_LIBRARY_DIRS}/LLVMSelectionDAG.lib
        ${LLVM_LIBRARY_DIRS}/LLVMSparcAsmParser.lib
        ${LLVM_LIBRARY_DIRS}/LLVMSparcCodeGen.lib
        ${LLVM_LIBRARY_DIRS}/LLVMSparcDesc.lib
        ${LLVM_LIBRARY_DIRS}/LLVMSparcInfo.lib
        ${LLVM_LIBRARY_DIRS}/LLVMSupport.lib
        ${LLVM_LIBRARY_DIRS}/LLVMSymbolize.lib
        ${LLVM_LIBRARY_DIRS}/LLVMSystemZAsmParser.lib
        ${LLVM_LIBRARY_DIRS}/LLVMSystemZCodeGen.lib
        ${LLVM_LIBRARY_DIRS}/LLVMSystemZDesc.lib
        ${LLVM_LIBRARY_DIRS}/LLVMSystemZInfo.lib
        ${LLVM_LIBRARY_DIRS}/LLVMTableGen.lib
        ${LLVM_LIBRARY_DIRS}/LLVMTarget.lib
        ${LLVM_LIBRARY_DIRS}/LLVMTargetParser.lib
        ${LLVM_LIBRARY_DIRS}/LLVMTextAPI.lib
        ${LLVM_LIBRARY_DIRS}/LLVMTextAPIBinaryReader.lib
        ${LLVM_LIBRARY_DIRS}/LLVMTransformUtils.lib
        ${LLVM_LIBRARY_DIRS}/LLVMVEAsmParser.lib
        ${LLVM_LIBRARY_DIRS}/LLVMVECodeGen.lib
        ${LLVM_LIBRARY_DIRS}/LLVMVectorize.lib
        ${LLVM_LIBRARY_DIRS}/LLVMVEDesc.lib
        ${LLVM_LIBRARY_DIRS}/LLVMVEInfo.lib
        ${LLVM_LIBRARY_DIRS}/LLVMWebAssemblyAsmParser.lib
        ${LLVM_LIBRARY_DIRS}/LLVMWebAssemblyCodeGen.lib
        ${LLVM_LIBRARY_DIRS}/LLVMWebAssemblyDesc.lib
        ${LLVM_LIBRARY_DIRS}/LLVMWebAssemblyInfo.lib
        ${LLVM_LIBRARY_DIRS}/LLVMWebAssemblyUtils.lib
        ${LLVM_LIBRARY_DIRS}/LLVMWindowsDriver.lib
        ${LLVM_LIBRARY_DIRS}/LLVMWindowsManifest.lib
        ${LLVM_LIBRARY_DIRS}/LLVMX86AsmParser.lib
        ${LLVM_LIBRARY_DIRS}/LLVMX86CodeGen.lib
        ${LLVM_LIBRARY_DIRS}/LLVMX86Desc.lib
        ${LLVM_LIBRARY_DIRS}/LLVMX86Info.lib
        ${LLVM_LIBRARY_DIRS}/LLVMX86TargetMCA.lib
        ${LLVM_LIBRARY_DIRS}/LLVMXCoreCodeGen.lib
        ${LLVM_LIBRARY_DIRS}/LLVMXCoreDesc.lib
        ${LLVM_LIBRARY_DIRS}/LLVMXCoreInfo.lib
        ${LLVM_LIBRARY_DIRS}/LLVMXRay.lib
        ws2_32.lib
    )
else()
    execute_process(COMMAND llvm-config-18 --libs all
                    OUTPUT_VARIABLE llvm_all_libs
                    OUTPUT_STRIP_TRAILING_WHITESPACE)
    execute_process(COMMAND llvm-config-18 --system-libs
                    OUTPUT_VARIABLE llvm_system_libs
                    OUTPUT_STRIP_TRAILING_WHITESPACE)
    set(llvm_libs "${llvm_all_libs} ${llvm_system_libs}")
    string(STRIP ${llvm_libs} llvm_libs)
endif()

add_subdirectory(compilerapi)
add_subdirectory(compiler)
add_subdirectory(unittests)

add_test(NAME tests COMMAND unittests WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/..)
