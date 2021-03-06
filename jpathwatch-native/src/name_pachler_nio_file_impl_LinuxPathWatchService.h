/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class name_pachler_nio_file_impl_LinuxPathWatchService */

#ifndef _Included_name_pachler_nio_file_impl_LinuxPathWatchService
#define _Included_name_pachler_nio_file_impl_LinuxPathWatchService
#ifdef __cplusplus
extern "C" {
#endif
#undef name_pachler_nio_file_impl_LinuxPathWatchService_FLAG_WATCH_SUBTREE
#define name_pachler_nio_file_impl_LinuxPathWatchService_FLAG_WATCH_SUBTREE 4096L
#undef name_pachler_nio_file_impl_LinuxPathWatchService_FLAG_ACCURATE
#define name_pachler_nio_file_impl_LinuxPathWatchService_FLAG_ACCURATE 8192L
#undef name_pachler_nio_file_impl_LinuxPathWatchService_FLAG_FILTER_ENTRY_RENAME_FROM
#define name_pachler_nio_file_impl_LinuxPathWatchService_FLAG_FILTER_ENTRY_RENAME_FROM 2L
#undef name_pachler_nio_file_impl_LinuxPathWatchService_FLAG_FILTER_ENTRY_RENAME_TO
#define name_pachler_nio_file_impl_LinuxPathWatchService_FLAG_FILTER_ENTRY_RENAME_TO 4L
#undef name_pachler_nio_file_impl_LinuxPathWatchService_FLAG_FILTER_ENTRY_CREATE
#define name_pachler_nio_file_impl_LinuxPathWatchService_FLAG_FILTER_ENTRY_CREATE 8L
#undef name_pachler_nio_file_impl_LinuxPathWatchService_FLAG_FILTER_ENTRY_DELETE
#define name_pachler_nio_file_impl_LinuxPathWatchService_FLAG_FILTER_ENTRY_DELETE 16L
#undef name_pachler_nio_file_impl_LinuxPathWatchService_FLAG_FILTER_ENTRY_MODIFY
#define name_pachler_nio_file_impl_LinuxPathWatchService_FLAG_FILTER_ENTRY_MODIFY 32L
#undef name_pachler_nio_file_impl_LinuxPathWatchService_FLAG_FILTER_KEY_INVALID
#define name_pachler_nio_file_impl_LinuxPathWatchService_FLAG_FILTER_KEY_INVALID 64L
#undef name_pachler_nio_file_impl_LinuxPathWatchService_CMD_CLOSE
#define name_pachler_nio_file_impl_LinuxPathWatchService_CMD_CLOSE 1L
#undef name_pachler_nio_file_impl_LinuxPathWatchService_CMD_NOTIFY
#define name_pachler_nio_file_impl_LinuxPathWatchService_CMD_NOTIFY 2L
/*
 * Class:     name_pachler_nio_file_impl_LinuxPathWatchService
 * Method:    translateInotifyEvents
 * Signature: ([BII)I
 */
JNIEXPORT jint JNICALL Java_name_pachler_nio_file_impl_LinuxPathWatchService_translateInotifyEvents
  (JNIEnv *, jobject, jbyteArray, jint, jint);

#ifdef __cplusplus
}
#endif
#endif
