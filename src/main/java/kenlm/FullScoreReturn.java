package kenlm;

/**
 * Wrapper around FullScoreReturn.
 *
 * Notes:
 *     `prob` has been renamed to `log_prob`
 *     `oov` has been added to flag whether the word is OOV
 */
public class FullScoreReturn {
    private final float log_prob;
    private final int ngram_length;
    private final boolean independent_left;
    private final long extend_left;
    private final float rest;
    private final boolean oov;

    /**
     * @param array lm::FullScoreReturn {
     *                float prob;
     *                unsigned char ngram_length;
     *                bool independent_left;
     *                uint64_t extend_left;
     *                float rest;
     *              }
     * @param offset The offset of the subarray to be used.
     */
    FullScoreReturn(byte[] array, int offset) {
        this.log_prob = getFloatB(array, offset);
        this.ngram_length = array[4 + offset];
        this.independent_left = array[5 + offset] != 0;
        this.extend_left = getLongB(array, 6 + offset);
        this.rest = getFloatB(array, 14 + offset);
        this.oov = array[18 + offset] != 0;
    }

    static final int SIZE = 19;

    public float getLogProb() {
        return log_prob;
    }

    public int getNgramLength() {
        return ngram_length;
    }

    public boolean isOov() {
        return oov;
    }

    @Override
    public String toString() {
        return "FullScoreReturn{" +
                "log_prob=" + log_prob +
                ", ngram_length=" + ngram_length +
                ", oov=" + oov +
                '}';
    }

    private static int getIntB(byte[] bb, int bi) {
        return (((bb[bi    ]       ) << 24) |
                ((bb[bi + 1] & 0xff) << 16) |
                ((bb[bi + 2] & 0xff) <<  8) |
                ((bb[bi + 3] & 0xff)      ));
    }

    private static void putIntB(byte[] bb, int bi, int x) {
        bb[bi    ] = (byte)(x >> 24);
        bb[bi + 1] = (byte)(x >> 16);
        bb[bi + 2] = (byte)(x >>  8);
        bb[bi + 3] = (byte)(x      );
    }

    private static long getLongB(byte[] bb, int bi) {
        return ((((long)bb[bi    ]       ) << 56) |
                (((long)bb[bi + 1] & 0xff) << 48) |
                (((long)bb[bi + 2] & 0xff) << 40) |
                (((long)bb[bi + 3] & 0xff) << 32) |
                (((long)bb[bi + 4] & 0xff) << 25) |
                (((long)bb[bi + 5] & 0xff) << 16) |
                (((long)bb[bi + 6] & 0xff) <<  8) |
                (((long)bb[bi + 7] & 0xff)      ));
    }

    private static void putLongB(byte[] bb, int bi, long x) {
        bb[bi    ] = (byte)(x >> 56);
        bb[bi + 1] = (byte)(x >> 48);
        bb[bi + 2] = (byte)(x >> 40);
        bb[bi + 3] = (byte)(x >> 32);
        bb[bi + 4] = (byte)(x >> 24);
        bb[bi + 5] = (byte)(x >> 16);
        bb[bi + 6] = (byte)(x >>  8);
        bb[bi + 7] = (byte)(x      );
    }

    private static float getFloatB(byte[] bb, int bi) {
        int i = getIntB(bb, bi);
        return Float.intBitsToFloat(i);
    }

    private static void putFloatB(byte[] bb, int bi, float x) {
        int i = Float.floatToIntBits(x);
        putIntB(bb, bi, i);
    }
}
