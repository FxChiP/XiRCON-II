/*-----------------------------------------------------------------------
 *  Engine.java --
 *
 *  Java source file that defines the hooks we place into the JVM and
 *  are filled at run-time.
 *
 *-----------------------------------------------------------------------
 * RCS: @(#) $Id: Engine.java,v 1.2 2002/08/25 03:46:08 davygrvy Exp $
 *-----------------------------------------------------------------------
 */
package IRC;

public class Engine {

    // Local calls in this JVM.
    //
    public static native void on(String event, java.lang.reflect.Method callBack);
    public static native String fulladdress();
    public static native String nick();
    public static native String user();
    public static native String host();
    public static native String event();
    public static native String[] args();
    public static native String raw_args();
    public static native String raw_line();
    public static native void complete();

    // Aliases up to the connection instance.
    //
    public static native void raw(String bytes);
    public static native void connect();
    public static native void disconnect();
    public static native boolean connected();
    public static native void server();
    public static native void my_nick();
    public static native void my_ip();
    public static native void my_host();
    public static native void my_user();
    public static native void my_port();
    public static native void channels();
    public static native void nicks();
    public static native void modes();
    public static native void whois();
    public static native void lookup();
    public static native String color();
    public static native void topic();

    // Aliases assumed in the UI across the connection instance.
    //
    public static native void echo();
    public static native void window();
    public static native void menu();
    public static native void hotkey();
    public static native void alias();
    public static native void channel();
    public static native void query();
    public static native void chat();
    public static native void queries();
    public static native void chats();
    public static native void say();
    public static native void input();
}
