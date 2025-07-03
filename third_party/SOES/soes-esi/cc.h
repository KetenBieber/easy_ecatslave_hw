/*
 * Licensed under the GNU General Public License version 2 with exceptions. See
 * LICENSE file in the project root for full license information
 */
/* 
   用于跨平台兼容性和编译器抽象的工具，主要用于处理不同操作系统、编译器和硬件架构之间的差异

*/

#ifndef CC_H
#define CC_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <sys/param.h>
#ifdef __linux__
// 区分Linux 和 其它系统，自动检测主机字节序（大端或小端）
   #include <endian.h>
#else
   #include <machine/endian.h>   
#endif

#ifndef MIN
#define MIN(a,b) (((a)<(b))?(a):(b))
#endif

#ifndef MAX
#define MAX(a,b) (((a)>(b))?(a):(b))
#endif

#define CC_PACKED_BEGIN
#define CC_PACKED_END
#define CC_PACKED       __attribute__((packed)) // 确保结构体紧凑排列，避免编译器填充对齐，适用于硬件寄存器映射或网络协议数据包

#ifdef __rtk__
#define CC_ASSERT(exp) ASSERT (exp)
#else
#define CC_ASSERT(exp) assert (exp)// 封装运行时断言
#endif
#define CC_STATIC_ASSERT(exp) _Static_assert (exp, "") // 进行编译时检查

#define CC_DEPRECATED   __attribute__((deprecated)) // 弃用标记，标记过时代码

#define CC_SWAP32(x) __builtin_bswap32 (x)// 交换32位整数的字节序
#define CC_SWAP16(x) __builtin_bswap16 (x)

// 使用GCC 内置原子操作
#define CC_ATOMIC_SET(var,val)   __atomic_store_n(&var,val,__ATOMIC_SEQ_CST)
#define CC_ATOMIC_GET(var)       __atomic_load_n(&var,__ATOMIC_SEQ_CST)
#define CC_ATOMIC_ADD(var,val)   __atomic_add_fetch(&var,val,__ATOMIC_SEQ_CST)
#define CC_ATOMIC_SUB(var,val)   __atomic_sub_fetch(&var,val,__ATOMIC_SEQ_CST)
#define CC_ATOMIC_AND(var,val)   __atomic_and_fetch(&var,val,__ATOMIC_SEQ_CST)
#define CC_ATOMIC_OR(var,val)    __atomic_or_fetch(&var,val,__ATOMIC_SEQ_CST)

#if BYTE_ORDER == BIG_ENDIAN // 如果是大端字节序，那就改成小端字节序
#define htoes(x) CC_SWAP16 (x)
#define htoel(x) CC_SWAP32 (x)
#else
#define htoes(x) (x)
#define htoel(x) (x)
#endif

#define etohs(x) htoes (x)
#define etohl(x) htoel (x)

#if BYTE_ORDER == LITTLE_ENDIAN
#define EC_LITTLE_ENDIAN
#else
#define EC_BIG_ENDIAN
#endif

#ifdef ESC_DEBUG
#ifdef __rtk__
#include <rprint.h>
#define DPRINT(...) rprintp ("soes: "__VA_ARGS__)
#else
#include <stdio.h>
#define DPRINT(...) printf ("soes: "__VA_ARGS__)
#endif
#else
#define DPRINT(...)
#endif

#ifdef __cplusplus
}
#endif

#endif /* CC_H */
