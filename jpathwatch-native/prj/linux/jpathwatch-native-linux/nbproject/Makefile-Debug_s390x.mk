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
CND_PLATFORM=GNU-Linux-x86
CND_CONF=Debug_s390x
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/_ext/1386528437/LinuxPathWatchService.o \
	${OBJECTDIR}/_ext/1386528437/Linux.o \
	${OBJECTDIR}/_ext/1386528437/Unix_timespec.o \
	${OBJECTDIR}/_ext/1386528437/JArgument.o \
	${OBJECTDIR}/_ext/1386528437/Unix.o \
	${OBJECTDIR}/_ext/1386528437/nativelib.o


# C Compiler Flags
CFLAGS=-m64

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ../../../dist/Debug/GNU-Linux-s390x/libjpathwatch-native.so

../../../dist/Debug/GNU-Linux-s390x/libjpathwatch-native.so: ${OBJECTFILES}
	${MKDIR} -p ../../../dist/Debug/GNU-Linux-s390x
	${LINK.cc} -shared -o ../../../dist/Debug/GNU-Linux-s390x/libjpathwatch-native.so -fPIC ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/_ext/1386528437/LinuxPathWatchService.o: ../../../src/LinuxPathWatchService.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1386528437
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -I/usr/lib/jvm/java/include -I/usr/lib/jvm/java/include/linux -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1386528437/LinuxPathWatchService.o ../../../src/LinuxPathWatchService.cpp

${OBJECTDIR}/_ext/1386528437/Linux.o: ../../../src/Linux.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1386528437
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -I/usr/lib/jvm/java/include -I/usr/lib/jvm/java/include/linux -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1386528437/Linux.o ../../../src/Linux.cpp

${OBJECTDIR}/_ext/1386528437/Unix_timespec.o: ../../../src/Unix_timespec.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1386528437
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -I/usr/lib/jvm/java/include -I/usr/lib/jvm/java/include/linux -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1386528437/Unix_timespec.o ../../../src/Unix_timespec.cpp

${OBJECTDIR}/_ext/1386528437/JArgument.o: ../../../src/JArgument.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1386528437
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -I/usr/lib/jvm/java/include -I/usr/lib/jvm/java/include/linux -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1386528437/JArgument.o ../../../src/JArgument.cpp

${OBJECTDIR}/_ext/1386528437/Unix.o: ../../../src/Unix.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1386528437
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -I/usr/lib/jvm/java/include -I/usr/lib/jvm/java/include/linux -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1386528437/Unix.o ../../../src/Unix.cpp

${OBJECTDIR}/_ext/1386528437/nativelib.o: ../../../src/nativelib.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1386528437
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -I/usr/lib/jvm/java/include -I/usr/lib/jvm/java/include/linux -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1386528437/nativelib.o ../../../src/nativelib.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ../../../dist/Debug/GNU-Linux-s390x/libjpathwatch-native.so

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
