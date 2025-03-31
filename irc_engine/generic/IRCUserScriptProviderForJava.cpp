/*-----------------------------------------------------------------------
 *  IRCUserScriptProviderForJava.cpp --
 *
 *  Makes the jump to Java (JVM).  Both Tcl and Java calls are used here.
 *
 *-----------------------------------------------------------------------
 * RCS: @(#) $Id: IRCUserScriptProviderForJava.cpp,v 1.16 2002/09/14 20:31:25 davygrvy Exp $
 *-----------------------------------------------------------------------
 */

#include "IRCUserScriptProvider.hpp"

#if IRCE_USEJAVA

#include "JavaCore.hpp"

class IRCUserScriptProviderForJavaPrivateImp
    : public IRCUserScriptProviderPrvImpBase
{
    static void
    WriteStdOut (const char *text)
    {
	Tcl_Channel stdOut = Tcl_GetStdChannel(TCL_STDOUT);
	//Tcl_WriteObj(stdOut, text);
	Tcl_Flush(stdOut);
    }

    static void
    WriteStdErr (const char *text)
    {
	Tcl_Channel stdErr = Tcl_GetStdChannel(TCL_STDERR);
	//Tcl_WriteObj(stdErr, text);
	Tcl_Flush(stdErr);
    }

    static void JNICALL Java_Engine_on (JNIEnv *env, jclass clazz, jstring event, jobject callback)
    {
	IRCUserScriptProviderForJavaPrivateImp *iface;
	const char *evStr;
	jboolean isCopy;

	if (connections->Find(getHashCode(env,clazz), &iface) == TCL_OK) {
	    evStr = env->GetStringUTFChars(event, &isCopy);
	    iface->Events.Add(evStr, callback);
	    if (isCopy == JNI_TRUE) env->ReleaseStringUTFChars(event, evStr);
	}
    }

    static jstring JNICALL Java_Engine_fulladdress (JNIEnv *env, jclass clazz)
    {
	IRCUserScriptProviderForJavaPrivateImp *iface;

	if (connections->Find(getHashCode(env,clazz), &iface) == TCL_OK) {
	    return env->NewStringUTF(Tcl_GetString(iface->FullAddress));
	} else {
	    return 0L;
	}
    }

    static jstring JNICALL Java_Engine_nick (JNIEnv *env, jclass clazz)
    {
	IRCUserScriptProviderForJavaPrivateImp *iface;

	if (connections->Find(getHashCode(env,clazz), &iface) == TCL_OK) {
	    return env->NewStringUTF(Tcl_GetString(iface->Nick));
	} else {
	    return 0L;
	}
    }

    static jstring JNICALL Java_Engine_user (JNIEnv *env, jclass clazz)
    {
	IRCUserScriptProviderForJavaPrivateImp *iface;

	if (connections->Find(getHashCode(env,clazz), &iface) == TCL_OK) {
	    return env->NewStringUTF(Tcl_GetString(iface->User));
	} else {
	    return 0L;
	}
    }

    static jstring JNICALL Java_Engine_host (JNIEnv *env, jclass clazz)
    {
	IRCUserScriptProviderForJavaPrivateImp *iface;

	if (connections->Find(getHashCode(env,clazz), &iface) == TCL_OK) {
	    return env->NewStringUTF(Tcl_GetString(iface->Host));
	} else {
	    return 0L;
	}
    }

    static jstring JNICALL Java_Engine_event (JNIEnv *env, jclass clazz)
    {
	IRCUserScriptProviderForJavaPrivateImp *iface;

	if (connections->Find(getHashCode(env,clazz), &iface) == TCL_OK) {
	    return env->NewStringUTF(Tcl_GetString(iface->Event));
	} else {
	    return 0L;
	}
    }

    static jobjectArray JNICALL Java_Engine_args (JNIEnv *env, jclass clazz)
    {
	IRCUserScriptProviderForJavaPrivateImp *iface;
	int size;
	Tcl_Obj *elem;
	jstring utf_str;
	jclass jStrC = env->FindClass("java/lang/String");

	if (connections->Find(getHashCode(env,clazz), &iface) == TCL_OK) {
	    Tcl_ListObjLength(0L, iface->Args, &size);
	    jobjectArray arr = env->NewObjectArray(size, jStrC, 0L);

	    for (int i = 0; i < size; i++)
	    {
		Tcl_ListObjIndex(0L, iface->Args, i, &elem);
  		utf_str = env->NewStringUTF(Tcl_GetString(elem));
		env->SetObjectArrayElement(arr, i, utf_str);
        	env->DeleteLocalRef(utf_str);
	    }
	    return arr;    
	} else {
	    return 0L;
	}
    }

    static jstring JNICALL Java_Engine_raw_args (JNIEnv *env, jclass clazz)
    {
	IRCUserScriptProviderForJavaPrivateImp *iface;

	if (connections->Find(getHashCode(env,clazz), &iface) == TCL_OK) {
	    return env->NewStringUTF(Tcl_GetString(iface->RawArgs));
	} else {
	    return 0L;
	}
    }

    static jstring JNICALL Java_Engine_raw_line (JNIEnv *env, jclass clazz)
    {
	IRCUserScriptProviderForJavaPrivateImp *iface;

	if (connections->Find(getHashCode(env,clazz), &iface) == TCL_OK) {
	    return env->NewStringUTF(Tcl_GetString(iface->RawLine));
	} else {
	    return 0L;
	}
    }

    static void JNICALL Java_Engine_complete (JNIEnv *env, jclass clazz)
    {
	IRCUserScriptProviderForJavaPrivateImp *iface;

	if (connections->Find(getHashCode(env,clazz), &iface) == TCL_OK) {
	    iface->Completed = IRCUserScriptProvider::COMPLETED;
	}
    }

    static void JNICALL Java_Engine_raw (JNIEnv *env, jclass clazz, jstring bytes)
    {
	IRCUserScriptProviderForJavaPrivateImp *iface;
	const char *byteStr;  // <- in utf-8
	jboolean isCopy;
	int result;

	if (connections->Find(getHashCode(env,clazz), &iface) == TCL_OK) {
	    byteStr = env->GetStringUTFChars(bytes, &isCopy);
	    result = Tcl::FastInvoke(iface->topInterp, iface->topInstance, "raw", byteStr, 0L);
	    if (isCopy == JNI_TRUE) env->ReleaseStringUTFChars(bytes, byteStr);
	    if (result != TCL_OK) {
		// TODO: How do we pass Tcl errors to Java ???
	    }
	}
    }
    static void JNICALL Java_Engine_connect (JNIEnv *env, jclass clazz)
    {
    }
    static void JNICALL Java_Engine_disconnect (JNIEnv *env, jclass clazz)
    {
    }
    static void JNICALL Java_Engine_connected (JNIEnv *env, jclass clazz)
    {
    }
    static void JNICALL Java_Engine_server (JNIEnv *env, jclass clazz)
    {
    }
    static jstring JNICALL Java_Engine_my_nick (JNIEnv *env, jclass clazz)
    {
	IRCUserScriptProviderForJavaPrivateImp *iface;
	int result;

	if (connections->Find(getHashCode(env,clazz), &iface) == TCL_OK) {
    	    result = Tcl::FastInvoke(iface->topInterp, iface->topInstance, "my_nick", 0L);
	    if (result != TCL_OK) {
		// TODO: How do we pass Tcl errors to Java ???
	    }
	    return env->NewStringUTF(Tcl_GetStringResult(iface->topInterp));
	} else {
	    return 0L;
	}
    }
    static void JNICALL Java_Engine_my_ip (JNIEnv *env, jclass clazz)
    {
    }
    static void JNICALL Java_Engine_my_host (JNIEnv *env, jclass clazz)
    {
    }
    static void JNICALL Java_Engine_my_user (JNIEnv *env, jclass clazz)
    {
    }
    static jint JNICALL Java_Engine_my_port (JNIEnv *env, jclass clazz)
    {
	IRCUserScriptProviderForJavaPrivateImp *iface;
	int result, port;

	if (connections->Find(getHashCode(env,clazz), &iface) == TCL_OK) {
    	    result = Tcl::FastInvoke(iface->topInterp, iface->topInstance, "my_port", 0L);
	    if (result != TCL_OK) {
		// TODO: How do we pass Tcl errors to Java ???
	    }
	    Tcl_GetIntFromObj(0L, Tcl_GetObjResult(iface->topInterp), &port);
	    return port;
	} else {
	    return 0;
	}
    }
    static jobjectArray JNICALL Java_Engine_channels (JNIEnv *env, jclass clazz)
    {
	IRCUserScriptProviderForJavaPrivateImp *iface;
	int size, result;
	Tcl_Obj *elem;
	jstring utf_str;
	jclass jStrC = env->FindClass("java/lang/String");

	if (connections->Find(getHashCode(env,clazz), &iface) == TCL_OK) {
    	    result = Tcl::FastInvoke(iface->topInterp, iface->topInstance, "channels", 0L);
	    if (result != TCL_OK) {
		// TODO: How do we pass Tcl errors to Java ???
	    }
	    Tcl_ListObjLength(0L, Tcl_GetObjResult(iface->topInterp), &size);
	    jobjectArray arr = env->NewObjectArray(size, jStrC, 0L);

	    for (int i = 0; i < size; i++)
	    {
		Tcl_ListObjIndex(0L, Tcl_GetObjResult(iface->topInterp), i, &elem);
  		utf_str = env->NewStringUTF(Tcl_GetString(elem));
		env->SetObjectArrayElement(arr, i, utf_str);
        	env->DeleteLocalRef(utf_str);
	    }
	    return arr;    
	} else {
	    return 0L;
	}
    }
    static void JNICALL Java_Engine_nicks (JNIEnv *env, jclass clazz)
    {
    }
    static void JNICALL Java_Engine_modes (JNIEnv *env, jclass clazz)
    {
    }
    static void JNICALL Java_Engine_whois (JNIEnv *env, jclass clazz)
    {
    }
    static void JNICALL Java_Engine_lookup (JNIEnv *env, jclass clazz)
    {
    }
    static void JNICALL Java_Engine_color (JNIEnv *env, jclass clazz)
    {
    }
    static void JNICALL Java_Engine_topic (JNIEnv *env, jclass clazz)
    {
    }
    static void JNICALL Java_Engine_echo (JNIEnv *env, jclass clazz)
    {
    }
    static void JNICALL Java_Engine_window (JNIEnv *env, jclass clazz)
    {
    }
    static void JNICALL Java_Engine_menu (JNIEnv *env, jclass clazz)
    {
    }
    static void JNICALL Java_Engine_hotkey (JNIEnv *env, jclass clazz)
    {
    }
    static void JNICALL Java_Engine_alias (JNIEnv *env, jclass clazz)
    {
    }
    static void JNICALL Java_Engine_channel (JNIEnv *env, jclass clazz)
    {
    }
    static void JNICALL Java_Engine_query (JNIEnv *env, jclass clazz)
    {
    }
    static void JNICALL Java_Engine_chat (JNIEnv *env, jclass clazz)
    {
    }
    static void JNICALL Java_Engine_queries (JNIEnv *env, jclass clazz)
    {
    }
    static void JNICALL Java_Engine_chats (JNIEnv *env, jclass clazz)
    {
    }
    static void JNICALL Java_Engine_say (JNIEnv *env, jclass clazz)
    {
    }
    static void JNICALL Java_Engine_input (JNIEnv *env, jclass clazz)
    {
    }

    static jint getHashCode(JNIEnv* env, jclass aClass)
    {
	jmethodID hcMID = env->GetMethodID(aClass, "hashCode", "()I");
	jint hashCode = env->CallIntMethod(aClass, hcMID);
	return hashCode;
    }

    static void JavaEventDeleteProc(void *)
    {
	// TODO: finish me.
    }

    typedef CPPTCL_USE_NAMESPACE(Tcl)Hash<IRCUserScriptProviderForJavaPrivateImp *,
	    TCL_ONE_WORD_KEYS> ConnectionHash;

    static ConnectionHash *connections;

    Java::Interp *javaPtr;
    int *status;

    Tcl_Obj *FullAddress;
    Tcl_Obj *Nick;
    Tcl_Obj *User;
    Tcl_Obj *Host;
    Tcl_Obj *Event;
    Tcl_Obj *Args;
    Tcl_Obj *RawArgs;
    Tcl_Obj *RawLine;
    IRCUserScriptProvider::CompletionStatus Completed;
    IRCEvents<jobject> Events;

    Tcl_Interp *topInterp;
    char *topInstance;

public:

    IRCUserScriptProviderForJavaPrivateImp(
	const char *scriptFileName,
	const char *instanceName,
	Tcl_Interp *globalInterp,
	int useSafe,
	int *_status)
	:
	status(_status),
	Events(JavaEventDeleteProc)
    {
	JNINativeMethod nativeMethods[] =
	{
	    // Local member functions
	    {"on",	    "(Ljava/lang/String;Ljava/lang/reflect/Method;)V", Java_Engine_on},
	    {"fulladdress", "()Ljava/lang/String",  Java_Engine_fulladdress},
	    {"nick",	    "()Ljava/lang/String",  Java_Engine_nick},
	    {"user",	    "()Ljava/lang/String",  Java_Engine_user},
	    {"host",	    "()Ljava/lang/String",  Java_Engine_host},
	    {"event",	    "()Ljava/lang/String",  Java_Engine_event},
	    {"args",	    "()[Ljava/lang/String", Java_Engine_args},
	    {"raw_args",    "()Ljava/lang/String",  Java_Engine_raw_args},
	    {"raw_line",    "()Ljava/lang/String",  Java_Engine_raw_line},
	    {"complete",    "()V",		    Java_Engine_complete},

	    // Aliased member functions
	    {"raw",	    "(Ljava/lang/String)V", Java_Engine_raw},
	    {"connect",	    "()V",		    Java_Engine_connect},
	    {"disconnect",  "()V",		    Java_Engine_disconnect},
	    {"connected",   "()V",		    Java_Engine_connected},
	    {"server",	    "()V",		    Java_Engine_server},
	    {"my_nick",	    "()Ljava/lang/String",  Java_Engine_my_nick},
	    {"my_ip",	    "()V",		    Java_Engine_my_ip},
	    {"my_host",	    "()V",		    Java_Engine_my_host},
	    {"my_user",	    "()V",		    Java_Engine_my_user},
	    {"my_port",	    "()I",		    Java_Engine_my_port},
	    {"channels",    "()[Ljava/lang/String", Java_Engine_channels},
	    {"nicks",	    "()V",		    Java_Engine_nicks},
	    {"modes",	    "()V",		    Java_Engine_modes},
	    {"whois",	    "()V",		    Java_Engine_whois},
	    {"lookup",	    "()V",		    Java_Engine_lookup},
	    {"color",	    "()V",		    Java_Engine_color},
	    {"topic",	    "()V",		    Java_Engine_topic},

	    // Assumed and aliased member functions
	    {"echo",	    "()V",		    Java_Engine_echo},
	    {"window",	    "()V",		    Java_Engine_window},
	    {"menu",	    "()V",		    Java_Engine_menu},
	    {"hotkey",	    "()V",		    Java_Engine_hotkey},
	    {"alias",	    "()V",		    Java_Engine_alias},
	    {"channel",	    "()V",		    Java_Engine_channel},
	    {"query",	    "()V",		    Java_Engine_query},
	    {"chat",	    "()V",		    Java_Engine_chat},
	    {"queries",	    "()V",		    Java_Engine_queries},
	    {"chats",	    "()V",		    Java_Engine_chats},
	    {"say",	    "()V",		    Java_Engine_say},
	    {"input",	    "()V",		    Java_Engine_input},
	    {0L, 0L, 0L}
	};
	
	if (connections == 0L) {
	    connections = new ConnectionHash;
	}

	topInterp = globalInterp;
	topInstance = strdup(instanceName);

	try {
    	    javaPtr = new Java::Interp(WriteStdOut, WriteStdErr);
	}
	catch (char *err) {
	    Tcl_SetObjResult(globalInterp, Tcl_NewStringObj(err, -1));
	    *status =  TCL_ERROR;
	    return;
	}

	javaPtr->setExtensions("IRC/Engine", nativeMethods);
    
	//while (objc--) perlPtr->???(Tcl_GetUnicode(objv[objc]));

	//if (javaPtr->Load(scriptFileName, &result) == 1) {
	//    Tcl_SetObjResult(globalInterp, Tcl_NewStringObj(result,-1));
	//    *status = TCL_ERROR;
	//}
    }

    ~IRCUserScriptProviderForJavaPrivateImp()
    {
	delete javaPtr;
    }

    IRCUserScriptProvider::CompletionStatus
    IncomingEvent(const IRCParse *line)
    {
	FullAddress = line->GetFullAddress();
	Nick = line->GetNick();
	User = line->GetUser();
	Host = line->GetHost();
	Event = line->GetEvent();
	Args = line->GetArgs();
	RawArgs = line->GetRawArgs();
	RawLine = line->GetRawLine();
	Completed = IRCUserScriptProvider::NOTCOMPLETED;

	// TODO: finish me!
	return IRCUserScriptProvider::NOTCOMPLETED;
    }

    IRCUserScriptProvider::CompletionStatus
	RunAlias(const IRCParse* line)
    {
	return NOTCOMPLETED;
    }

};

IRCUserScriptProviderForJavaPrivateImp::ConnectionHash *
IRCUserScriptProviderForJavaPrivateImp::connections = 0L;

IRCUserScriptProviderForJava::IRCUserScriptProviderForJava(
    const char *scriptFileName,
    const char *instanceName,
    Tcl_Interp *globalInterp,
    int useSafe)
    :
    IRCUserScriptProvider()
{
    _imp = new IRCUserScriptProviderForJavaPrivateImp(scriptFileName,
	    instanceName, globalInterp, useSafe, &status);
}

#endif
