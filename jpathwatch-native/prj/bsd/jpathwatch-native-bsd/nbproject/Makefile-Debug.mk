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
FC=
AS=as

# Macros
CND_PLATFORM=GNU-Generic
CND_CONF=Debug
CND_DISTDIR=dist

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=build/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/_ext/1386528437/BSD_kevent.o \
	${OBJECTDIR}/_ext/1386528437/Unix_timespec.o \
	${OBJECTDIR}/_ext/1386528437/JArgument.o \
	${OBJECTDIR}/_ext/1386528437/Unix.o \
	${OBJECTDIR}/_ext/1386528437/nativelib.o \
	${OBJECTDIR}/_ext/1386528437/BSD.o


# C Compiler Flags
CFLAGS=

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
	"${MAKE}"  -f nbproject/Makefile-Debug.mk ../../../dist/Debug/FreeBSD-i386/libjpathwatch-native.so

../../../dist/Debug/FreeBSD-i386/libjpathwatch-native.so: ${OBJECTFILES}
	${MKDIR} -p ../../../dist/Debug/FreeBSD-i386
	${LINK.cc} -shared -o ../../../dist/${CND_CONF}/FreeBSD-i386/libjpathwatch-native.so -fPIC ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/_ext/1386528437/BSD_kevent.o: ../../../src/BSD_kevent.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1386528437
	${RM} $@.d
	$(COMPILE.cc) -g -I/usr/local/diablo-jdk1.6.0/include/ -I/usr/local/diablo-jdk1.6.0/include/freebsd -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1386528437/BSD_kevent.o ../../../src/BSD_kevent.cpp

${OBJECTDIR}/_ext/1386528437/Unix_timespec.o: ../../../src/Unix_timespec.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1386528437
	${RM} $@.d
	$(COMPILE.cc) -g -I/usr/local/diablo-jdk1.6.0/include/ -I/usr/local/diablo-jdk1.6.0/include/freebsd -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1386528437/Unix_timespec.o ../../../src/Unix_timespec.cpp

${OBJECTDIR}/_ext/1386528437/JArgument.o: ../../../src/JArgument.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1386528437
	${RM} $@.d
	$(COMPILE.cc) -g -I/usr/local/diablo-jdk1.6.0/include/ -I/usr/local/diablo-jdk1.6.0/include/freebsd -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1386528437/JArgument.o ../../../src/JArgument.cpp

${OBJECTDIR}/_ext/1386528437/Unix.o: ../../../src/Unix.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1386528437
	${RM} $@.d
	$(COMPILE.cc) -g -I/usr/local/diablo-jdk1.6.0/include/ -I/usr/local/diablo-jdk1.6.0/include/freebsd -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1386528437/Unix.o ../../../src/Unix.cpp

${OBJECTDIR}/_ext/1386528437/nativelib.o: ../../../src/nativelib.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1386528437
	${RM} $@.d
	$(COMPILE.cc) -g -I/usr/local/diablo-jdk1.6.0/include/ -I/usr/local/diablo-jdk1.6.0/include/freebsd -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1386528437/nativelib.o ../../../src/nativelib.cpp

${OBJECTDIR}/_ext/1386528437/BSD.o: ../../../src/BSD.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1386528437
	${RM} $@.d
	$(COMPILE.cc) -g -I/usr/local/diablo-jdk1.6.0/include/ -I/usr/local/diablo-jdk1.6.0/include/freebsd -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1386528437/BSD.o ../../../src/BSD.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/Debug
	${RM} ../../../dist/Debug/FreeBSD-i386/libjpathwatch-native.so

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
