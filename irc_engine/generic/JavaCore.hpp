/*-----------------------------------------------------------------------
 *  JavaCore.hpp --
 *
 *  .
 *
 *-----------------------------------------------------------------------
 * RCS: @(#) $Id: JavaCore.hpp,v 1.5 2002/08/25 03:46:08 davygrvy Exp $
 *-----------------------------------------------------------------------
 */

#include "jni.h"
#ifdef _WIN32
#   include <windows.h>
#endif


namespace Java {

    typedef void (*WriteFunc) (const char *);

    class Interp
    {
	JavaVM *jvm;
	JNIEnv *env;

#ifdef _WIN32
	static HMODULE jvmMod;
	typedef jint (JNICALL *LPFN_JNI_CreateJavaVM)(JavaVM **pvm, JNIEnv **penv, void *args);
	typedef jint (JNICALL *LPFN_JNI_GetDefaultJavaVMInitArgs)(void *args);
	static LPFN_JNI_CreateJavaVM JNI_CreateJavaVM;
	static LPFN_JNI_GetDefaultJavaVMInitArgs JNI_GetDefaultJavaVMInitArgs;
#endif

    public:
	Interp(WriteFunc stdOut, WriteFunc stdErr);
	~Interp();
	int Load (const char *scriptFileName, char **errorString);
	int Eval ();
	void setExtensions(const char *className, JNINativeMethod *methodData);
    };
}