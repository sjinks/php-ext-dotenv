--TEST--
dotenv.cli.file (CGI)
--SKIPIF--
<?php require 'skipif.inc'; ?>
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
