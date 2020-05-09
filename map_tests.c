#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../mtm_map/map.h"
#include "test_utilities.h"
#include "utils.h"

bool testMapCreate() {
    printf("Testing mapCreate\n");
    Map map1 = mapCreate();
    Map map2 = mapCreate();
    Map map3 = mapCreate();
    mapDestroy(map3);
    ASSERT_TEST(map1 != NULL);
    ASSERT_TEST(map2 != NULL);
    ASSERT_TEST(mapGetSize(map1) == 0);
    ASSERT_TEST(mapGetFirst(map1) == NULL);
    mapPut(map1, "mary", "poppins");
    mapDestroy(map1);
    map1 = NULL;
    mapDestroy(map1);
    ASSERT_TEST(mapGet(map1, "mary") == NULL);
    ASSERT_TEST(mapGetSize(map1) == -1);
    ASSERT_TEST(map2 != NULL);
    mapDestroy(map2);
    return true;
}

bool testMapPutGet() {
    printf("Testing mapPut, mapGet and mapGetSize\n");
    Map map = mapCreate();

    const char *key1 = "key1";
    const char *key2 = "key2";
    const char *key3 = "key3";

    int count = 0;

    printf("Verify putting keys\n");
    ASSERT_TEST(mapPut(map, key1, "value1") == MAP_SUCCESS);
    count++;
    ASSERT_TEST(mapPut(map, key2, "value2") == MAP_SUCCESS);
    count++;
    ASSERT_TEST(mapPut(map, key3, "value3") == MAP_SUCCESS);
    count++;

    printf("Verify sizes change correctly\n");
    ASSERT_TEST(mapGetSize(map) == count);
    ASSERT_TEST(mapPut(map, key3, "value4") == MAP_SUCCESS);
    ASSERT_TEST(mapGetSize(map) == count);
    ASSERT_TEST(mapRemove(map, key2) == MAP_SUCCESS);
    count--;
    ASSERT_TEST(mapGetSize(map) == count);
    ASSERT_TEST(mapRemove(map, key1) == MAP_SUCCESS);
    count--;
    ASSERT_TEST(mapRemove(map, key3) == MAP_SUCCESS);
    count--;
    ASSERT_TEST(mapPut(map, key3, "value1") == MAP_SUCCESS);
    count++;

    printf("Verify `PUT` with NULL arguments\n");
    ASSERT_TEST(mapPut(map, key3, NULL) == MAP_NULL_ARGUMENT);
    ASSERT_TEST(mapPut(map, NULL, "value3") == MAP_NULL_ARGUMENT);
    ASSERT_TEST(mapPut(map, NULL, NULL) == MAP_NULL_ARGUMENT);
    ASSERT_TEST(mapPut(NULL, key3, "value3") == MAP_NULL_ARGUMENT);
    ASSERT_TEST(mapPut(NULL, NULL, NULL) == MAP_NULL_ARGUMENT);
    ASSERT_TEST(mapGetSize(map) == count);

    printf("Make sure added key has the correct value\n");
    ASSERT_TEST(mapPut(map, key1, "value2") == MAP_SUCCESS);
    count++;
    ASSERT_TEST(strcmp(mapGet(map, key1), "value2") == 0);
    printf("Verify value of key changed\n");
    ASSERT_TEST(mapPut(map, key1, "value1") == MAP_SUCCESS);
    ASSERT_TEST(strcmp(mapGet(map, key1), "value1") == 0);
    ASSERT_TEST(mapRemove(map, key1) == MAP_SUCCESS);
    count--;
    printf("Verify key doesn't exist\n");
    ASSERT_TEST(mapGet(map, key1) == NULL);

    printf("Verify string value is duplicated on PUT\n");
    const char *value1 = "value1";
    ASSERT_TEST(mapPut(map, key1, value1) == MAP_SUCCESS);
    count++;
    ASSERT_TEST(mapGet(map, key1) != value1);
    ASSERT_TEST(mapGetSize(map) == count);

    printf("Verify string isn't duplicated on GET\n");
    ASSERT_TEST(mapGet(map, key1) == mapGet(map, key1));

    printf("Verify override lookup by value and not by address\n");
    ASSERT_TEST(mapPut(map, "key1", value1) == MAP_SUCCESS);
    ASSERT_TEST(mapGetSize(map) == count);
    ASSERT_TEST(strcmp(mapGet(map, key1), value1) == 0);

    printf("Testing emptry strings\n");
    ASSERT_TEST(mapPut(map, key1, "") == MAP_SUCCESS);
    ASSERT_TEST(mapPut(map, key2, "") == MAP_SUCCESS);
    count++;
    ASSERT_TEST(mapGetSize(map) == count);
    ASSERT_TEST(mapPut(map, "", value1) == MAP_SUCCESS);
    count++;
    ASSERT_TEST(mapGetSize(map) == count);
    ASSERT_TEST(strcmp(mapGet(map, ""), value1) == 0);
    ASSERT_TEST(strcmp(mapGet(map, key1), "") == 0);

    printf("Test a super long string\n");
    const char *super_long_string =
            "Hi— I’m Ted Mosby. And exactly 45 days from now you and I are "
            "going to meet and we’re going to fall in love and we’re going to "
            "get married and we’re going to have 2 kids and we’re going to "
            "love them and each other so much. All that is 45 days away, but "
            "I’m here now I guess because… I want those extra 45 days with "
            "you. I want each one of them. Look and if I can’t have them I’ll "
            "take the 45 seconds before your boyfriend shows up and punches me "
            "in the face, because… I love you. I’m always gonna love you, til "
            "the end of my days and beyond. You’ll see.Hi— I’m Ted Mosby. And "
            "exactly 45 days from now you and I are "
            "going to meet and we’re going to fall in love and we’re going to "
            "get married and we’re going to have 2 kids and we’re going to "
            "love them and each other so much. All that is 45 days away, but "
            "I’m here now I guess because… I want those extra 45 days with "
            "you. I want each one of them. Look and if I can’t have them I’ll "
            "take the 45 seconds before your boyfriend shows up and punches me "
            "in the face, because… I love you. I’m always gonna love you, til "
            "the end of my days and beyond. You’ll see.Hi— I’m Ted Mosby. And "
            "exactly 45 days from now you and I are "
            "going to meet and we’re going to fall in love and we’re going to "
            "get married and we’re going to have 2 kids and we’re going to "
            "love them and each other so much. All that is 45 days away, but "
            "I’m here now I guess because… I want those extra 45 days with "
            "you. I want each one of them. Look and if I can’t have them I’ll "
            "take the 45 seconds before your boyfriend shows up and punches me "
            "in the face, because… I love you. I’m always gonna love you, til "
            "the end of my days and beyond. You’ll see.Hi— I’m Ted Mosby. And "
            "exactly 45 days from now you and I are "
            "going to meet and we’re going to fall in love and we’re going to "
            "get married and we’re going to have 2 kids and we’re going to "
            "love them and each other so much. All that is 45 days away, but "
            "I’m here now I guess because… I want those extra 45 days with "
            "you. I want each one of them. Look and if I can’t have them I’ll "
            "take the 45 seconds before your boyfriend shows up and punches me "
            "in the face, because… I love you. I’m always gonna love you, til "
            "the end of my days and beyond. You’ll see.Hi— I’m Ted Mosby. And "
            "exactly 45 days from now you and I are "
            "going to meet and we’re going to fall in love and we’re going to "
            "get married and we’re going to have 2 kids and we’re going to "
            "love them and each other so much. All that is 45 days away, but "
            "I’m here now I guess because… I want those extra 45 days with "
            "you. I want each one of them. Look and if I can’t have them I’ll "
            "take the 45 seconds before your boyfriend shows up and punches me "
            "in the face, because… I love you. I’m always gonna love you, til "
            "the end of my days and beyond. You’ll see.";
    ASSERT_TEST(mapPut(map, key1, super_long_string) == MAP_SUCCESS);
    ASSERT_TEST(strcmp(mapGet(map, key1), super_long_string) == 0);
    ASSERT_TEST(mapPut(map, super_long_string, value1) == MAP_SUCCESS);
    ASSERT_TEST(strcmp(mapGet(map, super_long_string), value1) == 0);
    count++;
    ASSERT_TEST(strcmp(mapGet(map, super_long_string), value1) == 0);
    ASSERT_TEST(mapGetSize(map) == count);
    ASSERT_TEST(strcmp(mapGet(map, key2), "") == 0);
    mapDestroy(map);
    printf(
            "Destroy the list without removing all the keys. To let valgrind catch "
            "de-allocation errors\n");
    return true;
}

