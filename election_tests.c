#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

#include "../election/election.h"
#include "../mtm_map/map.h"

#define TEST_WITH_SAMPLE(test, name)         \
    do {                                     \
        Election sample = getSampleData();   \
        printf("Running sub %s ... ", name); \
        if (test(sample)) {                  \
            printf("[OK]\n");                \
        } else {                             \
            printf("[Failed]\n");            \
        }                                    \
        cleanUp(sample);                     \
    } while (0)

bool isEven(int num) { return num % 2 ? false : true; }

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
    bool status = true;
    status = status & (electionAddTribe(sample, -1, "InvalidTribe") ==
                       ELECTION_INVALID_ID);
    return status;
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
    return 0;
}