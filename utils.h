#ifndef UTILS_H_
#define UTILS_H_

char *randString(int length);
char *randLowerString(int length);

/**
 * Will fail after X mallocs. To help simulate memory errors.
 * To easily use you can do in the beginning of your code:
 * #define malloc xmalloc
 * Before the unstable_malloc can fail you need to call it with:
 * size = -X. While X is malloc in which the allocation will fail.
 * To make it stable you need to call it with:
 * size = 0.
 * By default it will not fail and will act exactly like
 * malloc acts.
 */
void *xmalloc(long size);

char *tester_intToStr(int num);
#endif
