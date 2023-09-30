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
 * @defgroup  buttonMngr button-manager
 *
 * @{
 */

#ifndef BUTTON_MNGR
#define BUTTON_MNGR

/**
 * @brief The button row count.
*/
#define BUTTON_ROW_COUNT        8

/**
 * @brief The button column count.
*/
#define BUTTON_COL_COUNT        4

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
#define LIST_BUTTON_IDX   X(TC_ENC_BTN_IDX,       0, 0)     \
                          X(TC1_ENC_BTN_IDX,      0, 1)     \
                          X(ABS_ENC_BTN_IDX,      0, 2)     \
                          X(MAP_ENC_BTN_IDX,      0, 3)     \
                          X(LEFT_TOP_0_IDX,       0, 4)     \
                          X(LEFT_TOP_1_IDX,       0, 5)     \
                          X(LEFT_TOP_2_IDX,       0, 6)     \
                          X(LEFT_ENC_BTN_IDX,     0, 7)     \
                          X(RIGHT_TOP_0_IDX,      1, 0)     \
                          X(RIGHT_TOP_1_IDX,      1, 1)     \
                          X(RIGHT_TOP_2_IDX,      1, 2)     \
                          X(RIGHT_ENC_BTN_IDX,    1, 3)     \
                          X(LEFT_SHIFTER_IDX,     1, 4)     \
                          X(RIGHT_SHIFTER_IDX,    1, 5)     \
                          X(LEFT_ROCKER_IDX,      1, 6)     \
                          X(RIGHT_ROCKER_IDX,     1, 7)     \
                          X(LEFT_SIDE_0_IDX,      2, 0)     \
                          X(LEFT_SIDE_1_IDX,      2, 1)     \
                          X(LEFT_SIDE_SEL_IDX,    2, 2)     \
                          X(LEFT_SIDE_UP_IDX,     2, 3)     \
                          X(LEFT_SIDE_LEFT_IDX,   2, 4)     \
                          X(LEFT_SIDE_DOWN_IDX,   2, 5)     \
                          X(LEFT_SIDE_RIGHT_IDX,  2, 6)     \
                          X(LEFT_SIDE_2_IDX,      2, 7)     \
                          X(RIGHT_SIDE_0_IDX,     3, 0)     \
                          X(RIGHT_SIDE_1_IDX,     3, 1)     \
                          X(RIGHT_SIDE_SEL_IDX,   3, 2)     \
                          X(RIGHT_SIDE_UP_IDX,    3, 3)     \
                          X(RIGHT_SIDE_LEFT_IDX,  3, 4)     \
                          X(RIGHT_SIDE_DOWN_IDX,  3, 5)     \
                          X(RIGHT_SIDE_RIGHT_IDX, 3, 6)     \
                          X(RIGHT_SIDE_2_IDX,     3, 7)

typedef enum
{
#define X(name, col, row) name = col * BUTTON_ROW_COUNT + row,
  LIST_BUTTON_IDX
#undef X
  TC_INC_IDX,
  TC_DEC_IDX,
  TC1_INC_IDX,
  TC1_DEC_IDX,
  ABS_INC_IDX,
  ABS_DEC_IDX,
  MAP_INC_IDX,
  MAP_DEC_IDX,
  LEFT_ENC_M1_INC_IDX,
  LEFT_ENC_M1_DEC_IDX,
  LEFT_ENC_M2_INC_IDX,
  LEFT_ENC_M2_DEC_IDX,
  BB_INC_IDX,
  BB_DEC_IDX,
  RIGHT_ENC_M2_INC_IDX,
  RIGHT_ENC_M2_DEC_IDX,
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
