/**
 * Copyright (C) 2026 by Electronya
 *
 * @file      main.c
 * @author    jbacon
 * @date      2026-09-24
 * @brief     App Info Command Tests
 *
 *            Unit tests for the app info shell commands.
 */

#include <zephyr/ztest.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

/* Prevent shell.h */
#define SHELL_H__

/* Provide shell types */
struct shell;

/* Captured shell_print output */
#define MAX_CAPTURED_LINES 8
#define MAX_LINE_LEN       128

static char   captured_lines[MAX_CAPTURED_LINES][MAX_LINE_LEN];
static size_t captured_line_count;

static void shell_print_capture(const struct shell *sh, const char *fmt, ...)
{
  ARG_UNUSED(sh);
  va_list args;

  if(captured_line_count >= MAX_CAPTURED_LINES)
    return;

  va_start(args, fmt);
  vsnprintf(captured_lines[captured_line_count], MAX_LINE_LEN, fmt, args);
  va_end(args);

  captured_line_count++;
}

#define shell_print(sh, fmt, ...) shell_print_capture(sh, fmt, ##__VA_ARGS__)

/* Null out shell registration macros */
#define SHELL_CMD(...)
#define SHELL_STATIC_SUBCMD_SET_CREATE(...)
#define SHELL_SUBCMD_SET_END
#define SHELL_CMD_REGISTER(...)

#include "appInfo.h"

/* Include command implementation */
#include "appInfoCmd.c"

/**
 * @brief Setup function called before all tests in the suite.
 */
static void *appInfoCmd_tests_setup(void)
{
  return NULL;
}

/**
 * @brief Setup function called before each test in the suite.
 */
static void appInfoCmd_tests_before(void *f)
{
  ARG_UNUSED(f);

  memset(captured_lines, 0, sizeof(captured_lines));
  captured_line_count = 0;
}

/* ===========================================================================
 * execName
 * =========================================================================*/

/**
 * @test execName must print the info title and app name, and return 0.
 */
ZTEST(appInfoCmd, test_execName_success)
{
  const struct shell *sh = (const struct shell *)0x1234;
  char *argv[]           = {"name"};
  int result;

  result = execName(sh, 1, argv);

  zassert_equal(result, 0, "execName should return 0");
  zassert_equal(captured_line_count, 2, "execName should print 2 lines");
  zassert_equal(strcmp(captured_lines[0], APP_INFO_TITLE), 0,
                "first line should be the info title");
  zassert_equal(strcmp(captured_lines[1], APP_NAME), 0,
                "second line should be the app name");
}

/* ===========================================================================
 * execVersion
 * =========================================================================*/

/**
 * @test execVersion must print the info title and app version, and return 0.
 */
ZTEST(appInfoCmd, test_execVersion_success)
{
  const struct shell *sh = (const struct shell *)0x1234;
  char *argv[]           = {"version"};
  int result;

  result = execVersion(sh, 1, argv);

  zassert_equal(result, 0, "execVersion should return 0");
  zassert_equal(captured_line_count, 2, "execVersion should print 2 lines");
  zassert_equal(strcmp(captured_lines[0], APP_INFO_TITLE), 0,
                "first line should be the info title");
  zassert_equal(strcmp(captured_lines[1], APP_VERSION), 0,
                "second line should be the app version");
}

/* ===========================================================================
 * execInfo
 * =========================================================================*/

/**
 * @test execInfo must print the info title, app name, and app version, and
 *       return 0.
 */
ZTEST(appInfoCmd, test_execInfo_success)
{
  const struct shell *sh = (const struct shell *)0x1234;
  char *argv[]           = {"info"};
  int result;

  result = execInfo(sh, 1, argv);

  zassert_equal(result, 0, "execInfo should return 0");
  zassert_equal(captured_line_count, 3, "execInfo should print 3 lines");
  zassert_equal(strcmp(captured_lines[0], APP_INFO_TITLE), 0,
                "first line should be the info title");
  zassert_equal(strcmp(captured_lines[1], APP_NAME), 0,
                "second line should be the app name");
  zassert_equal(strcmp(captured_lines[2], APP_VERSION), 0,
                "third line should be the app version");
}

ZTEST_SUITE(appInfoCmd, NULL, appInfoCmd_tests_setup, appInfoCmd_tests_before, NULL, NULL);
