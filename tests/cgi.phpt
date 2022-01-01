--TEST--
dotenv.cli.file (CGI)
--SKIPIF--
<?php require 'skipif.inc'; ?>
<?php if (PHP_SAPI === 'cli') die('SKIP cannot run under cli SAPI');
--INI--
dotenv.file={PWD}/generic.env
dotenv.cli.file=cli.env
dotenv.cli.use_script_dir=On
--CGI--
1
--FILE--
<?php
echo getenv('SPECIAL_ENV_VAR'), PHP_EOL;
--EXPECT--
somevalue
