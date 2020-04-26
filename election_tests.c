#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

#include "../election/election.h"
#include "../mtm_map/map.h"

#define SUPER_LONG_STRING                                                      \
    "why how impolite of him i asked him a civil question and he pretended "   \
    "not to hear me thats not at all nice calling after him i say mr white "   \
    "rabbit where are you going hmmm he wont answer me and i do so want to "   \
    "know what he is late for i wonder if i might follow him why not theres "  \
    "no rule that i maynt go where i please ii will follow him wait for me "   \
    "mr white rabbit im coming too falling how curious i never realized that " \
    "rabbit holes were so dark    and so long    and so empty i believe i "    \
    "have been falling for five minutes and i still cant see the bottom hmph " \
    "after such a fall as this i shall think nothing of tumbling downstairs "  \
    "how brave theyll all think me at home why i wouldnt say anything about "  \
    "it even if i fell off the top of the house i wonder how many miles ive "  \
    "fallen by this time i must be getting somewhere near the center of the "  \
    "earth i wonder if i shall fall right through the earth how funny that "   \
    "would be oh i think i see the bottom yes im sure i see the bottom i "     \
    "shall hit the bottom hit it very hard and oh how it will hurt"

#define ASSERT_TEST(expr)                                                 \
    do {                                                                  \
        if (!(expr)) {                                                    \
            printf("\nAssertion failed at %s:%d %s ", __FILE__, __LINE__, \
                   #expr);                                                \
            return false;                                                 \
        }                                                                 \
    } while (0)

#define TEST_WITH_SAMPLE(test, name)         \
    do {                                     \
        Election sample = getSampleData();   \
        printf("Running sub %s ... ", name); \
        if (test(sample)) {                  \
            printf("[OK]\n");                \
        } else {                             \
            printf("[Failed]\n");            \
            g_status = false;                \
        }                                    \
        cleanUp(sample);                     \
    } while (0)

bool isEven(int num) { return num % 2 ? false : true; }

static bool g_status = true;

Election getSampleData() {
    Election election = electionCreate();
    assert(election != NULL);
    electionAddTribe(election, 11, "TribeA");
    electionAddTribe(election, 12, "TribeB");
    electionAddTribe(election, 13, "TribeC");
    electionAddTribe(election, 14, "TribeD");
    electionAddTribe(election, 15, "TribeE");
    electionAddArea(election, 21, "AreaA");
    electionAddArea(election, 22, "AreaB");
    electionAddArea(election, 23, "AreaC");
    electionAddArea(election, 24, "AreaD");
    electionAddArea(election, 25, "AreaE");
    electionAddArea(election, 26, "AreaF");
    return election;
}

void cleanUp(Election election) { electionDestroy(election); }

/**
 * The subtests are defined here.
 * Each one targets a specific edge case
 */

bool subAddTribeInvalidId(Election sample) {
    ASSERT_TEST(electionAddTribe(sample, -1, "InvalidTribe") ==
                ELECTION_INVALID_ID);
    // Verify it wasn't added
    ASSERT_TEST(electionGetTribeName(sample, -1) == NULL);
    return true;
}

bool subAddTribeExist(Election sample) {
    // Existing ID
    ASSERT_TEST(electionAddTribe(sample, 11, "AlreadyExist") ==
                ELECTION_TRIBE_ALREADY_EXIST);
    // Existing Name
    ASSERT_TEST(electionAddTribe(sample, 1, electionGetTribeName(sample, 11)) ==
                ELECTION_SUCCESS);
    // Make sure names match
    ASSERT_TEST(strcmp(electionGetTribeName(sample, 1),
                       electionGetTribeName(sample, 11)) == 0);
    // Make sure the names are different instances
    ASSERT_TEST(electionGetTribeName(sample, 1) !=
                electionGetTribeName(sample, 11));

    return true;
}

bool subAddTribeLongName(Election sample) {
    ASSERT_TEST(electionAddTribe(sample, 1, SUPER_LONG_STRING) ==
                ELECTION_SUCCESS);
    ASSERT_TEST(electionGetTribeName(sample, 1) != NULL);
    ASSERT_TEST(strcmp(electionGetTribeName(sample, 1), SUPER_LONG_STRING) ==
                0);
    return true;
}

// END SUBTESTS

/**
 * The super tests are defined here.
 * Each includes multiple possible sub tests convering as many edge cases as
 * possible.
 */

void testCreate() {}
void testAddTribe() {
    printf("Testing %s tests...\n", "'Add Tribe'");
    TEST_WITH_SAMPLE(subAddTribeInvalidId, "Invalid Tribe ID");
    TEST_WITH_SAMPLE(subAddTribeLongName, "Long Tribe Name");
    TEST_WITH_SAMPLE(subAddTribeExist, "Pre Existing Tribe/TribeId");
    // TODO:
    // TODO:
    // TODO:
    // TODO:
    // TODO:
}
void testRemoveTribe() {}
void testAddArea() {}
void testRemoveArea() {}
void testRemoveAreas() {}
void testAddVote() {}
void testRemoveVote() {}
void testComputeAreasToTribesMapping() {}
void testSetTribeName() {}
void testGetTribeName() {}
void testDoomsDay() {
    // TODO: Stress Election with lots of adds and removes for both tribes and
    // areas
}
void testDestroy() {}

/*The functions for the tests should be added here*/
void (*tests[])(void) = {testCreate,
                         testAddTribe,
                         testRemoveTribe,
                         testAddArea,
                         testRemoveArea,
                         testRemoveAreas,
                         testAddVote,
                         testRemoveVote,
                         testSetTribeName,
                         testGetTribeName,
                         testComputeAreasToTribesMapping,
                         testDestroy,
                         testDoomsDay,
                         NULL};

int main(int argc, char* argv[]) {
    for (int test_idx = 0; tests[test_idx] != NULL; test_idx++) {
        tests[test_idx]();
    }
    if (g_status) {
        return 0;
    } else {
        return -1;
    }
}