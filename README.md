# php-ext-dotenv

[![Build and Test](https://github.com/sjinks/php-ext-dotenv/actions/workflows/test.yml/badge.svg)](https://github.com/sjinks/php-ext-dotenv/actions/workflows/test.yml)
[![Quality Gate Status](https://sonarcloud.io/api/project_badges/measure?project=sjinks_php-ext-dotenv&metric=alert_status)](https://sonarcloud.io/summary/new_code?id=sjinks_php-ext-dotenv)

A PHP extension that sets environment variables from a file on request startup.

## Installation

```bash
phpize
./configure
make
sudo make install
```

## Configuration

|-----------------------|--------|----------------|---------------------------------------------------------------------|
| `dotenv.file`         | string | PHP_INI_PERDIR | Name of the environment file.                                       |
|-----------------------|--------|----------------|---------------------------------------------------------------------|

Example:
```ini
dotenv.file = /path/to/my/.env
```

For CGI and FastCGI, it is possible to set different values for [different hosts and paths](https://www.php.net/manual/en/ini.sections.php):
```ini
[HOST=example.com]
dotenv.file = /path/to/example.com/.env

[HOST=example.org]
dotenv.file = /path/to/example.org/.env

[php]
```

## How It Works

During the request startup, the extension reads the file specified in the `dotenv.file` setting, [parses that file as an INI-file](https://www.php.net/manual/en/function.parse-ini-file.php), and populates the environment with the keys and values read from the file.

The keys from the file will *overwrite* the existing environment variables.

During the request shutdown, all the environment variables added or modified by this extension will be removed from the environment.
