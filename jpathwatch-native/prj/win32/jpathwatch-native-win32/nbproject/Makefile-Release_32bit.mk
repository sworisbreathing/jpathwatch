#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc.exe
CCC=g++.exe
CXX=g++.exe
FC=
AS=as.exe

# Macros
CND_PLATFORM=MinGW_TDM-Windows
CND_CONF=Release_32bit
CND_DISTDIR=dist

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=build/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/_ext/1386528437/Windows_OVERLAPPED.o \
	${OBJECTDIR}/_ext/1386528437/Windows_tls.o \
	${OBJECTDIR}/_ext/1386528437/nativelib.o \
	${OBJECTDIR}/_ext/1386528437/Windows.o \
	${OBJECTDIR}/_ext/1386528437/WindowsPathWatchService.o \
	${OBJECTDIR}/_ext/1386528437/Windows_ByteBuffer.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-m32 -mwin32 -mno-cygwin
CXXFLAGS=-m32 -mwin32 -mno-cygwin

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-Release_32bit.mk ../../../dist/Release/Windows-x86/jpathwatch-native.dll

../../../dist/Release/Windows-x86/jpathwatch-native.dll: ${OBJECTFILES}
	${MKDIR} -p ../../../dist/Release/Windows-x86
	${LINK.cc} -Wl,--add-stdcall-alias -shared -o ../../../dist/Release/Windows-x86/jpathwatch-native.dll ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/_ext/1386528437/Windows_OVERLAPPED.o: ../../../src/Windows_OVERLAPPED.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1386528437
	${RM} $@.d
	$(COMPILE.cc) -O2 -D_WIN32 -I/C/Programme/Java/jdk1.6.0_18/include -I/C/Programme/Java/jdk1.6.0_18/include/win32  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1386528437/Windows_OVERLAPPED.o ../../../src/Windows_OVERLAPPED.cpp

${OBJECTDIR}/_ext/1386528437/Windows_tls.o: ../../../src/Windows_tls.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1386528437
	${RM} $@.d
	$(COMPILE.cc) -O2 -D_WIN32 -I/C/Programme/Java/jdk1.6.0_18/include -I/C/Programme/Java/jdk1.6.0_18/include/win32  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1386528437/Windows_tls.o ../../../src/Windows_tls.cpp

${OBJECTDIR}/_ext/1386528437/nativelib.o: ../../../src/nativelib.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1386528437
	${RM} $@.d
	$(COMPILE.cc) -O2 -D_WIN32 -I/C/Programme/Java/jdk1.6.0_18/include -I/C/Programme/Java/jdk1.6.0_18/include/win32  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1386528437/nativelib.o ../../../src/nativelib.cpp

${OBJECTDIR}/_ext/1386528437/Windows.o: ../../../src/Windows.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1386528437
	${RM} $@.d
	$(COMPILE.cc) -O2 -D_WIN32 -I/C/Programme/Java/jdk1.6.0_18/include -I/C/Programme/Java/jdk1.6.0_18/include/win32  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1386528437/Windows.o ../../../src/Windows.cpp

${OBJECTDIR}/_ext/1386528437/WindowsPathWatchService.o: ../../../src/WindowsPathWatchService.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1386528437
	${RM} $@.d
	$(COMPILE.cc) -O2 -D_WIN32 -I/C/Programme/Java/jdk1.6.0_18/include -I/C/Programme/Java/jdk1.6.0_18/include/win32  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1386528437/WindowsPathWatchService.o ../../../src/WindowsPathWatchService.cpp

${OBJECTDIR}/_ext/1386528437/Windows_ByteBuffer.o: ../../../src/Windows_ByteBuffer.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1386528437
	${RM} $@.d
	$(COMPILE.cc) -O2 -D_WIN32 -I/C/Programme/Java/jdk1.6.0_18/include -I/C/Programme/Java/jdk1.6.0_18/include/win32  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1386528437/Windows_ByteBuffer.o ../../../src/Windows_ByteBuffer.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/Release_32bit
	${RM} ../../../dist/Release/Windows-x86/jpathwatch-native.dll

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
