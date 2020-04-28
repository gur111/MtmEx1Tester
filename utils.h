#ifndef UTILS_H_

char *randString(int length);
char *randLowerString(int length);

enum { XMALLOC_MAKE_UNSTABLE = -1, XMALLOC_MAKE_STABLE = -2 };
/**
 * Will randomly fail. To help simulate memory errors.
 * To easily use you can do in the beginning of your code:
 * #define malloc xmalloc
 * Before the unstable_malloc can fail you need to call it with:
 * size = XMALLOC_MAKE_UNSTABLE.
 * To make it stable you need to call it with:
 * size = XMALLOC_MAKE_STABLE.
 * By default it will not fail and will act exactly like
 * malloc acts.
 */
void *xmalloc(size_t size);
#endif