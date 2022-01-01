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
