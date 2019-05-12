package kenlm;

import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;

class CleanerUtil {

    private static Object CLEANER;
    private static Method METHOD;

    static {
        try {
            // JDK 9+
            Class<?> clz = Class.forName("java.lang.ref.Cleaner");
            METHOD = clz.getMethod("register", Object.class, Runnable.class);
            CLEANER = clz.getMethod("create").invoke(null);
        } catch (ClassNotFoundException e) {
            try {
                // JDK 6+
                Class<?> clz = Class.forName("sun.misc.Cleaner");
                METHOD = clz.getMethod("create", Object.class, Runnable.class);
            } catch (ClassNotFoundException | NoSuchMethodException ex) {
                throw new Error(e);
            }
        } catch (NoSuchMethodException | IllegalAccessException | InvocationTargetException e) {
            throw new Error(e);
        }
    }

    static void createAndRegister(Object obj, Runnable action) {
        try {
            METHOD.invoke(CLEANER, obj, action);
        } catch (IllegalAccessException | InvocationTargetException e) {
            throw new Error(e);
        }
    }
}
