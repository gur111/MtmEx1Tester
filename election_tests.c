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
    assert(electionAddTribe(election, 11, "tribe a") == ELECTION_SUCCESS);
    assert(electionAddTribe(election, 12, "tribe b") == ELECTION_SUCCESS);
    assert(electionAddTribe(election, 13, "tribe c") == ELECTION_SUCCESS);
    assert(electionAddTribe(election, 14, "tribe d") == ELECTION_SUCCESS);
    assert(electionAddTribe(election, 15, "tribe e") == ELECTION_SUCCESS);
    assert(electionAddArea(election, 21, "area a") == ELECTION_SUCCESS);
    assert(electionAddArea(election, 22, "area b") == ELECTION_SUCCESS);
    assert(electionAddArea(election, 23, "area c") == ELECTION_SUCCESS);
    assert(electionAddArea(election, 24, "area d") == ELECTION_SUCCESS);
    assert(electionAddArea(election, 25, "area e") == ELECTION_SUCCESS);
    assert(electionAddArea(election, 26, "area f") == ELECTION_SUCCESS);
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
    ASSERT_TEST(electionAddTribe(sample, 11, "already exist") ==
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

bool subAddTribeInvalidName(Election sample) {
    ASSERT_TEST(electionAddTribe(sample, 1, "UPPER CASE INVALID") ==
                ELECTION_INVALID_NAME);
    ASSERT_TEST(electionAddTribe(sample, 1, "names.with.dots.invalid") ==
                ELECTION_INVALID_NAME);
    ASSERT_TEST(electionAddTribe(sample, 1, "hyphens-are-invalid-too") ==
                ELECTION_INVALID_NAME);
    ASSERT_TEST(electionAddTribe(sample, 1, "underscores_as_well") ==
                ELECTION_INVALID_NAME);
    ASSERT_TEST(electionAddTribe(sample, 1, "comma,is not valid") ==
                ELECTION_INVALID_NAME);
    ASSERT_TEST(electionAddTribe(sample, 1, "exclamation!mark") ==
                ELECTION_INVALID_NAME);
    ASSERT_TEST(electionAddTribe(sample, 1, "question?mark") ==
                ELECTION_INVALID_NAME);
    ASSERT_TEST(electionAddTribe(sample, 1, "new\nline") ==
                ELECTION_INVALID_NAME);
    ASSERT_TEST(electionAddTribe(sample, 1, "tab\tname") ==
                ELECTION_INVALID_NAME);
    ASSERT_TEST(electionAddTribe(sample, 1, "bell\bname") ==
                ELECTION_INVALID_NAME);
    return true;
}

bool subAddTribeValidName(Election sample) {
    ASSERT_TEST(electionAddTribe(sample, 1, "names with spaces") ==
                ELECTION_SUCCESS);
    ASSERT_TEST(electionAddTribe(sample, 2, "nospaces") == ELECTION_SUCCESS);
    ASSERT_TEST(strcmp(electionGetTribeName(sample, 2), "nospaces") == 0);
    // Check empty string
    ASSERT_TEST(electionAddTribe(sample, 3, "") == ELECTION_SUCCESS);
    ASSERT_TEST(strcmp(electionGetTribeName(sample, 3), "") == 0);
    // String with only space
    ASSERT_TEST(electionAddTribe(sample, 4, " ") == ELECTION_SUCCESS);
    ASSERT_TEST(strcmp(electionGetTribeName(sample, 4), " ") == 0);
    return true;
}

// TODO: Think of a better name
/**
 * This test makes sure the string sent to tribe as name is copied and not
 * merely the same instance is used
 */
bool subAddTribeCopyString(Election sample) {
    char name[] = "some name";
    ASSERT_TEST(electionAddTribe(sample, 1, name) == ELECTION_SUCCESS);
    ASSERT_TEST(strcmp(electionGetTribeName(sample, 1), name) == 0);
    name[0] = 'a';
    ASSERT_TEST(strcmp(electionGetTribeName(sample, 1), name) != 0);
    ASSERT_TEST(electionGetTribeName(sample, 1) != name);

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
    TEST_WITH_SAMPLE(subAddTribeInvalidName, "Invalid Tribe Names");
    TEST_WITH_SAMPLE(subAddTribeValidName, "Valid Tribe Names");
    TEST_WITH_SAMPLE(subAddTribeCopyString, "Dereferencing String Tribe Name");
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