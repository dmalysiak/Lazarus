/*
 * Defines.h
 *
 *  Created on: 04.08.2011
 *      Author: nao
 */

#ifndef DEFINES_H_
#define DEFINES_H_

#define CLEAR(x) memset (&(x), 0, sizeof (x))

typedef enum {
        IO_METHOD_READ,
        IO_METHOD_MMAP,
        IO_METHOD_USERPTR,
} io_method;

#endif /* DEFINES_H_ */
