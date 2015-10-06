#include "onebox-common.h"
#include "util-mem.h"
#include "util-unittest.h"

/***********vars************/
static UtTest *ut_list;

/***********funcs************/
/**
 * \brief Allocate UtTest list member
 *
 * \retval ut Pointer to UtTest
 */

static UtTest *UtAllocTest(void) 
{
    UtTest *ut = OBMalloc(sizeof(UtTest));
    if (unlikely(ut == NULL)) return NULL;

    memset(ut, 0, sizeof(UtTest));

    return ut;
}

/**
 * \brief Append test in UtTest list
 *
 * \param list Pointer to the start of the IP packet
 * \param test Pointer to unit test
 *
 * \retval 0 Function always returns zero
 */

static int UtAppendTest(UtTest **list, UtTest *test) 
{
    if (*list == NULL) {
        *list = test;
    } else {
        UtTest *tmp = *list;

        while (tmp->next != NULL) {
            tmp = tmp->next;
        }
        tmp->next = test;
    }

    return 0;
}

/**
 * \brief Register unit test
 *
 * \param name Unit test name
 * \param TestFn Unit test function
 * \param evalue Unit test function return value
 *
 */

void UtRegisterTest(char *name, int(*TestFn)(void), int evalue) 
{
    UtTest *ut = UtAllocTest();
    if (ut == NULL) return;

    ut->name = name;
    ut->TestFn = TestFn;
    ut->evalue = evalue;
    ut->next = NULL;

    /* append */
    UtAppendTest(&ut_list, ut);
}

/**
 * \brief Initialize unit test list
 */

void UtInitialize(void) 
{
    ut_list = NULL;
}

void UtCleanup(void) 
{
    UtTest *tmp = ut_list, *otmp;

    while (tmp != NULL) {
        otmp = tmp->next;
        OBFree(tmp);
        tmp = otmp;
    }

    ut_list = NULL;
}


