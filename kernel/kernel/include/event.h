#include "semaphore.h"
#include "types.h"

/*
 * event structure
 */
struct phy_event
{
    struct phy_ipc_object parent;                        /**< inherit from ipc_object */

    uint32_t          eventSet;                           /**< event set */
};
typedef struct phy_event *phy_event_t;

/**
 * flag defintions in event
 */
#define PHY_EVENT_FLAG_AND               0x01            /**< logic and */
#define PHY_EVENT_FLAG_OR                0x02            /**< logic or */
#define PHY_EVENT_FLAG_CLEAR             0x04            /**< clear flag */