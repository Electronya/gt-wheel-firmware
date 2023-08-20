/**
 * Copyright (C) 2023 by Electronya
 *
 * @file      buttonMngr.h
 * @author    jbacon
 * @date      2023-07-31
 * @brief     Button Manager Module
 *
 *            This file is the declaration of the button manager module.
 *
 * @defgroup  buttonMngr
 *
 * @{
 */

#ifndef BUTTON_MNGR
#define BUTTON_MNGR

/**
 * @brief The button row count.
*/
#define BUTTON_ROW_COUNT        12

/**
 * @brief The button column count.
*/
#define BUTTON_COL_COUNT        7

/**
 * @brief The shifter button count.
*/
#define BUTTON_SHIFTER_COUNT    2

/**
 * @brief The rocker button count.
*/
#define BUTTON_ROCKER_COUNT     2

/**
 * @brief The encoder button count.
*/
#define BUTTON_ENCODER_COUNT    2

/**
 * @brief The button index x(button name, button column, button row).
*/
#define LIST_BUTTON_IDX   X(TC_0_SELECT_IDX,      0, 0)     \
                          X(TC_1_SELECT_IDX,      0, 1)     \
                          X(TC_2_SELECT_IDX,      0, 2)     \
                          X(TC_3_SELECT_IDX,      0, 3)     \
                          X(TC_4_SELECT_IDX,      0, 4)     \
                          X(TC_5_SELECT_IDX,      0, 5)     \
                          X(TC_6_SELECT_IDX,      0, 6)     \
                          X(TC_7_SELECT_IDX,      0, 7)     \
                          X(TC_8_SELECT_IDX,      0, 8)     \
                          X(TC_9_SELECT_IDX,      0, 9)     \
                          X(TC_10_SELECT_IDX,     0, 10)    \
                          X(TC_11_SELECT_IDX,     0, 11)    \
                          X(TC1_0_SELECT_IDX,     1, 0)     \
                          X(TC1_1_SELECT_IDX,     1, 1)     \
                          X(TC1_2_SELECT_IDX,     1, 2)     \
                          X(TC1_3_SELECT_IDX,     1, 3)     \
                          X(TC1_4_SELECT_IDX,     1, 4)     \
                          X(TC1_5_SELECT_IDX,     1, 5)     \
                          X(TC1_6_SELECT_IDX,     1, 6)     \
                          X(TC1_7_SELECT_IDX,     1, 7)     \
                          X(TC1_8_SELECT_IDX,     1, 8)     \
                          X(TC1_9_SELECT_IDX,     1, 9)     \
                          X(TC1_10_SELECT_IDX,    1, 10)    \
                          X(TC1_11_SELECT_IDX,    1, 11)    \
                          X(BB_0_SELECT_IDX,      2, 0)     \
                          X(BB_1_SELECT_IDX,      2, 1)     \
                          X(BB_2_SELECT_IDX,      2, 2)     \
                          X(BB_3_SELECT_IDX,      2, 3)     \
                          X(BB_4_SELECT_IDX,      2, 4)     \
                          X(BB_5_SELECT_IDX,      2, 5)     \
                          X(BB_6_SELECT_IDX,      2, 6)     \
                          X(BB_7_SELECT_IDX,      2, 7)     \
                          X(BB_8_SELECT_IDX,      2, 8)     \
                          X(BB_9_SELECT_IDX,      2, 9)     \
                          X(BB_10_SELECT_IDX,     2, 10)    \
                          X(BB_11_SELECT_IDX,     2, 11)    \
                          X(MAP_0_SELECT_IDX,     3, 0)     \
                          X(MAP_1_SELECT_IDX,     3, 1)     \
                          X(MAP_2_SELECT_IDX,     3, 2)     \
                          X(MAP_3_SELECT_IDX,     3, 3)     \
                          X(MAP_4_SELECT_IDX,     3, 4)     \
                          X(MAP_5_SELECT_IDX,     3, 5)     \
                          X(MAP_6_SELECT_IDX,     3, 6)     \
                          X(MAP_7_SELECT_IDX,     3, 7)     \
                          X(MAP_8_SELECT_IDX,     3, 8)     \
                          X(MAP_9_SELECT_IDX,     3, 9)     \
                          X(MAP_10_SELECT_IDX,    3, 10)    \
                          X(MAP_11_SELECT_IDX,    3, 11)    \
                          X(LEFT_TOP_0_IDX,       4, 0)     \
                          X(LEFT_TOP_1_IDX,       4, 1)     \
                          X(LEFT_TOP_2_IDX,       4, 2)     \
                          X(RIGHT_TOP_0_IDX,      4, 3)     \
                          X(RIGHT_TOP_1_IDX,      4, 4)     \
                          X(RIGHT_TOP_2_IDX,      4, 5)     \
                          X(RESERVED_1,           4, 6)     \
                          X(RESERVED_2,           4, 7)     \
                          X(RESERVED_3,           4, 8)     \
                          X(RESERVED_4,           4, 9)     \
                          X(RESERVED_5,           4, 10)    \
                          X(RESERVED_6,           4, 11)    \
                          X(LEFT_SIDE_0_IDX,      5, 0)     \
                          X(LEFT_SIDE_1_IDX,      5, 1)     \
                          X(LEFT_SIDE_SEL_IDX,    5, 2)     \
                          X(LEFT_SIDE_UP_IDX,     5, 3)     \
                          X(LEFT_SIDE_LEFT_IDX,   5, 4)     \
                          X(LEFT_SIDE_DOWN_IDX,   5, 5)     \
                          X(LEFT_SIDE_RIGHT_IDX,  5, 6)     \
                          X(LEFT_SIDE_2_IDX,      5, 7)     \
                          X(LEFT_ENC_MODE_IDX,    5, 8)     \
                          X(RESERVED_7,           5, 9)     \
                          X(RESERVED_8,           5, 10)    \
                          X(RESERVED_9,           5, 11)    \
                          X(RIGHT_SIDE_0_IDX,     6, 0)     \
                          X(RIGHT_SIDE_1_IDX,     6, 1)     \
                          X(RIGHT_SIDE_SEL_IDX,   6, 2)     \
                          X(RIGHT_SIDE_UP_IDX,    6, 3)     \
                          X(RIGHT_SIDE_LEFT_IDX,  6, 4)     \
                          X(RIGHT_SIDE_DOWN_IDX,  6, 5)     \
                          X(RIGHT_SIDE_RIGHT_IDX, 6, 6)     \
                          X(RIGHT_SIDE_2_IDX,     6, 7)     \
                          X(RIGHT_ENC_MODE_IDX,   6, 8)     \
                          X(RESERVED_10,          6, 9)     \
                          X(RESERVED_11,          6, 10)    \
                          X(RESERVED_12,          6, 11)

