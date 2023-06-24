//
// Created by selischev on 16.04.23.
//
#ifndef ITMO_C_PNG_FEDORRRS_LIB_DECIDER_H
#define ITMO_C_PNG_FEDORRRS_LIB_DECIDER_H

#define ISAL

#ifdef ZLIB
#include <zlib.h>
#elif defined(LIBDEFLATE)
#include <libdeflate.h>
#elif defined(ISAL)
// #include <include/igzip_lib.h>
#include <isa-l.h>
/* #include <isa-l.h> standard including not working on my linux machine :( */
#else
#error "This library is not supported"
#endif

#endif	  // ITMO_C_PNG_FEDORRRS_LIB_DECIDER_H
