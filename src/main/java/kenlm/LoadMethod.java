package kenlm;

public enum LoadMethod {
    // mmap with no prepopulate
    LAZY,
    // On linux, pass MAP_POPULATE to mmap.
    POPULATE_OR_LAZY,
    // Populate on Linux.  malloc and read on non-Linux.
    POPULATE_OR_READ,
    // malloc and read.
    READ,
    // malloc and read in parallel (recommended for Lustre)
    PARALLEL_READ,
}
