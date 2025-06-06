# Contributing guide

**Want to contribute? Great!**

All contributions are more than welcome ! This includes bug reports, bug fixes, enhancements, features, questions, ideas,
and documentation.

This document will hopefully help you contribute to pgmoneta.

* [Legal](#legal)
* [Reporting an issue](#reporting-an-issue)
* [Setup your build environment](#setup-your-build-environment)
* [Building the main branch](#building-the-main-branch)
* [Before you contribute](#before-you-contribute)
* [Code reviews](#code-reviews)
* [Coding Guidelines](#coding-guidelines)
* [Discuss a Feature](#discuss-a-feature)
* [Development](#development)
* [Code Style](#code-style)

## Legal

All contributions to pgmoneta are licensed under the [The 3-Clause BSD License](https://opensource.org/licenses/BSD-3-Clause).

## Reporting an issue

This project uses GitHub issues to manage the issues. Open an issue directly in GitHub.

If you believe you found a bug, and it's likely possible, please indicate a way to reproduce it, what you are seeing and what you would expect to see.
Don't forget to indicate your pgmoneta version.

## Setup your build environment

You can use the follow command, if you are using a [Fedora](https://getfedora.org/) based platform:

```
dnf install git gcc clang clang-analyzer cmake make libev libev-devel openssl openssl-devel systemd systemd-devel zlib zlib-devel libzstd libzstd-devel lz4 lz4-devel libssh libssh-devel python3-docutils libatomic bzip2 bzip2-devel libarchive libarchive-devel libasan libasan-static
```

in order to get the necessary dependencies.

## Building the main branch

To build the `main` branch:

```
git clone https://github.com/pgmoneta/pgmoneta.git
cd pgmoneta
mkdir build
cd build
cmake -DCMAKE_C_COMPILER=clang -DCMAKE_BUILD_TYPE=Debug ..
make
cd src
cp ../../doc/etc/*.conf .
./pgmoneta -c pgmoneta.conf -u pgmoneta_users.conf
```

and you will have a running instance after you have created the `pgmoneta_users.conf` file.

## Before you contribute

To contribute, use GitHub Pull Requests, from your **own** fork.

Also, make sure you have set up your Git authorship correctly:

```
git config --global user.name "Your Full Name"
git config --global user.email your.email@example.com
```

We use this information to acknowledge your contributions in release announcements.

## Code reviews

GitHub pull requests can be reviewed by all such that input can be given to the author(s).

See [GitHub Pull Request Review Process](https://docs.github.com/en/pull-requests/collaborating-with-pull-requests/reviewing-changes-in-pull-requests/about-pull-request-reviews)
for more information.

## Coding Guidelines

* Discuss the feature
* Do development
  + Follow the code style
* Commits should be atomic and semantic. Therefore, squash your pull request before submission and keep it rebased until merged
  + If your feature has independent parts submit those as separate pull requests

## Discuss a Feature

You can discuss bug reports, enhancements and features in our [forum](https://github.com/pgmoneta/pgmoneta/discussions).

Once there is an agreement on the development plan you can open an issue that will used for reference in the pull request.

## Development

You can follow this workflow for your development.

Add your repository

```
git clone git@github.com:yourname/pgmoneta.git
cd pgmoneta
git remote add upstream https://github.com/pgmoneta/pgmoneta.git
```

Create a work branch

```
git checkout -b mywork main
```

During development

```
git commit -a -m "[#issue] My feature"
git push -f origin mywork
```

If you have more commits then squash them

```
git rebase -i HEAD~2
git push -f origin mywork
```

If the `main` branch changes then

```
git fetch upstream
git rebase -i upstream/main
git push -f origin mywork
```

as all pull requests should be squashed and rebased.

In your first pull request you need to add yourself to the

```
AUTHORS
doc/manual/97-acknowledgement.md
doc/manual/advanced/97-acknowledgement.md
```

files.

## Code Style

Please, follow the coding style of the project.

You can use the [uncrustify](http://uncrustify.sourceforge.net/) tool to help with the formatting, by running

```
./uncrustify.sh
```

and verify the changes.
