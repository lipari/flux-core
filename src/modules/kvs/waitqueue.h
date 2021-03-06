#ifndef _FLUX_KVS_WAITQUEUE_H
#define _FLUX_KVS_WAITQUEUE_H

#include <stdbool.h>
#include <flux/core.h>

/* A wait_t represents a waiter, which may be waiting on multiple things.
 * A waitqueue_t represents an object that can can change state and wake up
 * multiple waiters.
 *
 * A wait_t may be added to multiple waitqueue_t's, and multiple wait_t's
 * may be added to a single waitqueue_t.
 *
 * When a wait_t is added to a waitqueue_t, it's usecount is incremented.
 * When one is removed from a waitqueue_t, its usecount is decremented.
 * Once a wait_t's usecount reaches zero, its callback is called and the
 * wait_t is destroyed.
 *
 * When a waitqueue_t is "run", all wait_t's are removed from it.
 */

typedef struct wait_struct wait_t;
typedef struct waitqueue_struct waitqueue_t;

typedef void (*wait_cb_f)(void *arg);

/* Create/destroy/get usecount of a wait_t.
 * Normally a wait_t is destroyed via wait_runqueue().
 */
wait_t *wait_create (wait_cb_f cb, void *arg);
void wait_destroy (wait_t *wait);
int wait_get_usecount (wait_t *wait);

/* Create/destroy/get length of a waitqueue_t.
 */
waitqueue_t *wait_queue_create (void);
void wait_queue_destroy (waitqueue_t *q);
int wait_queue_length (waitqueue_t *q);

/* Add a wait_t to a queue.
 * Returns -1 on error, 0 on success
 */
int wait_addqueue (waitqueue_t *q, wait_t *wait);

/* Remove all wait_t's from the specified queue.
 * Note: wait_runqueue() empties the waitqueue_t before invoking wait_t
 * callbacks for waiters that have a usecount of zero, hence it is safe
 * to manipulate the waitqueue_t from the callback.
 * Returns -1 on error, 0 on success.  On error, all wait_t's on the
 * specified queue remain there.
 */
int wait_runqueue (waitqueue_t *q);

/* Specialized wait_t for restarting message handlers (must be idempotent!).
 * The message handler will be reinvoked once the wait_t usecount reaches zero.
 * Message will be copied and destroyed with the wait_t.
 */
wait_t *wait_create_msg_handler (flux_t *h, flux_msg_handler_t *w,
                                 const flux_msg_t *msg,
                                 flux_msg_handler_f cb, void *arg);

/* Destroy all wait_t's fitting message match critieria, tested with
 * wait_test_msg_f callback.
 * On error, the waitqueue is unaltered.
 */
typedef bool (*wait_test_msg_f)(const flux_msg_t *msg, void *arg);
int wait_destroy_msg (waitqueue_t *q, wait_test_msg_f cb, void *arg);


#endif /* !_FLUX_KVS_WAITQUEUE_H */

/*
 * vi:tabstop=4 shiftwidth=4 expandtab
 */

