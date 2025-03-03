/*-----------------------------------------------------------------------
 *  JavaCore.cpp --
 *
 *  .
 *
 *-----------------------------------------------------------------------
 * RCS: @(#) $Id: JavaCore.cpp,v 1.4 2002/08/25 03:46:08 davygrvy Exp $
 *-----------------------------------------------------------------------
 */

#if IRCE_USEJAVA

#include "JavaCore.hpp"

#define LOCAL_CLASSPATH "."

Java::Interp::Interp(WriteFunc stdOut, WriteFunc stdErr)
{
    JDK1_1InitArgs vm_args;
    jint res;

#ifdef _WIN32
    if (jvmMod == 0L) {
	//jvmMod = LoadLibrary("jvm.dll");
	jvmMod = LoadLibrary("msjava.dll");
	if (jvmMod) {
	    JNI_CreateJavaVM = (LPFN_JNI_CreateJavaVM)
		    GetProcAddress(jvmMod, "JNI_CreateJavaVM");
	    JNI_GetDefaultJavaVMInitArgs = (LPFN_JNI_GetDefaultJavaVMInitArgs)
		    GetProcAddress(jvmMod, "JNI_GetDefaultJavaVMInitArgs");
	} else {
	    throw "Can't load java dll";
	}
    }
#endif

    vm_args.version = JNI_VERSION_1_1;
    JNI_GetDefaultJavaVMInitArgs(&vm_args);

    vm_args.classpath = "-Djava.class.path=" LOCAL_CLASSPATH;
    //vm_args.properties = properties;

    /* Create the Java VM */
    res = JNI_CreateJavaVM(&jvm, &env, &vm_args);

    if (res < 0) {
	throw "Can't create Java VM";
    }
}

Java::Interp::~Interp()
{
    jvm->DestroyJavaVM();
}

void Java::Interp::setExtensions (const char *className, JNINativeMethod *methodData)
{
    int i = 0;
    jclass clazz = env->FindClass(className);

    if (clazz != 0L) {
	while (methodData[++i].name != 0L)
	    env->RegisterNatives(clazz,&methodData[i],1);
    }
}


#ifdef _WIN32
HMODULE Java::Interp::jvmMod = 0L;
Java::Interp::LPFN_JNI_CreateJavaVM Java::Interp::JNI_CreateJavaVM = 0L;
Java::Interp::LPFN_JNI_GetDefaultJavaVMInitArgs Java::Interp::JNI_GetDefaultJavaVMInitArgs = 0L;
#endif

#endif