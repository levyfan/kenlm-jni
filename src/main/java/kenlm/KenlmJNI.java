package kenlm;

import java.io.IOException;

class KenlmJNI {

    static {
        try {
            System.load(NativeLibLoader.createTempFileFromResource("/" + System.mapLibraryName("kenlm_jni")));
        } catch (IOException e) {
            throw new UnsatisfiedLinkError(e.getMessage());
        }
    }

    static native long ConfigNew();
    static native void ConfigDelete(long c);
    static native int ConfigGetLoadMethod(long c);
    static native void ConfigSetLoadMethod(long c, int load_method);

    static native long ModelLoadVirtual(String path, long config);
    static native void ModelDelete(long model);

    static native void ModelBeginSentenceWrite(long model, long state);
    static native void ModelNullContextWrite(long model, long state);

    static native float ModelBaseScore(long model, long state, String word, long out_state);
    static native byte[] ModelBaseFullScore(long model, long state, String word, long out_state);

    static native float ModelScore(long model, String sentence, boolean bos, boolean eos);
    static native double ModelPerplexity(long model, String sentence);
    static native byte[] ModelFullScores(long model, String sentence, boolean bos, boolean eos);

    static native long StateNew();
    static native void StateDelete(long s);
    static native int StateCompare(long s1, long s2);
    static native boolean StateEquals(long s1, long s2);
    static native long StateHashValue(long s);
}
