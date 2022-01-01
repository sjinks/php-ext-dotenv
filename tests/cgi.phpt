--TEST--
dotenv.cli.file (CGI)
--SKIPIF--
<?php require 'skipif.inc'; ?>
<?php if (PHP_VERSION_ID < 70300) die('SKIP PHP 7.3+ required');
--CGI--
--INI--
dotenv.file={PWD}/generic.env
dotenv.cli.file=cli.env
dotenv.cli.use_script_dir=On
--FILE--
<?php
echo getenv('SPECIAL_ENV_VAR'), PHP_EOL;
--EXPECT--
somevalue
