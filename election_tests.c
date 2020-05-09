#include <assert.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "../election.h"
#include "utils.h"
// Allow malloc to be unstable
#define malloc xmalloc

#define STRESS_INVERTALS_MODIFIER 10000

#ifdef __unix__
#define WITH_FORK
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
// Fuck Microsoft and all it stands for.
// If you need to debug on this shitty OS, get the errors one by one.
// Also, good luck. You'll need it
#endif

#ifdef __MACH__
#define WITH_FORK
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
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

#define ASSERT_TEST(expr)                                                  \
    do {                                                                   \
        if (!(expr)) {                                                     \
            fprintf(stderr, "\nAssertion failed at %s:%d %s \n", __FILE__, \
                    __LINE__, #expr);                                      \
            return false;                                                  \
        }                                                                  \
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

bool subComputePrecedence(Election sample);
bool subComputeNoVotes(Election sample);

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
    char *res = electionGetTribeName(sample, -1);
    ASSERT_TEST(res == NULL);
    free(res);
    return true;
}

bool subAddTribeExist(Election sample) {
    // Existing ID
    ASSERT_TEST(electionAddTribe(sample, 11, "already exist") ==
                ELECTION_TRIBE_ALREADY_EXIST);
    // Existing Name
    char *tribe_name = electionGetTribeName(sample, 11);
    ASSERT_TEST(electionAddTribe(sample, 1, tribe_name) == ELECTION_SUCCESS);
    free(tribe_name);

    // Make sure names match
    tribe_name = electionGetTribeName(sample, 1);
    char *tribe_name2 = electionGetTribeName(sample, 11);
    ASSERT_TEST(strcmp(tribe_name, tribe_name2) == 0);
    free(tribe_name);
    free(tribe_name2);

    // Make sure the names are different instances
    tribe_name = electionGetTribeName(sample, 1);
    tribe_name2 = electionGetTribeName(sample, 11);
    ASSERT_TEST(tribe_name != tribe_name2);
    free(tribe_name);
    free(tribe_name2);
    return true;
}

bool subAddTribeLongName(Election sample) {
    ASSERT_TEST(electionAddTribe(sample, 1, SUPER_LONG_STRING) ==
                ELECTION_SUCCESS);
    char *tribe_name = electionGetTribeName(sample, 1);
    ASSERT_TEST(tribe_name != NULL);
    ASSERT_TEST(strcmp(tribe_name, SUPER_LONG_STRING) == 0);
    free(tribe_name);
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
    char *tribe_name = electionGetTribeName(sample, 2);
    ASSERT_TEST(strcmp(tribe_name, "nospaces") == 0);
    free(tribe_name);

    // Check empty string
    ASSERT_TEST(electionAddTribe(sample, 3, "") == ELECTION_SUCCESS);
    tribe_name = electionGetTribeName(sample, 3);
    ASSERT_TEST(strcmp(tribe_name, "") == 0);
    free(tribe_name);

    // String with only space
    ASSERT_TEST(electionAddTribe(sample, 4, " ") == ELECTION_SUCCESS);
    tribe_name = electionGetTribeName(sample, 4);
    ASSERT_TEST(strcmp(tribe_name, " ") == 0);
    free(tribe_name);
    return true;
}

// Checking on edge cases integers. Max, min and zero ad id
bool subAddTribeExtremeIdValues(Election sample) {
    ASSERT_TEST(electionAddTribe(sample, INT_MAX, "max int") ==
                ELECTION_SUCCESS);
    char *tribe_name = electionGetTribeName(sample, INT_MAX);
    ASSERT_TEST(strcmp(tribe_name, "max int") == 0);
    free(tribe_name);

    ASSERT_TEST(electionAddTribe(sample, 0, "zero id") == ELECTION_SUCCESS);
    tribe_name = electionGetTribeName(sample, 0);
    ASSERT_TEST(strcmp(tribe_name, "zero id") == 0);
    free(tribe_name);

    ASSERT_TEST(electionAddTribe(sample, INT_MIN, "min int") ==
                ELECTION_INVALID_ID);
    tribe_name = electionGetTribeName(sample, INT_MIN);
    ASSERT_TEST(tribe_name == NULL);
    free(tribe_name);
    return true;
}

bool subAddTribeErrorPrecedence(Election sample) {
    ASSERT_TEST(electionAddTribe(NULL, -1, "FSFS!!") == ELECTION_NULL_ARGUMENT);
    ASSERT_TEST(electionAddTribe(sample, -1, "FSFS!!") == ELECTION_INVALID_ID);
    ASSERT_TEST(electionAddTribe(sample, 11, "FSFS!!") ==
                ELECTION_TRIBE_ALREADY_EXIST);
    ASSERT_TEST(electionAddTribe(sample, 99, "FSFS!!") ==
                ELECTION_INVALID_NAME);
    return true;
}
// Test removing and readding tribe
/**
 * sub tests for removing tribes.
 */
bool subRemoveTribeReadd(Election sample) {
    ASSERT_TEST(electionRemoveTribe(sample, 11) == ELECTION_SUCCESS);
    ASSERT_TEST(electionAddTribe(sample, 11, "re added") == ELECTION_SUCCESS);
    char *tribe_name = electionGetTribeName(sample, 11);
    ASSERT_TEST(strcmp(tribe_name, "re added") == 0);
    ASSERT_TEST(electionRemoveTribe(sample, 11) == ELECTION_SUCCESS);
    ASSERT_TEST(electionAddTribe(sample, 11, "and again") == ELECTION_SUCCESS);
    free(tribe_name);
    return true;
}

bool subRemoveTribeNullArgument(Election sample) {
    ASSERT_TEST(electionRemoveTribe(NULL, 1) == ELECTION_NULL_ARGUMENT);
    return true;
}

bool subRemoveTribeInvalidId(Election sample) {
    ASSERT_TEST(electionRemoveTribe(sample, -1) == ELECTION_INVALID_ID);
    ASSERT_TEST(electionAddTribe(sample, 0, "adding id zero") ==
                ELECTION_SUCCESS);
    ASSERT_TEST(electionRemoveTribe(sample, 0) == ELECTION_SUCCESS);
    return true;
}

bool subRemoveTribeNotExist(Election sample) {
    ASSERT_TEST(electionRemoveTribe(sample, 1) == ELECTION_TRIBE_NOT_EXIST);
    ASSERT_TEST(electionRemoveTribe(sample, 11) == ELECTION_SUCCESS);
    char *tribe_name = electionGetTribeName(sample, 11);
    ASSERT_TEST(tribe_name == NULL);
    ASSERT_TEST(electionRemoveTribe(sample, 11) == ELECTION_TRIBE_NOT_EXIST);
    free(tribe_name);
    return true;
}

bool subRemoveTribeWithVotes(Election sample) {
    ASSERT_TEST(electionAddVote(sample, 21, 11, 3) == ELECTION_SUCCESS);
    ASSERT_TEST(electionRemoveTribe(sample, 11) == ELECTION_SUCCESS);
    // todo: check with the map compute the result
    return true;
}

bool subRemoveTribeFirstMiddelLast(Election sample) {
    ASSERT_TEST(electionRemoveTribe(sample, 11) == ELECTION_SUCCESS);
    ASSERT_TEST(electionRemoveTribe(sample, 15) == ELECTION_SUCCESS);
    ASSERT_TEST(electionRemoveTribe(sample, 13) == ELECTION_SUCCESS);
    return true;
}

bool subRemoveTribeErrorPrecedence(Election sample) {
    ASSERT_TEST(electionRemoveTribe(NULL, -1) == ELECTION_NULL_ARGUMENT);
    ASSERT_TEST(electionRemoveTribe(sample, -1) == ELECTION_INVALID_ID);
    ASSERT_TEST(electionRemoveTribe(sample, 99) == ELECTION_TRIBE_NOT_EXIST);
    return true;
}

/**
 * This test makes sure the string sent to tribe as name is copied and not
 * merely the same instance is used
 */
bool subAddTribeVerifyStringsDereferencing(Election sample) {
    char name[] = "some name";
    ASSERT_TEST(electionAddTribe(sample, 1, name) == ELECTION_SUCCESS);
    char *tribe_name = electionGetTribeName(sample, 1);
    ASSERT_TEST(strcmp(tribe_name, name) == 0);
    name[0] = 'a';
    ASSERT_TEST(strcmp(tribe_name, name) != 0);
    ASSERT_TEST(tribe_name != name);
    free(tribe_name);
    return true;
}

bool subAddAreaInvalidId(Election sample) {
    ASSERT_TEST(electionAddArea(sample, -1, "invalid area") ==
                ELECTION_INVALID_ID);
    Map results = electionComputeAreasToTribesMapping(sample);
    ASSERT_TEST(mapGet(results, "-1") == NULL);
    ASSERT_TEST(mapGetSize(results) == 6);
    mapDestroy(results);
    return true;
}

bool subAddAreaExist(Election sample) {
    electionAddVote(sample, 21, 12, 10);
    Map results = electionComputeAreasToTribesMapping(sample);
    ASSERT_TEST(strcmp(mapGet(results, "21"), "12") == 0);
    ASSERT_TEST(mapGetSize(results) == 6);
    mapDestroy(results);
    // Existing ID
    ASSERT_TEST(electionAddArea(sample, 21, "id exist") ==
                ELECTION_AREA_ALREADY_EXIST);
    results = electionComputeAreasToTribesMapping(sample);
    ASSERT_TEST(strcmp(mapGet(results, "21"), "12") == 0);
    ASSERT_TEST(mapGetSize(results) == 6);
    mapDestroy(results);
    // Existing Name
    ASSERT_TEST(electionAddArea(sample, 1, "area a") == ELECTION_SUCCESS);

    return true;
}

bool subAddAreaLongName(Election sample) {
    ASSERT_TEST(electionAddArea(sample, 1, SUPER_LONG_STRING) ==
                ELECTION_SUCCESS);
    // We can't verify anything here since no one can ever know what the name of
    // an area is. There's literally no way of getting it using the API. Shlach
    // and Shkach
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
    // Check empty string
    ASSERT_TEST(electionAddArea(sample, 3, "") == ELECTION_SUCCESS);
    // String with only space
    ASSERT_TEST(electionAddArea(sample, 4, " ") == ELECTION_SUCCESS);
    return true;
}

// Checking on edge cases integers. Max, min and zero ad id
bool subAddAreaExtremeIdValues(Election sample) {
    ASSERT_TEST(electionAddArea(sample, INT_MAX, "max int") ==
                ELECTION_SUCCESS);
    // Verify it was added. Try adding votes
    ASSERT_TEST(electionAddVote(sample, INT_MAX, 12, 10) == ELECTION_SUCCESS);

    ASSERT_TEST(electionAddArea(sample, 0, "zero id") == ELECTION_SUCCESS);
    // Verify it was added. Try adding votes
    ASSERT_TEST(electionAddVote(sample, 0, 12, 10) == ELECTION_SUCCESS);

    ASSERT_TEST(electionAddArea(sample, INT_MIN, "min int") ==
                ELECTION_INVALID_ID);
    // Verify it wasn't added. Try adding votes
    ASSERT_TEST(electionAddVote(sample, INT_MIN, 12, 10) ==
                ELECTION_INVALID_ID);
    return true;
}

bool subAddAreaErrorPrecedence(Election sample) {
    ASSERT_TEST(electionAddArea(NULL, -1, "!!!!!") == ELECTION_NULL_ARGUMENT);
    ASSERT_TEST(electionAddArea(sample, -1, "!!") == ELECTION_INVALID_ID);
    ASSERT_TEST(electionAddArea(sample, 21, "!!!") ==
                ELECTION_AREA_ALREADY_EXIST);
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

bool subRemoveAreaFirstLast(Election sample) {
    ASSERT_TEST(electionRemoveAreas(sample, specificArea(21)) ==
                ELECTION_SUCCESS);
    ASSERT_TEST(electionRemoveAreas(sample, specificArea(26)) ==
                ELECTION_SUCCESS);

    Map results = electionComputeAreasToTribesMapping(sample);
    ASSERT_TEST(strcmp(mapGet(results, "22"), "11") == 0);
    ASSERT_TEST(strcmp(mapGet(results, "23"), "11") == 0);
    ASSERT_TEST(strcmp(mapGet(results, "24"), "11") == 0);
    ASSERT_TEST(strcmp(mapGet(results, "25"), "11") == 0);
    ASSERT_TEST(mapGetSize(results) == 4);
    mapDestroy(results);
    return true;
}

/**
 * sub tests for adding votes.
 */
bool subAddVotesNullArgument(Election sample) {
    ASSERT_TEST(electionAddVote(NULL, 21, 11, 1) == ELECTION_NULL_ARGUMENT);
    ASSERT_TEST(electionAddVote(sample, 21, 11, 1) == ELECTION_SUCCESS);
    ASSERT_TEST(electionRemoveVote(NULL, 22, 21, 1) == ELECTION_NULL_ARGUMENT);
    return true;
}

bool subAddVotesInvalidId(Election sample) {
    ASSERT_TEST(electionAddVote(sample, -1, 11, 2) == ELECTION_INVALID_ID);
    ASSERT_TEST(electionAddVote(sample, 21, -11, 2) == ELECTION_INVALID_ID);

    ASSERT_TEST(electionAddArea(sample, 0, "zero area") == ELECTION_SUCCESS);
    ASSERT_TEST(electionAddVote(sample, 0, 11, 2) == ELECTION_SUCCESS);
    ASSERT_TEST(electionAddTribe(sample, 0, "zero tribe") == ELECTION_SUCCESS);
    ASSERT_TEST(electionAddVote(sample, 21, 0, 3) == ELECTION_SUCCESS);

    ASSERT_TEST(electionAddVote(sample, 11, 21, -1) == ELECTION_INVALID_VOTES);
    ASSERT_TEST(electionAddVote(sample, 11, 21, 0) == ELECTION_INVALID_VOTES);

    ASSERT_TEST(electionAddVote(sample, 21, 11, 8) == ELECTION_SUCCESS);
    return true;
}

bool subAddVotesNotExits(Election sample) {
    ASSERT_TEST(electionAddVote(sample, 99, 11, 3) == ELECTION_AREA_NOT_EXIST);
    ASSERT_TEST(electionAddVote(sample, 21, 99, 3) == ELECTION_TRIBE_NOT_EXIST);

    ASSERT_TEST(electionAddVote(sample, 21, 11, 1) == ELECTION_SUCCESS);
    ASSERT_TEST(electionAddVote(sample, 22, 12, 2) == ELECTION_SUCCESS);
    ASSERT_TEST(electionRemoveAreas(sample, specificArea(21)) ==
                ELECTION_SUCCESS);
    ASSERT_TEST(electionAddVote(sample, 21, 11, 1) == ELECTION_AREA_NOT_EXIST);
    ASSERT_TEST(electionRemoveTribe(sample, 12) == ELECTION_SUCCESS);
    ASSERT_TEST(electionAddVote(sample, 25, 12, 5) == ELECTION_TRIBE_NOT_EXIST);
    return true;
}

bool subAddVotesErrorPrecedence(Election sample) {
    ASSERT_TEST(electionAddVote(NULL, -1, -1, -1) == ELECTION_NULL_ARGUMENT);
    ASSERT_TEST(electionAddVote(NULL, 99, 99, -1) == ELECTION_NULL_ARGUMENT);

    ASSERT_TEST(electionAddVote(sample, -1, 11, -1) == ELECTION_INVALID_ID);
    ASSERT_TEST(electionAddVote(sample, 21, -1, -1) == ELECTION_INVALID_ID);
    ASSERT_TEST(electionAddVote(sample, -1, 99, -1) == ELECTION_INVALID_ID);
    ASSERT_TEST(electionAddVote(sample, 99, -1, -1) == ELECTION_INVALID_ID);

    ASSERT_TEST(electionAddVote(sample, 99, 99, -1) == ELECTION_INVALID_VOTES);
    ASSERT_TEST(electionAddVote(sample, 99, 99, 3) == ELECTION_AREA_NOT_EXIST);
    ASSERT_TEST(electionAddVote(sample, 21, 99, 3) == ELECTION_TRIBE_NOT_EXIST);

    return true;
}

bool subAddVotesNegative(Election sample) {
    ASSERT_TEST(electionAddVote(sample, 21, 11, -100) ==
                ELECTION_INVALID_VOTES);
    ASSERT_TEST(electionAddVote(sample, 22, 11, -100) ==
                ELECTION_INVALID_VOTES);
    ASSERT_TEST(electionAddVote(sample, 26, 11, -100) ==
                ELECTION_INVALID_VOTES);
    ASSERT_TEST(subComputeNoVotes(sample));
    return true;
}

/**
 * sub tests for removing  votes.
 */
bool subRemoveVotesInvalidId(Election sample) {
    ASSERT_TEST(electionRemoveVote(sample, -1, 11, 2) == ELECTION_INVALID_ID);
    ASSERT_TEST(electionRemoveVote(sample, 21, -11, 2) == ELECTION_INVALID_ID);

    ASSERT_TEST(electionAddArea(sample, 0, "zero area") == ELECTION_SUCCESS);
    ASSERT_TEST(electionRemoveVote(sample, 0, 11, 2) == ELECTION_SUCCESS);
    ASSERT_TEST(electionAddTribe(sample, 0, "zero tribe") == ELECTION_SUCCESS);
    ASSERT_TEST(electionRemoveVote(sample, 21, 0, 3) == ELECTION_SUCCESS);

    ASSERT_TEST(electionRemoveVote(sample, 11, 21, -1) ==
                ELECTION_INVALID_VOTES);
    ASSERT_TEST(electionRemoveVote(sample, 11, 21, 0) ==
                ELECTION_INVALID_VOTES);
    return true;
}

bool subRemoveVotesNonExisting(Election sample) {
    ASSERT_TEST(electionAddVote(sample, 21, 11, 4) == ELECTION_SUCCESS);
    ASSERT_TEST(electionRemoveVote(sample, 99, 11, 3) ==
                ELECTION_AREA_NOT_EXIST);
    ASSERT_TEST(electionRemoveVote(sample, 21, 99, 2) ==
                ELECTION_TRIBE_NOT_EXIST);

    ASSERT_TEST(electionRemoveTribe(sample, 11) == ELECTION_SUCCESS);
    ASSERT_TEST(electionRemoveVote(sample, 21, 11, 2) ==
                ELECTION_TRIBE_NOT_EXIST);
    ASSERT_TEST(electionRemoveAreas(sample, specificArea(24)) ==
                ELECTION_SUCCESS);
    ASSERT_TEST(electionRemoveVote(sample, 24, 13, 3) ==
                ELECTION_AREA_NOT_EXIST);
    return true;
}

bool subRemoveVotesNegative(Election sample) {
    ASSERT_TEST(electionRemoveVote(sample, 21, 12, -100) ==
                ELECTION_INVALID_VOTES);
    ASSERT_TEST(electionRemoveVote(sample, 22, 12, -100) ==
                ELECTION_INVALID_VOTES);
    ASSERT_TEST(electionRemoveVote(sample, 26, 12, -100) ==
                ELECTION_INVALID_VOTES);
    ASSERT_TEST(subComputeNoVotes(sample));
    return true;
}

bool subRemoveVotesMinIsZero(Election sample) {
    ASSERT_TEST(electionRemoveVote(sample, 21, 11, 100) == ELECTION_SUCCESS);
    ASSERT_TEST(electionRemoveVote(sample, 22, 11, 100) == ELECTION_SUCCESS);
    ASSERT_TEST(electionRemoveVote(sample, 26, 11, 100) == ELECTION_SUCCESS);
    ASSERT_TEST(subComputeNoVotes(sample));
    return true;
}

/**
 * sub tests for Setting tribe names.
 */

bool subSetTribeNameNULLArgument(Election sample) {
    ASSERT_TEST(electionSetTribeName(NULL, 11, "NULL ELECTION NAME") ==
                ELECTION_NULL_ARGUMENT);
    ASSERT_TEST(electionSetTribeName(sample, 11, NULL) ==
                ELECTION_NULL_ARGUMENT);
    return true;
}

bool subSetTribeNameInvalidId(Election sample) {
    ASSERT_TEST(electionAddTribe(sample, 0, "tribe id zero") ==
                ELECTION_SUCCESS);
    ASSERT_TEST(electionSetTribeName(sample, 0,
                                     "changing the name of a tribe id zero") ==
                ELECTION_SUCCESS);
    ASSERT_TEST(electionSetTribeName(
                    sample, -1, "changing the name of a NEGATIVE tribe id ") ==
                ELECTION_INVALID_ID);
    return true;
}

bool sudSetTribeNameTribeNotExits(Election sample) {
    ASSERT_TEST(electionSetTribeName(sample, 99, "tribe id not exists") ==
                ELECTION_TRIBE_NOT_EXIST);
    ASSERT_TEST(electionRemoveTribe(sample, 11) == ELECTION_SUCCESS);
    ASSERT_TEST(electionSetTribeName(sample, 11, "not exists") ==
                ELECTION_TRIBE_NOT_EXIST);
    return true;
}

bool subSetTribeNameDifferentStrings(Election sample) {
    ASSERT_TEST(electionSetTribeName(sample, 12, "exclamation point!") ==
                ELECTION_INVALID_NAME);
    ASSERT_TEST(
        electionSetTribeName(sample, 12, "{right bracket without space") ==
        ELECTION_INVALID_NAME);
    ASSERT_TEST(electionSetTribeName(sample, 12, "` Grave accent with space") ==
                ELECTION_INVALID_NAME);
    ASSERT_TEST(
        electionSetTribeName(sample, 12, "Grave` accent without space") ==
        ELECTION_INVALID_NAME);
    ASSERT_TEST(
        electionSetTribeName(sample, 12, "{ right bracket with space") ==
        ELECTION_INVALID_NAME);
    ASSERT_TEST(electionSetTribeName(sample, 12, "ALL UPPER CASE") ==
                ELECTION_INVALID_NAME);
    char *tribe_name = electionGetTribeName(sample, 12);
    ASSERT_TEST(strcmp(tribe_name, "ALL UPPER CASE") != 0);
    ASSERT_TEST(electionSetTribeName(sample, 12, "normal string") ==
                ELECTION_SUCCESS);
    free(tribe_name);

    tribe_name = electionGetTribeName(sample, 12);
    ASSERT_TEST(strcmp(tribe_name, "normal string") == 0);
    free(tribe_name);
    return true;
}

bool subSetTribeNameErrorPrecedence(Election sample) {
    ASSERT_TEST(electionSetTribeName(NULL, -1, "UPPER CASE") ==
                ELECTION_NULL_ARGUMENT);
    ASSERT_TEST(electionSetTribeName(sample, -1, "UPPER CASE") ==
                ELECTION_INVALID_ID);
    ASSERT_TEST(electionSetTribeName(sample, 99, "UPERR CASE") ==
                ELECTION_TRIBE_NOT_EXIST);
    ASSERT_TEST(electionSetTribeName(sample, 11, "fine name") ==
                ELECTION_SUCCESS);
    return true;
}

/**
 * sub tests for Getting tribe names.
 */
bool subGetTribeNameNullArgument(Election sample) {
    char *tribe_name = electionGetTribeName(NULL, 11);
    ASSERT_TEST(tribe_name == NULL);
    return true;
}

bool subGetTribeNameComperingStrings(Election sample) {
    char *tribe_name = electionGetTribeName(sample, 11);
    ASSERT_TEST(strcmp(tribe_name, "tribe a") == 0);
    ASSERT_TEST(electionRemoveTribe(sample, 11) == ELECTION_SUCCESS);
    free(tribe_name);

    tribe_name = electionGetTribeName(sample, 11);
    ASSERT_TEST(tribe_name == NULL);

    ASSERT_TEST(electionAddTribe(sample, 11, "new tribe") == ELECTION_SUCCESS);
    tribe_name = electionGetTribeName(sample, 11);
    ASSERT_TEST(strcmp(tribe_name, "new tribe") == 0);
    free(tribe_name);
    return true;
}

/**
 * Sub tests for creating new map.
 */

bool subCreateEmptyElection(Election sample) {
    Election empty_election = electionCreate();
    Map results = electionComputeAreasToTribesMapping(empty_election);
    ASSERT_TEST(mapGetSize(results) == 0);
    ASSERT_TEST(mapGetFirst(results) == NULL);
    electionDestroy(empty_election);
    mapDestroy(results);
    return true;
}

/**
 * Sub tests for computing votes to areas
 */
bool subComputePrecedence(Election sample) {
    electionAddVote(sample, 21, 12, 101);
    electionAddVote(sample, 21, 11, 101);
    electionAddVote(sample, 21, 13, 101);
    Map results = electionComputeAreasToTribesMapping(sample);
    ASSERT_TEST(results);
    ASSERT_TEST(strcmp(mapGet(results, "21"), "11") == 0);
    ASSERT_TEST(mapGetSize(results) == 6);
    mapDestroy(results);
    return true;
}

bool subComputeSanity(Election sample) {
    electionAddVote(sample, 21, 12, 11);
    electionAddVote(sample, 21, 11, 10);
    electionAddVote(sample, 21, 13, 10);
    Map results = electionComputeAreasToTribesMapping(sample);
    ASSERT_TEST(mapGetSize(results) == 6);
    ASSERT_TEST(results);
    ASSERT_TEST(strcmp(mapGet(results, "21"), "12") == 0);
    mapDestroy(results);
    electionAddVote(sample, 21, 13, 10);
    results = electionComputeAreasToTribesMapping(sample);
    ASSERT_TEST(mapGetSize(results) == 6);
    ASSERT_TEST(strcmp(mapGet(results, "21"), "13") == 0);
    mapDestroy(results);
    electionRemoveVote(sample, 21, 13, 10);
    results = electionComputeAreasToTribesMapping(sample);
    ASSERT_TEST(mapGetSize(results) == 6);
    ASSERT_TEST(strcmp(mapGet(results, "21"), "12") == 0);

    mapDestroy(results);
    electionRemoveVote(sample, 21, 12, 1);
    results = electionComputeAreasToTribesMapping(sample);
    ASSERT_TEST(mapGetSize(results) == 6);
    ASSERT_TEST(strcmp(mapGet(results, "21"), "11") == 0);
    mapDestroy(results);
    electionRemoveVote(sample, 21, 11, 1);

    results = electionComputeAreasToTribesMapping(sample);
    ASSERT_TEST(mapGetSize(results) == 6);
    ASSERT_TEST(strcmp(mapGet(results, "21"), "12") == 0);

    mapDestroy(results);
    return true;
}

bool subComputeNoTribes(Election sample) {
    electionRemoveTribe(sample, 11);
    electionRemoveTribe(sample, 12);
    electionRemoveTribe(sample, 13);
    electionRemoveTribe(sample, 14);
    electionRemoveTribe(sample, 15);
    Map results = electionComputeAreasToTribesMapping(sample);
    ASSERT_TEST(results);
    ASSERT_TEST(mapGetSize(results) == 0);
    mapDestroy(results);
    return true;
}

bool subComputeNoVotes(Election sample) {
    Map results = electionComputeAreasToTribesMapping(sample);
    ASSERT_TEST(results);
    ASSERT_TEST(strcmp(mapGet(results, "21"), "11") == 0);
    ASSERT_TEST(strcmp(mapGet(results, "22"), "11") == 0);
    ASSERT_TEST(strcmp(mapGet(results, "23"), "11") == 0);
    ASSERT_TEST(strcmp(mapGet(results, "24"), "11") == 0);
    ASSERT_TEST(strcmp(mapGet(results, "25"), "11") == 0);
    ASSERT_TEST(strcmp(mapGet(results, "26"), "11") == 0);
    ASSERT_TEST(mapGetSize(results) == 6);
    mapDestroy(results);
    return true;
}

bool subComputeAddAreasFirst(Election sample) {
    Election election = electionCreate();
    ASSERT_TEST(election);
    ASSERT_TEST(electionAddArea(election, 21, "area a") == ELECTION_SUCCESS);
    ASSERT_TEST(electionAddArea(election, 22, "area b") == ELECTION_SUCCESS);
    ASSERT_TEST(electionAddArea(election, 23, "area c") == ELECTION_SUCCESS);
    ASSERT_TEST(electionAddArea(election, 24, "area d") == ELECTION_SUCCESS);
    ASSERT_TEST(electionAddArea(election, 25, "area e") == ELECTION_SUCCESS);
    ASSERT_TEST(electionAddArea(election, 26, "area f") == ELECTION_SUCCESS);
    ASSERT_TEST(electionAddTribe(election, 11, "tribe a") == ELECTION_SUCCESS);
    ASSERT_TEST(electionAddTribe(election, 12, "tribe b") == ELECTION_SUCCESS);
    ASSERT_TEST(electionAddTribe(election, 13, "tribe c") == ELECTION_SUCCESS);
    ASSERT_TEST(electionAddTribe(election, 14, "tribe d") == ELECTION_SUCCESS);
    ASSERT_TEST(electionAddTribe(election, 15, "tribe e") == ELECTION_SUCCESS);
    bool status = subComputeSanity(election);

    electionDestroy(election);
    return status;
}

bool subComputeAddTribesFirst(Election sample) {
    Election election = electionCreate();
    ASSERT_TEST(election);
    ASSERT_TEST(electionAddTribe(election, 11, "tribe a") == ELECTION_SUCCESS);
    ASSERT_TEST(electionAddTribe(election, 12, "tribe b") == ELECTION_SUCCESS);
    ASSERT_TEST(electionAddTribe(election, 13, "tribe c") == ELECTION_SUCCESS);
    ASSERT_TEST(electionAddTribe(election, 14, "tribe d") == ELECTION_SUCCESS);
    ASSERT_TEST(electionAddTribe(election, 15, "tribe e") == ELECTION_SUCCESS);
    ASSERT_TEST(electionAddArea(election, 21, "area a") == ELECTION_SUCCESS);
    ASSERT_TEST(electionAddArea(election, 22, "area b") == ELECTION_SUCCESS);
    ASSERT_TEST(electionAddArea(election, 23, "area c") == ELECTION_SUCCESS);
    ASSERT_TEST(electionAddArea(election, 24, "area d") == ELECTION_SUCCESS);
    ASSERT_TEST(electionAddArea(election, 25, "area e") == ELECTION_SUCCESS);
    ASSERT_TEST(electionAddArea(election, 26, "area f") == ELECTION_SUCCESS);
    bool status = subComputeSanity(election);

    electionDestroy(election);
    return status;
}

bool subComputeAddMixed(Election sample) {
    Election election = electionCreate();
    ASSERT_TEST(election);
    ASSERT_TEST(electionAddArea(election, 21, "area a") == ELECTION_SUCCESS);
    ASSERT_TEST(electionAddTribe(election, 14, "tribe d") == ELECTION_SUCCESS);
    ASSERT_TEST(electionAddTribe(election, 12, "tribe b") == ELECTION_SUCCESS);
    ASSERT_TEST(electionAddArea(election, 22, "area b") == ELECTION_SUCCESS);
    ASSERT_TEST(electionAddTribe(election, 13, "tribe c") == ELECTION_SUCCESS);
    ASSERT_TEST(electionAddArea(election, 23, "area c") == ELECTION_SUCCESS);
    ASSERT_TEST(electionAddArea(election, 24, "area d") == ELECTION_SUCCESS);
    ASSERT_TEST(electionAddTribe(election, 11, "tribe a") == ELECTION_SUCCESS);
    ASSERT_TEST(electionAddTribe(election, 15, "tribe e") == ELECTION_SUCCESS);
    ASSERT_TEST(electionAddArea(election, 25, "area e") == ELECTION_SUCCESS);
    ASSERT_TEST(electionAddArea(election, 26, "area f") == ELECTION_SUCCESS);
    bool status = subComputeSanity(election);

    electionDestroy(election);
    return status;
}

bool subStressAddRemoveRepeat(Election sample) {
    bool status = true;
    const int iterations = STRESS_INVERTALS_MODIFIER * 10;

    for (int i = 0; i < iterations; i++) {
        ASSERT_TEST(subRemoveTribeReadd(sample));
    }

    ASSERT_TEST(subComputeSanity(sample));

    for (int i = 0; i < iterations; i++) {
        ASSERT_TEST(subRemoveAreaReAdd(sample));
        ASSERT_TEST(subComputeSanity(sample));
    }

    return status;
}

bool subStressAddThenRemove(Election sample) {
    char *str;
    const int iterations = STRESS_INVERTALS_MODIFIER / 20;
    for (int i = 0; i < iterations; i++) {
        str = randLowerString(7);
        ASSERT_TEST(electionAddArea(sample, i + 100, str) == ELECTION_SUCCESS);
        free(str);
        str = randLowerString(7);
        ASSERT_TEST(electionAddTribe(sample, i + 100, str) == ELECTION_SUCCESS);
        ASSERT_TEST(electionAddVote(sample, i + 100, i + 100, 1) ==
                    ELECTION_SUCCESS);
        free(str);
    }

    // Verify votes
    Map results = electionComputeAreasToTribesMapping(sample);
    ASSERT_TEST(mapGetSize(results) == (6 + iterations));
    char *cur_ind;
    for (int i = 0; i < iterations; i++) {
        cur_ind = tester_intToStr(i + 100);
        ASSERT_TEST(strcmp(mapGet(results, cur_ind), cur_ind) == 0);
        free(cur_ind);
    }
    mapDestroy(results);

    for (int i = 0; i < iterations; i++) {
        ASSERT_TEST(electionRemoveAreas(sample, specificArea(i + 100)) ==
                    ELECTION_SUCCESS);
        ASSERT_TEST(electionRemoveTribe(sample, i + 100) == ELECTION_SUCCESS);
    }

    subComputeSanity(sample);

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
    TEST_WITH_SAMPLE(subCreateEmptyElection, "Empty Election System");
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
    TEST_WITH_SAMPLE(subAddTribeErrorPrecedence, "Error Precedence");
}

void testRemoveTribe() {
    printf("Testing %s tests:\n", "'Remove Tribe'");
    TEST_WITH_SAMPLE(subRemoveTribeReadd, "Re-Adding Tribe");
    TEST_WITH_SAMPLE(subRemoveTribeNullArgument, "Inserting Null argument");
    TEST_WITH_SAMPLE(subRemoveTribeInvalidId, "Inserting Invalid Id");
    TEST_WITH_SAMPLE(subRemoveTribeNotExist, "Removing a non existing tribe");
    TEST_WITH_SAMPLE(subRemoveTribeWithVotes, "adding votes");
    TEST_WITH_SAMPLE(subRemoveTribeFirstMiddelLast,
                     "removing from the top, bottom, middle list");
    TEST_WITH_SAMPLE(subRemoveTribeErrorPrecedence, "Error Precedence");
}

void testAddArea() {
    printf("Testing %s tests:\n", "'Add Area'");
    TEST_WITH_SAMPLE(subAddAreaInvalidId, "Invalid Area ID");
    TEST_WITH_SAMPLE(subAddAreaLongName, "Long Area Name");
    TEST_WITH_SAMPLE(subAddAreaExist, "Pre Existing Area/AreaId");
    TEST_WITH_SAMPLE(subAddAreaInvalidName, "Invalid Area Names");
    TEST_WITH_SAMPLE(subAddAreaValidName, "Valid Area Names");
    TEST_WITH_SAMPLE(subAddAreaExtremeIdValues,
                     "Verify Area Extreme Id Values");
    TEST_WITH_SAMPLE(subAddAreaErrorPrecedence, "checking order of errors");
}

void testRemoveArea() {
    printf("Testing %s tests:\n", "'Remove Area'");
    TEST_WITH_SAMPLE(subRemoveAreaReAdd, "Re-Adding Area");
    TEST_WITH_SAMPLE(subRemoveAreaFirstLast, "Remove First&Last");
}

void testRemoveAreas() {}

void testAddVote() {
    printf("Testing %s tests:\n", "'Add Vote'");
    TEST_WITH_SAMPLE(subAddVotesNullArgument, "Inserting Null argument");
    TEST_WITH_SAMPLE(subAddVotesInvalidId, "Inserting Invalid Id");
    TEST_WITH_SAMPLE(subAddVotesNotExits,
                     "Inserting non existing areas and tribes");
    TEST_WITH_SAMPLE(subAddVotesNegative, "Insert Negative Votes");
    TEST_WITH_SAMPLE(subAddVotesErrorPrecedence, "Error Precedence");
}

void testRemoveVote() {
    printf("Testing %s tests:\n", "'Remove Vote'");
    TEST_WITH_SAMPLE(subRemoveVotesInvalidId, "Inserting Invalid Id");
    TEST_WITH_SAMPLE(subRemoveVotesNonExisting, "Non Existing Tribes Or Areas");
    TEST_WITH_SAMPLE(subRemoveVotesNegative, "Remove Negative Votes");
    TEST_WITH_SAMPLE(subRemoveVotesMinIsZero,
                     "Remove More Votes Than Tribe Has");
}

void testComputeAreasToTribesMapping() {
    printf("Testing %s tests:\n", "'Compute Votes'");
    TEST_WITH_SAMPLE(subComputePrecedence, "Compute Tribe Precedence");
    TEST_WITH_SAMPLE(subComputeNoVotes, "Compute Area w/ No Votes");
    TEST_WITH_SAMPLE(subComputeNoTribes, "Compute Area w/ No Tribes");
    TEST_WITH_SAMPLE(subComputeSanity, "Compute Sanity");
    TEST_WITH_SAMPLE(subComputeAddAreasFirst,
                     "Compute When Adding Areas First");
    TEST_WITH_SAMPLE(subComputeAddTribesFirst,
                     "Compute When Adding Tribes First");
    TEST_WITH_SAMPLE(subComputeAddMixed,
                     "Compute When Adding Areas&Tribes in a Mix");
}

void testSetTribeName() {
    printf("Testing %s tests:\n", "'Set Tribe Name'");
    TEST_WITH_SAMPLE(subSetTribeNameDifferentStrings,
                     "Set Tribe Name Vallidations");
    TEST_WITH_SAMPLE(subSetTribeNameInvalidId, "Invalid Ids");
    TEST_WITH_SAMPLE(subSetTribeNameNULLArgument, "Null Args");
    TEST_WITH_SAMPLE(sudSetTribeNameTribeNotExits, "Non Existing");
    TEST_WITH_SAMPLE(subSetTribeNameErrorPrecedence, "Error Precedence");
}

void testGetTribeName() {
    printf("Testing %s tests:\n", "'Get Tribe Name'");
    TEST_WITH_SAMPLE(subGetTribeNameNullArgument, "Null Args");
    TEST_WITH_SAMPLE(subGetTribeNameComperingStrings, "Compare Names");
}

void testDoomsDay() {
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