typedef enum
{
  #define X(name, col, row) name = col * BUTTON_ROW_COUNT + row,
    LIST_BUTTON_IDX
  #undef X
  LEFT_SHIFTER_IDX,
  RIGHT_SHIFTER_IDX,
  LEFT_ROCKER_IDX,
  RIGHT_ROCKER_IDX,
  BUTTON_COUNT,
} WheelButtonIdx;

/**
 * @brief The button state.
*/
typedef enum
{
  BUTTON_DEPRESSED = 0,                   /**< The button depressed state. */
  BUTTON_PRESSED,                         /**< The button pressed state. */
} WheelButtonState;

/**
 * @brief   Initialize the button manager.
 *
 * @return  0 if success, the error code otherwise.
 */
int buttonMngrInit(void);

/**
 * @brief   Get all the current button states.
 *
 * @param states  All the current button states.
 * @param count   The count of button states to get.
 *
 * @return  0 if successful, the error code otherwise.
 */
int buttonMngrGetAllStates(WheelButtonState *states, size_t count);

/**
 * @brief   Get the encoder button states.
 *
 * @param states  The encoder button states.
 * @param count   The count of button states to get.
 *
 * @return  0 if successful, the error code otherwise.
 */
int buttonMngrGetEncoderStates(WheelButtonState *states, size_t count);

// TODO: implement the thread + mutex.

#endif    /* BUTTON_MNGR */

/** @} */
