/*
 * Copyright 2017 Icenowy Zheng <icenowy@aosc.xyz>
 *
 * SPDX-License-Identifier: GPL-2.0
 */

#ifndef _CCU_SUN50I_H6_R_H
#define _CCU_SUN50I_H6_R_H

#include <dt-bindings/clock/sun50i-h6-r-ccu.h>
#include <dt-bindings/reset/sun50i-h6-r-ccu.h>

/* AHB/APB bus clocks are not exported except APB1 for R_PIO */
#define CLK_R_AHB	1

#define CLK_R_APB2	3

#define CLK_NUMBER	(CLK_W1 + 1)

#endif /* _CCU_SUN50I_H6_R_H */