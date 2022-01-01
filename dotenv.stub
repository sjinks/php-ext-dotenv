<?php

/**
 * Parses the env file specified in $filename.
 * See fopen() for the details on $filename and $context.
 *
 * @param string $filename Name of the file to parse.
 * @param resource|null $context An optional context stream resource.
 *
 * @return array<string,string> Parsed variables and their values.
 */
function dotenv_parse_file(string $filename, $context = null): array {}

/**
 * Set the environment variables from $env.
 * These environment variables will be cleared on request shutdown.
 *
 * @param array<string,string> $env Variables to set. Non-string keys and/or values will be silently ignored.
 * @param bool $overwrite Whether to overwrite the existing environment variables.
 *
 * @return void 
 */
function dotenv_setenv(array $env, bool $overwrite = ini_get('dotenv.overwrite')): void {}

/**
 * Search for file $file by walking up the directory tree from $dir until the file is found.
 *
 * @param string $dir Starting directory
 * @param string $file File to find
 * @return null|string Path to the file or null
 */
function dotenv_find_file_upward(string $dir, string $file): ?string {}
