#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include <dbg.h>
#include <database.h>
#include <table.h>
#include <bstring.h>

#include "minunit.h"


//==============================================================================
//
// Macros
//
//==============================================================================

#define ADD_EVENT(OBJECT_FILE, TIMESTAMP, OBJECT_ID, ACTION_ID) do {\
    sky_event *event = sky_event_create(TIMESTAMP, OBJECT_ID, ACTION_ID);\
    mu_assert(sky_table_add_event(OBJECT_FILE, event) == 0, "");\
    sky_event_free(event);\
} while(0)

#define ADD_EVENT_WITH_DATA1(OBJECT_FILE, TIMESTAMP, OBJECT_ID, ACTION_ID, KEY, VALUE) do {\
    sky_event *event = sky_event_create(TIMESTAMP, OBJECT_ID, ACTION_ID);\
    sky_event_set_data(event, KEY, VALUE);\
    mu_assert(sky_table_add_event(OBJECT_FILE, event) == 0, "");\
    sky_event_free(event);\
} while(0)

#define ADD_EVENT_WITH_DATA2(OBJECT_FILE, TIMESTAMP, OBJECT_ID, ACTION_ID, KEY1, VALUE1, KEY2, VALUE2) do {\
    sky_event *event = sky_event_create(TIMESTAMP, OBJECT_ID, ACTION_ID);\
    sky_event_set_data(event, KEY1, VALUE1);\
    sky_event_set_data(event, KEY2, VALUE2);\
    mu_assert(sky_table_add_event(OBJECT_FILE, event) == 0, "");\
    sky_event_free(event);\
} while(0)



//==============================================================================
//
// Constants
//
//==============================================================================

struct tagbstring ROOT = bsStatic("tmp/db");
struct tagbstring OBJECT_TYPE = bsStatic("users");

struct tagbstring foo = bsStatic("foo");
struct tagbstring bar = bsStatic("bar");
struct tagbstring baz = bsStatic("baz");
struct tagbstring google = bsStatic("http://www.google.com/this/is/a/test/yay/super!");
struct tagbstring data10 = bsStatic("0123456789");
struct tagbstring data20 = bsStatic("01234567890123456789");
struct tagbstring data30 = bsStatic("012345678901234567890123456789");
struct tagbstring data40 = bsStatic("0123456789012345678901234567890123456789");
struct tagbstring data50 = bsStatic("01234567890123456789012345678901234567890123456789");


//==============================================================================
//
// Test Cases
//
//==============================================================================

//--------------------------------------
// Open
//--------------------------------------

int test_sky_table_open() {
    struct stat buffer;
    int rc;
    
    copydb("simple");
    
    sky_database *database = sky_database_create(&ROOT);
    sky_table *table = sky_table_create(database, &OBJECT_TYPE);
    mu_assert(table->state == SKY_OBJECT_FILE_STATE_CLOSED, "Expected state initialize as closed");
    mu_assert(table->block_size == SKY_DEFAULT_BLOCK_SIZE, "Expected block size to be reset");

    rc = sky_table_open(table);
    mu_assert(rc == 0, "Table could not be opened");

    mu_assert(table->state == SKY_OBJECT_FILE_STATE_OPEN, "Expected state to be open");
    mu_assert(table->block_count == 9, "Expected 9 blocks");
    mu_assert(table->block_size == 0x10000, "Expected block size to be 64K");

    rc = sky_table_lock(table);
    mu_assert(rc == 0, "Table could not be locked");
    mu_assert(table->state == SKY_OBJECT_FILE_STATE_LOCKED, "Expected state to be locked");

    // Verify lock file.
    rc = stat("tmp/db/users/.lock", &buffer);
    mu_assert(rc == 0, "Expected lock file to exist");

    // Verify block info.
    mu_assert_block_info(0, 1, 1, 3, 1325376000000000LL, 1328054400000000LL, false);
    mu_assert_block_info(1, 8, 4, 5, 1325376000000000LL, 1328054400000000LL, false);
    mu_assert_block_info(2, 0, 6, 6, 1325376000000000LL, 1328054400000000LL, true);
    mu_assert_block_info(3, 3, 6, 6, 1330560000000000LL, 1333238400000000LL, true);
    mu_assert_block_info(4, 5, 6, 6, 1338508800000000LL, 1341100800000000LL, true);
    mu_assert_block_info(5, 2, 7, 9, 1325376000000000LL, 1328054400000000LL, false);
    mu_assert_block_info(6, 4, 10, 10, 1325376000000000LL, 1328054400000000LL, true);
    mu_assert_block_info(7, 7, 10, 10, 1333238400000000LL, 1335830400000000LL, true);
    mu_assert_block_info(8, 6, 10, 10, 1335830400000000LL, 1338508800000000LL, true);

    // Verify properties.
    mu_assert(table->property_count == 3, "Expected 3 properties");
    mu_assert_property(0, 1, "first_name");
    mu_assert_property(1, 2, "last_name");
    mu_assert_property(2, 3, "salary");

    rc = sky_table_unlock(table);
    mu_assert(rc == 0, "Table could not be unlocked");
    mu_assert(table->state == SKY_OBJECT_FILE_STATE_OPEN, "Expected state to be open after unlock");

    // Verify lock is gone.
    rc = stat("tmp/db/users/.lock", &buffer);
    mu_assert(rc == -1, "Expected lock file to not exist");
    mu_assert(errno == ENOENT, "Expected stat error on lock file to be ENOENT");

    rc = sky_table_close(table);
    mu_assert(rc == 0, "Table could not be closed");
    mu_assert(table->state == SKY_OBJECT_FILE_STATE_CLOSED, "Expected state to be closed");
    mu_assert(table->block_size == SKY_DEFAULT_BLOCK_SIZE, "Expected block size to be reset");

    sky_table_free(table);
    sky_database_free(database);

    return 0;
}


