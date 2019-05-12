package kenlm;

import sun.misc.Cleaner;

import static kenlm.KenlmJNI.*;

/**
 * Wrapper around lm::ngram::State so that python code can make incremental queries.
 *
 * Notes:
 *     rich comparisons
 *     hashable
 */
public class State implements Comparable<State> {

    private long _c_state;

    private final Cleaner cleaner;

    public State() {
        this._c_state = StateNew();
        this.cleaner = Cleaner.create(this, new Deallocator(_c_state));
    }

    @Override
    public int compareTo(State o) {
        return StateCompare(_c_state, o._c_state);
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        State state = (State) o;
        return StateEquals(_c_state, state._c_state);
    }

    @Override
    public int hashCode() {
        return (int) StateHashValue(_c_state);
    }

    public static void swap(State s1, State s2) {
        long tmp = s1._c_state;
        s1._c_state = s2._c_state;
        s2._c_state = tmp;
    }

    long getCState() {
        return _c_state;
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
            StateDelete(address);
        }
    }
}
