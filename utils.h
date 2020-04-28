#ifndef UTILS_H_

char *randString(int length);
char *randLowerString(int length);

/**
 * Will randomly fail. To help simulate memory errors.
 * To easily use you can do in the beginning of your code:
 * #define malloc xmalloc
 * Before the unstable_malloc can fail you need to * call it with size = -1.
 * To make it stable you need to call it with size = -2.
 * By default it will not fail and will act exactly like malloc acts.
 */
void *xmalloc(size_t size);
#endif