//--------------------------------------
// Add events
//--------------------------------------

int test_sky_table_add_event() {
    cleandb();
    
    sky_database *database = sky_database_create(&ROOT);
    sky_table *table = sky_table_create(database, &OBJECT_TYPE);
    table->block_size = 128;

    mu_assert(sky_table_open(table) == 0, "");
    mu_assert(sky_table_lock(table) == 0, "");

    ADD_EVENT(table, 946684800000000LL, 10, 20);
    ADD_EVENT_WITH_DATA2(table, 946684800000000LL, 11, 0, 1, &foo, 2, &bar);
    ADD_EVENT_WITH_DATA1(table, 946688400000000LL, 11, 20, 1, &foo);
    ADD_EVENT_WITH_DATA1(table, 946688400000000LL, 10, 21, 1, &google);
    ADD_EVENT(table, 946692000000000LL, 10, 22);

    // Verify database files.
    mu_assert_file("tmp/db/users/data", "tests/fixtures/table/0/users/data");
    mu_assert_file("tmp/db/users/header", "tests/fixtures/table/0/users/header");
    
    mu_assert(sky_table_unlock(table) == 0, "");
    mu_assert(sky_table_close(table) == 0, "");

    sky_table_free(table);
    sky_database_free(database);
    
    return 0;
}


//--------------------------------------
// Block Split
//--------------------------------------

int test_sky_table_spanned_block_split() {
    cleandb();
    
    sky_database *database = sky_database_create(&ROOT);
    sky_table *table = sky_table_create(database, &OBJECT_TYPE);
    table->block_size = 128;

    mu_assert(sky_table_open(table) == 0, "");
    mu_assert(sky_table_lock(table) == 0, "");

    ADD_EVENT_WITH_DATA1(table, 946688400000000LL, 10, 20, 5, &data50);
    ADD_EVENT_WITH_DATA1(table, 946684800000000LL, 12, 20, 1, &data30);
    ADD_EVENT_WITH_DATA1(table, 946688400000000LL, 11, 20, 1, &data10);
    ADD_EVENT_WITH_DATA1(table, 946692000000000LL, 10, 0, 1, &data50);
    ADD_EVENT_WITH_DATA1(table, 946684800000000LL, 11, 20, 1, &data10);
    ADD_EVENT_WITH_DATA1(table, 946684800000000LL, 10, 0, 2, &data10);

    // Verify block info.
    mu_assert_block_info(0, 0, 10, 10, 946684800000000LL, 946688400000000LL, true);
    mu_assert_block_info(1, 2, 10, 10, 946692000000000LL, 946692000000000LL, true);
    mu_assert_block_info(2, 1, 11, 11, 946684800000000LL, 946688400000000LL, false);
    mu_assert_block_info(3, 3, 12, 12, 946684800000000LL, 946684800000000LL, false);

    // Verify database files.
    mu_assert_file("tmp/db/users/data", "tests/fixtures/db/table_test1/users/data");
    mu_assert_file("tmp/db/users/header", "tests/fixtures/db/table_test1/users/header");
    
    mu_assert(sky_table_unlock(table) == 0, "");
    mu_assert(sky_table_close(table) == 0, "");

    sky_table_free(table);
    sky_database_free(database);
    
    return 0;
}


//==============================================================================
//
// Setup
//
//==============================================================================

int all_tests() {
    mu_run_test(test_sky_table_open);
    mu_run_test(test_sky_table_add_event);
    mu_run_test(test_sky_table_spanned_block_split);
    return 0;
}

RUN_TESTS()