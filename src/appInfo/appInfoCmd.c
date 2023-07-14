/**
 * Copyright (C) 2020 by Electronya
 *
 * @file      appCmd.c
 * @author    jbacon
 * @date      2020-10-07
 * @brief     App Info Command Implementation
 *
 * This file is the implementation of the app info command.
 *
 * @ingroup  appInfo
 * @{
 */

#include <zephyr/shell/shell.h>

#include "appInfo.h"

/**
 * Execute the app name command
 *
 * @param shell     Handle to the shell
 * @param argc      Command argument count
 * @param argv      Pointer to the array of arguments
 *
 * @return 0 if successful, -1 otherwise
 */
static int execName(const struct shell *shell, size_t argc, char **argv)
{
  ARG_UNUSED(argc);
  ARG_UNUSED(argv);

  shell_print(shell, APP_INFO_TITLE);
  shell_print(shell, APP_NAME);

  return 0;
}

/**
 * Execute the app version command
 *
 * @param shell     Handle to the shell
 * @param argc      Command argument count
 * @param argv      Pointer to the array of arguments
 *
 * @return 0 if successful, -1 otherwise
 */
static int execVersion(const struct shell *shell, size_t argc, char **argv)
{
  ARG_UNUSED(argc);
  ARG_UNUSED(argv);

  shell_print(shell, APP_INFO_TITLE);
  shell_print(shell, APP_VERSION);

  return 0;
}

/**
 * Execute the app info command
 *
 * @param shell     Handle to the shell
 * @param argc      Command argument count
 * @param argv      Pointer to the array of arguments
 *
 * @return 0 if successful, -1 otherwise
 */
static int execInfo(const struct shell *shell, size_t argc, char **argv)
{
  ARG_UNUSED(argc);
  ARG_UNUSED(argv);

  shell_print(shell, APP_INFO_TITLE);
  shell_print(shell, APP_NAME);
  shell_print(shell, APP_VERSION);

  return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(app_sub,
	SHELL_CMD(name, NULL, APP_NAME_USAGE, execName),
	SHELL_CMD(version, NULL, APP_VER_UAGE, execVersion),
	SHELL_CMD(info, NULL, APP_INFO_USAGE, execInfo),
	SHELL_SUBCMD_SET_END);
SHELL_CMD_REGISTER(app, &app_sub, APP_CMD_USAGE,	NULL);

/** @} */