bool testMapCopy() {
    printf("Test mapCopy function\n");
    ASSERT_TEST(mapCopy(NULL) == NULL);
    Map map1 = mapCreate();
    ASSERT_TEST(mapPut(map1, "key1", "value1") == MAP_SUCCESS);
    ASSERT_TEST(mapPut(map1, "key2", "value2") == MAP_SUCCESS);
    Map map2 = mapCopy(map1);
    ASSERT_TEST(strcmp(mapGet(map2, "key1"), mapGet(map1, "key1")) == 0);
    ASSERT_TEST(strcmp(mapGet(map2, "key2"), mapGet(map1, "key2")) == 0);

    printf("Verify size are the same\n");
    ASSERT_TEST(mapGetSize(map2) == mapGetSize(map1));

    printf("Verify mapCopy duplicated the strings\n");
    ASSERT_TEST(mapGet(map2, "key2") != mapGet(map1, "key2"));
    ASSERT_TEST(mapRemove(map1, "key1") == MAP_SUCCESS);

    // Try re-allocating the same memory so it will change
    ASSERT_TEST(mapPut(map1, "key4", "value1") == MAP_SUCCESS);
    ASSERT_TEST(mapPut(map1, "key5", "value1") == MAP_SUCCESS);
    ASSERT_TEST(mapPut(map1, "key6", "value1") == MAP_SUCCESS);
    ASSERT_TEST(mapPut(map1, "key7", "value1") == MAP_SUCCESS);
    ASSERT_TEST(mapGet(map2, "key2") != NULL);

    mapDestroy(map1);
    ASSERT_TEST(strcmp(mapGet(map2, "key1"), "value1") == 0);
    ASSERT_TEST(strcmp(mapGet(map2, "key2"), "value2") == 0);
    mapDestroy(map2);
    return true;
}

