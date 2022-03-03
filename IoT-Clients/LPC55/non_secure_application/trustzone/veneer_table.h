/*
 * Copyright 2018 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _VENEER_TABLE_
#define _VENEER_TABLE_

/*******************************************************************************
 * Definitions
 ******************************************************************************/
typedef int (*callbackptr)(char const *s1, char const *s2);

/* NOTE: These defines are not related to veneer table. But since they are needed
 *       in both secure and non-secure project, they are placed here */
#define FAULT_NONE                0
#define FAULT_INV_S_TO_NS_TRANS   1
#define FAULT_INV_S_ENTRY         2
#define FAULT_INV_NS_DATA_ACCESS  3
#define FAULT_INV_INPUT_PARAMS    4
#define FAULT_INV_NS_DATA2_ACCESS 5

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/*!
 * @brief Entry function for debug PRINTF (DbgConsole_Printf)
 *
 * This function provides interface between secure and normal worlds
 * This function is called from normal world only
 *
 * @param s     String to be printed
 *
 */
void DbgConsole_Printf_NSE(char const *s);

/*!
 * @brief Entry function for two string comparison
 *
 * This function compares two strings by using non-secure callback
 * This function is called from normal world only
 *
 * @param callback     pointer to strcmp() function
 * @param s1           String to be compared
 * @param s2           String to be compared
 * @return             result of strcmp function
 *                     >0 for s1 > s2
 *                     =0 for s1 = s2
 *                     <0 for s1 < s2
 */
uint32_t StringCompare_NSE(volatile callbackptr callback, char const *s1, char const *s2);


void trustQuerry(char *trustor, char *trustee, uint8_t *output, int len);
void submitEvidenceVeneer(char *blockID, uint8_t *output, int len);
void checkRequest(char *output, int len);
//void trustQuerry2(char *output, int len);
#endif /* _VENEER_TABLE_ */