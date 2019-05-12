package kenlm;

import sun.misc.Cleaner;

import static kenlm.KenlmJNI.*;


/**
 * Wrapper around lm::ngram::Config.
 * Pass this to Model's constructor to set the load_method.
 */
public class Config {

    private final long _c_config;

    private final Cleaner cleaner;

    public Config() {
        this._c_config = ConfigNew();
        this.cleaner = Cleaner.create(this, new Deallocator(_c_config));
    }

    public LoadMethod getLoadMethod() {
        return LoadMethod.values()[ConfigGetLoadMethod(_c_config)];
    }

    public void setLoadMethod(LoadMethod loadMethod) {
        ConfigSetLoadMethod(_c_config, loadMethod.ordinal());
    }

    long getCConfig() {
        return _c_config;
    }

    private static class Deallocator implements Runnable {

        private long address;

        private Deallocator(long address) {
            assert (address != 0);
            this.address = address;
        }

        @Override
        public void run() {
            if (address == 0) {
                // Paranoia
                return;
            }
            ConfigDelete(address);
        }
    }
}