bool testMapGetSize() {
    Map map = mapCreate();

    ASSERT_TEST(mapGetSize(map) == 0);
    ASSERT_TEST(mapPut(map, "key1", "value1") == MAP_SUCCESS);
    ASSERT_TEST(mapPut(map, "key1", "value2") == MAP_SUCCESS);
    ASSERT_TEST(mapGetSize(map) == 1);
    ASSERT_TEST(mapRemove(map, "key1") == MAP_SUCCESS);
    ASSERT_TEST(mapGetSize(map) == 0);
    ASSERT_TEST(mapGetSize(NULL) == -1);
    mapDestroy(map);
    return true;
}

bool testMapContains() {
    Map map1 = mapCreate();
    mapPut(map1, "key1", "value1");
    ASSERT_TEST(mapContains(map1, "key1") == true);
    ASSERT_TEST(mapContains(NULL, "key1") == false);
    ASSERT_TEST(mapContains(map1, NULL) == false);
    ASSERT_TEST(mapContains(map1, "dani") == false);
    mapDestroy(map1);
    return true;
}

bool testMapRemove() {
    Map map1 = mapCreate();
    mapPut(map1, "key1", "value1");
    mapPut(map1, "key2", "value2");
    mapPut(map1, "key3", "value3");
    mapRemove(map1, "key1");
    ASSERT_TEST(mapContains(map1, "key1") == false);
    ASSERT_TEST(mapGetSize(map1) == 2);
    mapPut(map1, "key1", "value1");
    mapRemove(map1, "key3");
    ASSERT_TEST(mapContains(map1, "key3") == false);
    ASSERT_TEST(mapGetSize(map1) == 2);
    mapPut(map1, "key3", "value3");
    mapRemove(map1, "key3");
    ASSERT_TEST(mapContains(map1, "key3") == false);
    ASSERT_TEST(mapGetSize(map1) == 2);
    mapDestroy(map1);
    return true;
}

bool doomsDay() {
    Map map = mapCreate();
    const int repeat = 10000;
    const int length = 7;
    char *arr[repeat];
    static int my_seed = 25011984;
    srand(time(NULL) * length + ++my_seed);
    char *str;
    for (int i = 0; i < repeat; i++) {
        str = randString(length);
        arr[i] = str;
        ASSERT_TEST(mapPut(map, str, str) == MAP_SUCCESS);
    }
    ASSERT_TEST(mapGetSize(map) == repeat);
    for (int i = 1; i <= repeat; i++) {
        ASSERT_TEST(strcmp(mapGet(map, arr[repeat - i]), arr[repeat - i]) == 0);
        ASSERT_TEST(mapRemove(map, arr[repeat - i]) == MAP_SUCCESS);
        free(arr[repeat - i]);
    }
    mapDestroy(map);
    return true;
}

bool testMapForEach() {
    printf("start testMapForEach \n");
    Map map = mapCreate();
    mapPut(map, "key 1", "value 1");
    mapPut(map, "key 2", "value 2");
    mapPut(map, "key 3", "value 3");
    mapPut(map, "key 4", "value 4");
    mapPut(map, "key 5", "value 5");
    int arr[5] = {0};
    int i = 0;
    //checking that the MAP_FOREACH went through all of the keys one at a time
    MAP_FOREACH(key, map) {
        arr[i]++;
        i++;
    }
    for (int j = 0; j < 5; j++){
        ASSERT_TEST(arr[j] == 1);
    }
    //trying to iterate over a NULL argument
    bool something_happened = false;
    MAP_FOREACH(key, NULL) {
        ASSERT_TEST(key == NULL);
        something_happened = true;
    }
    ASSERT_TEST(something_happened == false);

    //trying to iterate over a empty map
    Map map_2 = mapCreate();
    MAP_FOREACH(key, map_2) {
        ASSERT_TEST(key == NULL);
        something_happened = true;
    }
    ASSERT_TEST(something_happened == false);
    MAP_FOREACH(key,map){
        mapPut(map_2,key,mapGet(map,key));
    }
    ASSERT_TEST(mapGetSize(map_2) == mapGetSize(map));
    MAP_FOREACH(key,map_2){
        ASSERT_TEST(mapGet(map,key)!=NULL);
    }
    
    mapDestroy(map);
    mapDestroy(map_2);
    return true;
}

int main(int argc, char *argv[]) {
    printf("Start Map Tests\n");
    bool status = true;
    status = status && testMapPutGet();
    status = status && testMapCopy();
    status = status && testMapGetSize();
    status = status && testMapCreate();
    status = status && testMapContains();
    status = status && testMapRemove();
    status = status && doomsDay();
    status = status && testMapForEach();
    printf("end Map Tests\n");
    return status ? 0 : 1;
}
