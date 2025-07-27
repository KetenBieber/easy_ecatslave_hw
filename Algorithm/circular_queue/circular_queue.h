/**
 * @file circular_queue.h
 * @author Keten (2863861004@qq.com)
 * @brief 循环队列实现
 * @version 0.1
 * @date 2025-07-15
 *
 * @copyright Copyright (c) 2025
 *
 * @attention :
 * @note :
 * @versioninfo :
 */
#ifndef CIRCULAR_QUEUE_H
#define CIRCULAR_QUEUE_H

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------include-----------------------------------*/

/*----------------------------------typedef-----------------------------------*/
#define DECLARE_CIRCULAR_QUEUE(name, type, cap)                             \
  typedef struct {                                                          \
    type buf[(cap)];                                                        \
    int head, tail;                                                         \
    int capacity;                                                           \
  } name;                                                                   \
  static inline void name##_init(name *q) {                                 \
    q->head = q->tail = 0;                                                  \
    q->capacity = (cap);                                                    \
  }                                                                         \
  static inline int name##_is_empty(name *q) { return q->head == q->tail; } \
  static inline int name##_is_full(name *q) {                               \
    return ((q->tail + 1) % q->capacity) == q->head;                        \
  }                                                                         \
  static inline int name##_enqueue(name *q, type v) {                       \
    int nxt = (q->tail + 1) % q->capacity;                                  \
    if (nxt != q->head) { /* 如果没写满 */                                  \
      q->buf[q->tail] = v;                                                  \
      q->tail = nxt;                                                        \
      return 0; /* 成功入队 */                                              \
    }                                                                       \
    return -1;                                                              \
  }                                                                         \
  static inline int name##_dequeue(name *q, type *out) {                    \
    if (q->head == q->tail) return -1; /* 队列空 */                         \
    *out = q->buf[q->head];                                                 \
    q->head = (q->head + 1) % q->capacity; /* 出队 */                       \
    return 0;                              /* 成功出队 */                   \
  }                                                                         \
  static inline int name##_get_length(name *q) {                            \
    return (q->tail + q->capacity - q->head) % q->capacity;                 \
  }

/*----------------------------------variable----------------------------------*/

/*----------------------------------function----------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* CIRCULAR_QUEUE_H */
