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
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-MacOSX
CND_CONF=Debug_32bit
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/_ext/1386528437/BSD_kevent.o \
	${OBJECTDIR}/_ext/1386528437/Unix_timespec.o \
	${OBJECTDIR}/_ext/1386528437/JArgument.o \
	${OBJECTDIR}/_ext/1386528437/Unix.o \
	${OBJECTDIR}/_ext/1386528437/nativelib.o \
	${OBJECTDIR}/_ext/1386528437/BSD.o


# C Compiler Flags
CFLAGS=-m32

# CC Compiler Flags
CCFLAGS=-m32
CXXFLAGS=-m32

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ../../../dist/Debug/${CND_PLATFORM}/libjpathwatch-native-32.dylib

../../../dist/Debug/${CND_PLATFORM}/libjpathwatch-native-32.dylib: ${OBJECTFILES}
	${MKDIR} -p ../../../dist/Debug/${CND_PLATFORM}
	${LINK.cc} -dynamiclib -install_name libjpathwatch-native-32.dylib -o ../../../dist/Debug/${CND_PLATFORM}/libjpathwatch-native-32.dylib -fPIC ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/_ext/1386528437/BSD_kevent.o: ../../../src/BSD_kevent.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1386528437
	${RM} $@.d
	$(COMPILE.cc) -g -I/System/Library/Frameworks/JavaVM.framework/Versions/Current/Headers -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1386528437/BSD_kevent.o ../../../src/BSD_kevent.cpp

${OBJECTDIR}/_ext/1386528437/Unix_timespec.o: ../../../src/Unix_timespec.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1386528437
	${RM} $@.d
	$(COMPILE.cc) -g -I/System/Library/Frameworks/JavaVM.framework/Versions/Current/Headers -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1386528437/Unix_timespec.o ../../../src/Unix_timespec.cpp

${OBJECTDIR}/_ext/1386528437/JArgument.o: ../../../src/JArgument.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1386528437
	${RM} $@.d
	$(COMPILE.cc) -g -I/System/Library/Frameworks/JavaVM.framework/Versions/Current/Headers -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1386528437/JArgument.o ../../../src/JArgument.cpp

${OBJECTDIR}/_ext/1386528437/Unix.o: ../../../src/Unix.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1386528437
	${RM} $@.d
	$(COMPILE.cc) -g -I/System/Library/Frameworks/JavaVM.framework/Versions/Current/Headers -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1386528437/Unix.o ../../../src/Unix.cpp

${OBJECTDIR}/_ext/1386528437/nativelib.o: ../../../src/nativelib.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1386528437
	${RM} $@.d
	$(COMPILE.cc) -g -I/System/Library/Frameworks/JavaVM.framework/Versions/Current/Headers -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1386528437/nativelib.o ../../../src/nativelib.cpp

${OBJECTDIR}/_ext/1386528437/BSD.o: ../../../src/BSD.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1386528437
	${RM} $@.d
	$(COMPILE.cc) -g -I/System/Library/Frameworks/JavaVM.framework/Versions/Current/Headers -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1386528437/BSD.o ../../../src/BSD.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ../../../dist/Debug/${CND_PLATFORM}/libjpathwatch-native-32.dylib

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
