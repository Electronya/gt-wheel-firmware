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

typedef enum
{
  TC_0_SELECT_IDX = 0,                    /**< The TC 0 selection button. */
  TC_1_SELECT_IDX,                        /**< The TC 1 selection button. */
  TC_2_SELECT_IDX,                        /**< The TC 2 selection button. */
  TC_3_SELECT_IDX,                        /**< The TC 3 selection button. */
  TC_4_SELECT_IDX,                        /**< The TC 4 selection button. */
  TC_5_SELECT_IDX,                        /**< The TC 5 selection button. */
  TC_6_SELECT_IDX,                        /**< The TC 6 selection button. */
  TC_7_SELECT_IDX,                        /**< The TC 7 selection button. */
  TC_8_SELECT_IDX,                        /**< The TC 8 selection button. */
  TC_9_SELECT_IDX,                        /**< The TC 9 selection button. */
  TC_10_SELECT_IDX,                       /**< The TC 10 selection button. */
  TC_11_SELECT_IDX,                       /**< The TC 11 selection button. */
  TC1_0_SELECT_IDX,                       /**< The TC1 0 selection button. */
  TC1_1_SELECT_IDX,                       /**< The TC1 1 selection button. */
  TC1_2_SELECT_IDX,                       /**< The TC1 2 selection button. */
  TC1_3_SELECT_IDX,                       /**< The TC1 3 selection button. */
  TC1_4_SELECT_IDX,                       /**< The TC1 4 selection button. */
  TC1_5_SELECT_IDX,                       /**< The TC1 5 selection button. */
  TC1_6_SELECT_IDX,                       /**< The TC1 6 selection button. */
  TC1_7_SELECT_IDX,                       /**< The TC1 7 selection button. */
  TC1_8_SELECT_IDX,                       /**< The TC1 8 selection button. */
  TC1_9_SELECT_IDX,                       /**< The TC1 9 selection button. */
  TC1_10_SELECT_IDX,                      /**< The TC1 10 selection button. */
  TC1_11_SELECT_IDX,                      /**< The TC1 11 selection button. */
  BB_0_SELECT_IDX,                        /**< The BB 0 selection button. */
  BB_1_SELECT_IDX,                        /**< The BB 1 selection button. */
  BB_2_SELECT_IDX,                        /**< The BB 2 selection button. */
  BB_3_SELECT_IDX,                        /**< The BB 3 selection button. */
  BB_4_SELECT_IDX,                        /**< The BB 4 selection button. */
  BB_5_SELECT_IDX,                        /**< The BB 5 selection button. */
  BB_6_SELECT_IDX,                        /**< The BB 6 selection button. */
  BB_7_SELECT_IDX,                        /**< The BB 7 selection button. */
  BB_8_SELECT_IDX,                        /**< The BB 8 selection button. */
  BB_9_SELECT_IDX,                        /**< The BB 9 selection button. */
  BB_10_SELECT_IDX,                       /**< The BB 10 selection button. */
  BB_11_SELECT_IDX,                       /**< The BB 11 selection button. */
  MAP_0_SELECT_IDX,                       /**< The MAP 0 selection button. */
  MAP_1_SELECT_IDX,                       /**< The MAP 1 selection button. */
  MAP_2_SELECT_IDX,                       /**< The MAP 2 selection button. */
  MAP_3_SELECT_IDX,                       /**< The MAP 3 selection button. */
  MAP_4_SELECT_IDX,                       /**< The MAP 4 selection button. */
  MAP_5_SELECT_IDX,                       /**< The MAP 5 selection button. */
  MAP_6_SELECT_IDX,                       /**< The MAP 6 selection button. */
  MAP_7_SELECT_IDX,                       /**< The MAP 7 selection button. */
  MAP_8_SELECT_IDX,                       /**< The MAP 8 selection button. */
  MAP_9_SELECT_IDX,                       /**< The MAP 9 selection button. */
  MAP_10_SELECT_IDX,                      /**< The MAP 10 selection button. */
  MAP_11_SELECT_IDX,                      /**< The MAP 11 selection button. */
  LEFT_TOP_0_IDX,                         /**< The left top first button. */
  LEFT_TOP_1_IDX,                         /**< The left top second button. */
  LEFT_TOP_2_IDX,                         /**< The left top third button. */
  RIGHT_TOP_0_IDX,                        /**< The right top first button. */
  RIGHT_TOP_1_IDX,                        /**< The right top second button. */
  RIGHT_TOP_2_IDX,                        /**< The right top third button. */
  LEFT_SIDE_0_IDX,                        /**< The left side first button. */
  LEFT_SIDE_1_IDX,                        /**< The left side second button. */
  LEFT_SIDE_SEL_IDX,                      /**< The left side select button. */
  LEFT_SIDE_UP_IDX,                       /**< The left side UP button. */
  LEFT_SIDE_LEFT_IDX,                     /**< The left side LEFT button. */
  LEFT_SIDE_DOWN_IDX,                     /**< The left side DOWN button. */
  LEFT_SIDE_RIGHT_IDX,                    /**< The left side RIGHT button. */
  LEFT_SIDE_2_IDX,                        /**< The left side third button. */
  LEFT_ENC_MODE_IDX,                      /**< The left encoder mode button. */
  RIGHT_SIDE_0_IDX,                       /**< The right side first button. */
  RIGHT_SIDE_1_IDX,                       /**< The right side second button. */
  RIGHT_SIDE_SEL_IDX,                     /**< The right side select button. */
  RIGHT_SIDE_UP_IDX,                      /**< The right side UP button. */
  RIGHT_SIDE_LEFT_IDX,                    /**< The right side LEFT button. */
  RIGHT_SIDE_DOWN_IDX,                    /**< The right side DOWN button. */
  RIGHT_SIDE_RIGHT_IDX,                   /**< The right side RIGHT button. */
  RIGHT_SIDE_2_IDX,                       /**< The right side third button. */
  RIGHT_ENC_MODE_IDX,                     /**< The left encoder mode button. */
  SHIFTER_RIGHT_IDX,                      /**< The right shifter button index. */
  SHIFTER_LEFT_IDX,                       /**< The left shifter button index. */
  ROCKER_RIGHT_IDX,                       /**< The right rocker button index. */
  ROCKER_LEFT_IDX,                        /**< The left rocker button index. */
  BUTTON_COUNT,                           /**< The total button count. */
} ButtonIdx;

/**
 * @brief The button state.
*/
typedef enum
{
  BUTTON_DEPRESSED = 0,                   /**< The button depressed state. */
  BUTTON_PRESSED,                         /**< The button pressed state. */
} WheelButtonState;

/**
 * @brief Initialize the button manager.
 *
 * @return 0 if success, the error code otherwise.
 */
int buttonMngrInit(void);

#endif    /* BUTTON_MNGR */

/** @} */
