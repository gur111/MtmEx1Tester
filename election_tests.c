#include <assert.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "../election/election.h"
#include "../mtm_map/map.h"
#include "utils.h"
// Allow malloc to be unstable
#define malloc xmalloc

#define STRESS_INVERTALS_MODIFIER 10000

#ifdef __unix__
#define WITH_FORK
// Fuck Microsoft and all it stands for.
// If you need to debug on this shitty OS, get the errors one by one.
// Also, good luck. You'll need it
#endif

#ifdef __MACH__
//#define WITH_FORK
// Fuck Microsoft and all it stands for.
// If you need to debug on this shitty OS, get the errors one by one.
// Also, good luck. You'll need it
#endif

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

// Problematic with id 0
bool isCorrectArea(int area_id) {
    static int correct_area = -1;
    if (area_id < 0) {
        correct_area = area_id * (-1);
        return true;  // Return value doesn't matter here
    } else {
        assert(correct_area >= 0);
        return area_id == correct_area;
    }
}

AreaConditionFunction specificArea(int area_id) {
    assert(area_id >= 0);
    isCorrectArea((-1) * area_id);
    return isCorrectArea;
}

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
    ASSERT_TEST(electionAddTribe(sample, -1, "invalid tribe id") ==
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
    ASSERT_TEST(electionAddTribe(sample, 1, "Beginning") ==
                ELECTION_INVALID_NAME);
    ASSERT_TEST(electionAddTribe(sample, 1, "endinG") == ELECTION_INVALID_NAME);
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

// Checking on edge cases integers. Max, min and zero ad id
bool subAddTribeExtremeIdValues(Election sample) {
    ASSERT_TEST(electionAddTribe(sample, INT_MAX, "max int") ==
                ELECTION_SUCCESS);
    ASSERT_TEST(strcmp(electionGetTribeName(sample, INT_MAX), "max int") == 0);

    ASSERT_TEST(electionAddTribe(sample, 0, "zero id") == ELECTION_SUCCESS);
    ASSERT_TEST(strcmp(electionGetTribeName(sample, 0), "zero id") == 0);

    ASSERT_TEST(electionAddTribe(sample, INT_MIN, "min int") ==
                ELECTION_INVALID_ID);
    ASSERT_TEST(electionGetTribeName(sample, INT_MIN) == NULL);
    return true;
}

// Test removing and readding tribe
/**
 * sub tests for removing tribes.
 */
bool subRemoveTribeReadd(Election sample) {
    ASSERT_TEST(electionRemoveTribe(sample, 11) == ELECTION_SUCCESS);
    ASSERT_TEST(electionAddTribe(sample, 11, "re added") == ELECTION_SUCCESS);
    ASSERT_TEST(strcmp(electionGetTribeName(sample, 11), "re added") == 0);
    ASSERT_TEST(electionRemoveTribe(sample, 11) == ELECTION_SUCCESS);
    ASSERT_TEST(electionAddTribe(sample, 11, "and again") == ELECTION_SUCCESS);
    return true;
}

bool subRemoveTribeNullArgument(Election sample) {
    assert(electionRemoveTribe(NULL, 1) == ELECTION_NULL_ARGUMENT);
    return true;
}

bool subRemoveTribeInvalidId(Election sample) {
    assert(electionRemoveTribe(sample, -1) == ELECTION_INVALID_ID);
    assert(electionAddTribe(sample, 0, "adding id zero") == ELECTION_SUCCESS);
    assert(electionRemoveTribe(sample, 0) == ELECTION_SUCCESS);
    return true;
}

bool subRemoveTribeNotExist(Election sample) {
    assert(electionRemoveTribe(sample, 1) == ELECTION_TRIBE_NOT_EXIST);
    assert(electionRemoveTribe(sample, 11) == ELECTION_SUCCESS);
    assert(electionGetTribeName(sample, 11) == NULL);
    assert(electionRemoveTribe(sample, 11) == ELECTION_TRIBE_NOT_EXIST);
    return true;
}

bool subRemoveTribeWithVotes(Election sample) {
    assert(electionAddVote(sample, 21, 11, 3) == ELECTION_SUCCESS);
    assert(electionRemoveTribe(sample, 11) == ELECTION_SUCCESS);
    // todo: check with the map compute the result
    return true;
}

bool subRemoveTribeFirstMiddelLast(Election sample) {
    assert(electionRemoveTribe(sample, 11) == ELECTION_SUCCESS);
    assert(electionRemoveTribe(sample, 15) == ELECTION_SUCCESS);
    assert(electionRemoveTribe(sample, 13) == ELECTION_SUCCESS);
    return true;
}

/**
 * This test makes sure the string sent to tribe as name is copied and not
 * merely the same instance is used
 */
bool subAddTribeVerifyStringsDereferencing(Election sample) {
    char name[] = "some name";
    ASSERT_TEST(electionAddTribe(sample, 1, name) == ELECTION_SUCCESS);
    ASSERT_TEST(strcmp(electionGetTribeName(sample, 1), name) == 0);
    name[0] = 'a';
    ASSERT_TEST(strcmp(electionGetTribeName(sample, 1), name) != 0);
    ASSERT_TEST(electionGetTribeName(sample, 1) != name);

    return true;
}

bool subAddAreaInvalidId(Election sample) {
    ASSERT_TEST(electionAddArea(sample, -1, "invalid area") ==
                ELECTION_INVALID_ID);
    // Verify it wasn't added
    // ASSERT_TEST(electionGetAreaName(sample, -1) == NULL); TODO: What do we
    // check here?
    return true;
}

bool subAddAreaExist(Election sample) {
    // Existing ID
    ASSERT_TEST(electionAddArea(sample, 21, "id exist") ==
                ELECTION_AREA_ALREADY_EXIST);
    // Existing Name
    ASSERT_TEST(electionAddArea(sample, 1, "area a") == ELECTION_SUCCESS);
    // Make sure names match
    // TODO: How can we verify this
    // ASSERT_TEST(strcmp(electionGetAreaName(sample, 1),
    //                    electionGetAreaName(sample, 11)) == 0);
    // Make sure the names are different instances
    // ASSERT_TEST(electionGetAreaName(sample, 1) !=
    //             electionGetAreaName(sample, 11));

    return true;
}

bool subAddAreaLongName(Election sample) {
    ASSERT_TEST(electionAddArea(sample, 1, SUPER_LONG_STRING) ==
                ELECTION_SUCCESS);
    // TODO: What else can we verify here?
    // ASSERT_TEST(electionGetTribeName(sample, 1) != NULL);
    // ASSERT_TEST(strcmp(electionGetTribeName(sample, 1), SUPER_LONG_STRING) ==
    // 0);
    return true;
}

bool subAddAreaInvalidName(Election sample) {
    ASSERT_TEST(electionAddArea(sample, 1, "UPPER CASE INVALID") ==
                ELECTION_INVALID_NAME);
    ASSERT_TEST(electionAddArea(sample, 1, "names.with.dots.invalid") ==
                ELECTION_INVALID_NAME);
    ASSERT_TEST(electionAddArea(sample, 1, "hyphens-are-invalid-too") ==
                ELECTION_INVALID_NAME);
    ASSERT_TEST(electionAddArea(sample, 1, "underscores_as_well") ==
                ELECTION_INVALID_NAME);
    ASSERT_TEST(electionAddArea(sample, 1, "comma,is not valid") ==
                ELECTION_INVALID_NAME);
    ASSERT_TEST(electionAddArea(sample, 1, "exclamation!mark") ==
                ELECTION_INVALID_NAME);
    ASSERT_TEST(electionAddArea(sample, 1, "question?mark") ==
                ELECTION_INVALID_NAME);
    ASSERT_TEST(electionAddArea(sample, 1, "new\nline") ==
                ELECTION_INVALID_NAME);
    ASSERT_TEST(electionAddArea(sample, 1, "tab\tname") ==
                ELECTION_INVALID_NAME);
    ASSERT_TEST(electionAddArea(sample, 1, "bell\bname") ==
                ELECTION_INVALID_NAME);
    ASSERT_TEST(electionAddArea(sample, 1, "Beginning") ==
                ELECTION_INVALID_NAME);
    ASSERT_TEST(electionAddArea(sample, 1, "endinG") == ELECTION_INVALID_NAME);
    return true;
}

bool subAddAreaValidName(Election sample) {
    ASSERT_TEST(electionAddArea(sample, 1, "names with spaces") ==
                ELECTION_SUCCESS);
    ASSERT_TEST(electionAddArea(sample, 2, "nospaces") == ELECTION_SUCCESS);
    // TODO: How can we verify
    // ASSERT_TEST(strcmp(electionGetTribeName(sample, 2), "nospaces") == 0);
    // Check empty string
    ASSERT_TEST(electionAddArea(sample, 3, "") == ELECTION_SUCCESS);
    // ASSERT_TEST(strcmp(electionGetTribeName(sample, 3), "") == 0);
    // String with only space
    ASSERT_TEST(electionAddArea(sample, 4, " ") == ELECTION_SUCCESS);
    // ASSERT_TEST(strcmp(electionGetTribeName(sample, 4), " ") == 0);
    return true;
}

// Checking on edge cases integers. Max, min and zero ad id
bool subAddAreaExtremeIdValues(Election sample) {
    ASSERT_TEST(electionAddArea(sample, INT_MAX, "max int") ==
                ELECTION_SUCCESS);
    // TODO: We want to verify it was actually added. Maybe try adding a vote
    // ASSERT_TEST(strcmp(electionGetAreaName(sample, INT_MAX), "max int") ==
    // 0);

    ASSERT_TEST(electionAddArea(sample, 0, "zero id") == ELECTION_SUCCESS);
    // ASSERT_TEST(strcmp(electionGetTribeName(sample, 0), "zero id") == 0);

    ASSERT_TEST(electionAddArea(sample, INT_MIN, "min int") ==
                ELECTION_INVALID_ID);
    // ASSERT_TEST(electionGetAreaName(sample, INT_MIN) == NULL);
    return true;
}

// Test removing and ReAdding area
bool subRemoveAreaReAdd(Election sample) {
    ASSERT_TEST(electionRemoveAreas(sample, specificArea(21)) ==
                ELECTION_SUCCESS);
    ASSERT_TEST(electionAddArea(sample, 21, "re added") == ELECTION_SUCCESS);
    ASSERT_TEST(electionRemoveAreas(sample, specificArea(21)) ==
                ELECTION_SUCCESS);
    ASSERT_TEST(electionAddArea(sample, 21, "and again") == ELECTION_SUCCESS);
    return true;
}
/**
 * sub tests for adding votes.
 */
bool subAddVotesNullArgument(Election sample) {
    assert(electionAddVote(NULL, 21, 11, 1) == ELECTION_NULL_ARGUMENT);
    assert(electionAddVote(sample, 21, 11, 1) == ELECTION_SUCCESS);
    assert(electionRemoveVote(NULL, 22, 21, 1) == ELECTION_NULL_ARGUMENT);
    return true;
}

bool subStressAddRemoveRepeat(Election sample) {
    bool status = true;
    const int iterations = STRESS_INVERTALS_MODIFIER * 10;

    for (int i = 0; i < iterations; i++) {
        status = status && subRemoveTribeReadd(sample);
    }
    // TODO Add some votes. Can rely on computation test
    for (int i = 0; i < iterations; i++) {
        status = status && subRemoveAreaReadd(sample);
        // TODO: Add some votes
    }

    return status;
}

bool subStressAddThenRemove(Election sample) {
    bool status = true;
    const int iterations = STRESS_INVERTALS_MODIFIER / 20;
    for (int i = 0; i < iterations; i++) {
        ASSERT_TEST(electionAddArea(sample, i + 100, randLowerString(7)) ==
                    ELECTION_SUCCESS);
        ASSERT_TEST(electionAddTribe(sample, i + 100, randLowerString(7)) ==
                    ELECTION_SUCCESS);
    }
    // TODO Add some votes. Can rely on computation test
    for (int i = 0; i < iterations; i++) {
        ASSERT_TEST(electionRemoveAreas(sample, specificArea(i + 100)) ==
                    ELECTION_SUCCESS);
        ASSERT_TEST(electionRemoveTribe(sample, i + 100) == ELECTION_SUCCESS);
        // TODO: Add some votes
    }
    return status;
}

bool subAddVotesInvalidId(Election sample) {
    assert(electionAddVote(sample, -1, 11, 2) == ELECTION_INVALID_ID);
    assert(electionAddVote(sample, 21, -11, 2) == ELECTION_INVALID_ID);

    assert(electionAddArea(sample, 0, "zero area") == ELECTION_SUCCESS);
    assert(electionAddVote(sample, 0, 11, 2) == ELECTION_SUCCESS);
    assert(electionAddTribe(sample, 0, "zero tribe") == ELECTION_SUCCESS);
    assert(electionAddVote(sample, 21, 0, 3) == ELECTION_SUCCESS);

    assert(electionAddVote(sample, 11, 21, -1) == ELECTION_INVALID_VOTES);
    assert(electionAddVote(sample, 11, 21, 0) == ELECTION_INVALID_VOTES);

    assert(electionAddVote(sample, 21, 11, 8) == ELECTION_SUCCESS);
    assert(electionAddVote(sample, 21, 11, INT_MAX) == ELECTION_SUCCESS);
    return true;
}

bool subAddVotesNotExits(Election sample) {
    assert(electionAddVote(sample, 99, 11, 3) == ELECTION_AREA_NOT_EXIST);
    assert(electionAddVote(sample, 21, 99, 3) == ELECTION_TRIBE_NOT_EXIST);

    assert(electionAddVote(sample, 21, 11, 1) == ELECTION_SUCCESS);
    assert(electionAddVote(sample, 22, 12, 2) == ELECTION_SUCCESS);
    assert(electionRemoveAreas(sample, specificArea(21)) == ELECTION_SUCCESS);
    assert(electionAddVote(sample, 21, 11, 1) == ELECTION_AREA_NOT_EXIST);
    assert(electionRemoveTribe(sample, 12) == ELECTION_SUCCESS);
    assert(electionAddVote(sample, 25, 12, 5) == ELECTION_TRIBE_NOT_EXIST);
    return true;
}

/**
 * sub tests for removing  votes.
 */
bool subRemoveVotesInvalidId(Election sample) {
    assert(electionRemoveVote(sample, -1, 11, 2) == ELECTION_INVALID_ID);
    assert(electionRemoveVote(sample, 21, -11, 2) == ELECTION_INVALID_ID);

    assert(electionAddArea(sample, 0, "zero area") == ELECTION_SUCCESS);
    assert(electionRemoveVote(sample, 0, 11, 2) == ELECTION_SUCCESS);
    assert(electionAddTribe(sample, 0, "zero tribe") == ELECTION_SUCCESS);
    assert(electionRemoveVote(sample, 21, 0, 3) == ELECTION_SUCCESS);

    assert(electionRemoveVote(sample, 11, 21, -1) == ELECTION_INVALID_VOTES);
    assert(electionRemoveVote(sample, 11, 21, 0) == ELECTION_INVALID_VOTES);
    return true;
}

bool subRemoveVotesNonExisting(Election sample) {
    assert(electionAddVote(sample, 21, 11, 4) == ELECTION_SUCCESS);
    assert(electionRemoveVote(sample, 99, 11, 3) == ELECTION_AREA_NOT_EXIST);
    assert(electionRemoveVote(sample, 21, 99, 2) == ELECTION_TRIBE_NOT_EXIST);

    assert(electionRemoveTribe(sample, 11) == ELECTION_SUCCESS);
    assert(electionRemoveVote(sample, 21, 11, 2) == ELECTION_TRIBE_NOT_EXIST);
    assert(electionRemoveAreas(sample, specificArea(24)) == ELECTION_SUCCESS);
    assert(electionRemoveVote(sample, 24, 13, 3) == ELECTION_AREA_NOT_EXIST);
    return true;
}

/**
 * sub tests for Getting tribe names.
 */
 
bool subSetTribeNameNULLArgument(Election sample) {
    assert(electionSetTribeName(NULL, 11, "NULL ELECTION NAME") == ELECTION_NULL_ARGUMENT);
    assert(electionSetTribeName(sample, 11, NULL) == ELECTION_NULL_ARGUMENT);
    return true;
}

bool subSetTribeNameInvalidId(Election sample) {
    assert(electionAddTribe(sample, 0, "tribe id zero") == ELECTION_SUCCESS);
    assert(electionSetTribeName(sample, 0, "changing the name of a tribe id zero") == ELECTION_SUCCESS);
    assert(electionSetTribeName(sample, -1, "changing the name of a NEGATIVE tribe id ") == ELECTION_INVALID_ID);
    return true;
}

bool sudSetTribeNameTribeNotExits(Election sample) {
    assert(electionSetTribeName(sample, 99, "tribe id not exists") == ELECTION_TRIBE_NOT_EXIST);
    assert(electionRemoveTribe(sample, 11) == ELECTION_SUCCESS);
    assert(electionSetTribeName(sample, 11, "not exists") == ELECTION_TRIBE_NOT_EXIST);
    return true;
}

bool subSetTribeNameDifferentStrings(Election sample) {
    assert(electionSetTribeName(sample, 12, "exclamation point!") == ELECTION_INVALID_NAME);
    assert(electionSetTribeName(sample, 12, "{right bracket without space") == ELECTION_INVALID_NAME);
    assert(electionSetTribeName(sample, 12, "` Grave accent with space") == ELECTION_INVALID_NAME);
    assert(electionSetTribeName(sample, 12, "Grave` accent without space") == ELECTION_INVALID_NAME);
    assert(electionSetTribeName(sample, 12, "{ right bracket with space") == ELECTION_INVALID_NAME);
    assert(electionSetTribeName(sample, 12, "ALL UPPER CASE") == ELECTION_INVALID_NAME);
    assert(strcmp(electionGetTribeName(sample, 12), "ALL UPPER CASE") != 0);
    assert(electionSetTribeName(sample, 12, "normal string") == ELECTION_SUCCESS);
    assert(strcmp(electionGetTribeName(sample, 12), "normal string") == 0);

    return true;
}
/**
 * sub tests for Getting tribe names.
 */
bool subGetTribeNameNullArgument(Election sample) {
    assert(electionGetTribeName(NULL, 11) == NULL);
    return true;
}

bool subGetTribeNameComperingStrings(Election sample) {
    assert(strcmp(electionGetTribeName(sample, 11), "tribe a") == 0);
    assert(electionRemoveTribe(sample, 11) == ELECTION_SUCCESS);
    assert(electionGetTribeName(sample, 11) == NULL);
    assert(electionAddTribe(sample, 11, "new tribe") == ELECTION_SUCCESS);
    assert(strcmp(electionGetTribeName(sample, 11), "new tribe") == 0);
    return true;
}
/**
 * sub tests for creating new map.
 */

bool subtestCreateNullMap(Election sample) {
    /**
    Election sample = electionCreate();
    AUG_MAP_FOREACH(key,sample.tribes){
        assert(key == NULL);
    }
    AUG_MAP_FOREACH(key,sample.areas){
        assert(key == NULL);
    }
    AUG_MAP_FOREACH(key,sample.votes_by_area){
        assert(key == NULL);
    }*/
    return true;
}
// END SUBTESTS

/**
 * The super tests are defined here.
 * Each includes multiple possible sub tests convering as many edge cases as
 * possible.
 */

void testCreate() {
    printf("Testing %s tests:\n", "'Create Map'");
    TEST_WITH_SAMPLE(subtestCreateNullMap, "checkin if new election is empty");
}

void testAddTribe() {
    printf("Testing %s tests:\n", "'Add Tribe'");
    TEST_WITH_SAMPLE(subAddTribeInvalidId, "Invalid Tribe ID");
    TEST_WITH_SAMPLE(subAddTribeLongName, "Long Tribe Name");
    TEST_WITH_SAMPLE(subAddTribeExist, "Pre Existing Tribe/TribeId");
    TEST_WITH_SAMPLE(subAddTribeInvalidName, "Invalid Tribe Names");
    TEST_WITH_SAMPLE(subAddTribeValidName, "Valid Tribe Names");
    TEST_WITH_SAMPLE(subAddTribeVerifyStringsDereferencing,
                     "Dereferencing String Tribe Name");
    TEST_WITH_SAMPLE(subAddTribeExtremeIdValues,
                     "Verify Tribe Extreme Id Values");
}

void testRemoveTribe() {
    printf("Testing %s tests:\n", "'Remove Tribe'");
    TEST_WITH_SAMPLE(subRemoveTribeReadd, "Re-Adding Tribe");
    TEST_WITH_SAMPLE(subRemoveTribeNullArgument, "Inserting Null argument");
    TEST_WITH_SAMPLE(subRemoveTribeInvalidId, "Inserting Invalid Id");
    TEST_WITH_SAMPLE(subRemoveTribeNotExist, "Removing a non existing tribe");
    TEST_WITH_SAMPLE(subRemoveTribeWithVotes, "adding votes");
    TEST_WITH_SAMPLE(subRemoveTribeFirstMiddelLast, "removing from the top, bottom, middle list");
}

void testAddArea() {
    printf("Testing %s tests:\n", "'Add Area'");
    TEST_WITH_SAMPLE(subAddAreaInvalidId, "Invalid Area ID");
    TEST_WITH_SAMPLE(subAddAreaLongName, "Long Area Name");
    TEST_WITH_SAMPLE(subAddAreaExist, "Pre Existing Area/AreaId");
    TEST_WITH_SAMPLE(subAddAreaInvalidName, "Invalid Area Names");
    // TODO: Add this case
    TEST_WITH_SAMPLE(subAddAreaValidName, "Valid Area Names");
    // TEST_WITH_SAMPLE(subAddAreaVerifyStringsDereferencing,
    //                  "Dereferencing String Area Name");
    TEST_WITH_SAMPLE(subAddAreaExtremeIdValues,
                     "Verify Area Extreme Id Values");
}

void testRemoveArea() {
    printf("Testing %s tests:\n", "'Remove Area'");
    TEST_WITH_SAMPLE(subRemoveAreaReAdd, "Re-Adding Area");
}

void testRemoveAreas() {}

void testAddVote() {
    TEST_WITH_SAMPLE(subAddVotesNullArgument, "Inserting Null argument");
    TEST_WITH_SAMPLE(subAddVotesInvalidId, "Inserting Invalid Id");
    TEST_WITH_SAMPLE(subAddVotesNotExits,
                     "Inserting non existing areas and tribes");
}

void testRemoveVote() {
    TEST_WITH_SAMPLE(subRemoveVotesInvalidId, "Inserting Invalid Id");
    TEST_WITH_SAMPLE(subRemoveVotesNonExisting,
                     "Non Existing tribes and areas");
}

void testComputeAreasToTribesMapping() {}

void testSetTribeName() {
    TEST_WITH_SAMPLE(subSetTribeNameDifferentStrings, "trying to insert different strings");
    TEST_WITH_SAMPLE(subSetTribeNameInvalidId, "inserting invalid id`s");
    TEST_WITH_SAMPLE(subSetTribeNameNULLArgument, "inserting null arguments");
    TEST_WITH_SAMPLE(sudSetTribeNameTribeNotExits, "trying to set name for not existing tribe");
}

void testGetTribeName() {
    TEST_WITH_SAMPLE(subGetTribeNameNullArgument, "inserting null arguments");
    TEST_WITH_SAMPLE(subGetTribeNameComperingStrings, "compering strings");
}

void testDoomsDay() {
    // TODO: Stress Election with lots of adds and removes for both tribes and
    // areas
    printf("Testing %s tests:\n", "'Dooms` Day'");
    TEST_WITH_SAMPLE(subStressAddRemoveRepeat, "Rapid Add and Remove");
    TEST_WITH_SAMPLE(subStressAddThenRemove, "Fill Up Then Clear One By One");
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

int main(int argc, char *argv[]) {
#ifdef WITH_FORK
    pid_t pid;
    int exit_code;
#endif
    for (int test_idx = 0; tests[test_idx] != NULL; test_idx++) {
#ifdef WITH_FORK
        pid = fork();

        if (pid < 0) {
            fprintf(stderr, "Forking process failed for test index %d\n",
                    test_idx);
        } else if (pid == 0) {
            // We're in the subprocess
            tests[test_idx]();
            // We don't want to continue the loop after the current test in the
            // subprocess. The main process will do it anyways
            return g_status == true ? 0 : 1;
        } else {
            // We're in the parrent process
            if (waitpid(pid, &exit_code, 0) != pid) {
                exit_code = -1;
            }
            if (exit_code != 0) {
                g_status = false;
            }
        }
#else
        tests[test_idx]();
#endif
    }

    if (g_status) {
        printf("All tests finishes successfully\n");
        return 0;
    } else {
        fprintf(stderr,
                "One or more tests have failed. See above log for more "
                "information.\n");
        return 1;
    }
}
