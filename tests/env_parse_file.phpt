--TEST--
env_parse_file()
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php
$arr = env_parse_file(__DIR__ . '/parse.env');
var_export($arr);
--EXPECT--
array (
  'BASIC' => 'basic',
  'AFTER_LINE' => 'after_line',
  'EMPTY' => '',
  'SINGLE_QUOTES' => 'single_quotes',
  'SINGLE_QUOTES_SPACED' => '    single quotes    ',
  'DOUBLE_QUOTES' => 'double_quotes',
  'DOUBLE_QUOTES_SPACED' => '    double quotes    ',
  'EXPAND_NEWLINES' => 'expand
new
lines',
  'DONT_EXPAND_UNQUOTED' => 'dontexpand\\nnewlines',
  'DONT_EXPAND_SQUOTED' => 'dontexpand\\nnewlines',
  'EQUAL_SIGNS' => 'equals==',
  'RETAIN_INNER_QUOTES' => '{"foo": "bar"}',
  'RETAIN_LEADING_DQUOTE' => '"retained',
  'RETAIN_LEADING_SQUOTE' => '\'retained',
  'RETAIN_TRAILING_DQUOTE' => 'retained"',
  'RETAIN_TRAILING_SQUOTE' => 'retained\'',
  'RETAIN_INNER_QUOTES_AS_STRING' => '{"foo": "bar"}',
  'TRIM_SPACE_FROM_UNQUOTED' => 'some spaced out string',
  'USERNAME' => 'therealnerdybeast@example.tld',
  'SPACED_KEY' => 'parsed',
  'ANOTHER_SPACED_KEY' => 'trailing spaces after value',
  'EMPTY_WITH_TRAILING_SPACES' => '',
)
