/*
 * compiler-version.h
 *
 *  Created on: 2022-9-20
 *      Author: Test
 */

#ifndef SDK_LIB_PLATFORM_INCLUDE_COMPILER_VERSION_H_
#define SDK_LIB_PLATFORM_INCLUDE_COMPILER_VERSION_H_

/* SPDX-License-Identifier: GPL-2.0-only */

#ifdef  __PHY_COMPILER_VERSION_H
#error "Please do not include <linux/compiler-version.h>. This is done by the build system."
#endif
#define __PHY_COMPILER_VERSION_H

/*
 * This header exists to force full rebuild when the compiler is upgraded.
 *
 * When fixdep scans this, it will find this string "CONFIG_CC_VERSION_TEXT"
 * and add dependency on include/config/CC_VERSION_TEXT, which is touched
 * by Kconfig when the version string from the compiler changes.
 */




#endif /* SDK_LIB_PLATFORM_INCLUDE_COMPILER_VERSION_H_ */
