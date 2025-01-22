# Myrlyn Package Manager GUI for Linux

_(Formerly known as YQPkg)_

Myrlyn is a graphical package manager to select software packages and patterns
for installation, update and removal. It uses _libzypp_ as its backend and Qt
as its GUI toolkit.

This started in the 11/2024 SUSE Hack Week to make the SUSE YaST Qt package
selector usable as a standalone Qt program without any YaST dependencies,
but it has grown beyond that since then.

[<img src="https://raw.githubusercontent.com/shundhammer/yqpkg/refs/heads/master/src/artwork/Myrlyn-256x256.png">](https://raw.githubusercontent.com/shundhammer/yqpkg/refs/heads/master/src/artwork/Myrlyn-256x256.png)


## License

GPL V2; see file LICENSE.


## Development Status

Late Alpha.

It is now well usable and fairly feature complete.


## Screenshots

[<img width="500" src="https://github.com/shundhammer/myrlyn/blob/master/screenshots/Myrlyn-10-search-view.png">](https://raw.githubusercontent.com/shundhammer/myrlyn/refs/heads/master/screenshots/Myrlyn-10-search-view.png)


[<img width="250" src="https://github.com/shundhammer/myrlyn/blob/master/screenshots/Myrlyn-01-init-repos.png">](https://raw.githubusercontent.com/shundhammer/myrlyn/refs/heads/master/screenshots/Myrlyn-01-init-repos.png)
[<img width="250" src="https://github.com/shundhammer/myrlyn/blob/master/screenshots/Myrlyn-11-updates-view.png">](https://raw.githubusercontent.com/shundhammer/myrlyn/refs/heads/master/screenshots/Myrlyn-11-updates-view.png)
[<img width="250" src="https://github.com/shundhammer/myrlyn/blob/master/screenshots/Myrlyn-20-pkg-commit-default.png">](https://raw.githubusercontent.com/shundhammer/myrlyn/refs/heads/master/screenshots/Myrlyn-20-pkg-commit-default.png)

[<img width="250" src="https://github.com/shundhammer/myrlyn/blob/master/screenshots/Myrlyn-21-pkg-commit-details-1.png">](https://raw.githubusercontent.com/shundhammer/myrlyn/refs/heads/master/screenshots/Myrlyn-21-pkg-commit-details-1.png)
[<img width="250" src="https://github.com/shundhammer/myrlyn/blob/master/screenshots/Myrlyn-22-pkg-commit-details-2.png">](https://raw.githubusercontent.com/shundhammer/myrlyn/refs/heads/master/screenshots/Myrlyn-22-pkg-commit-details-2.png)
[<img width="250" src="https://github.com/shundhammer/myrlyn/blob/master/screenshots/Myrlyn-30-summary.png">](https://raw.githubusercontent.com/shundhammer/myrlyn/refs/heads/master/screenshots/Myrlyn-30-summary.png)



## Features

### Features Taken over from YaST

- Repos are automatically refreshed at the start of the program with some
  visual feedback what is going on.

- Search for packages:
  - by name, summary, description
  - by RPM provides and requires
  - in their file list (installed packages only)

- Package details views:
  - Package description
  - Technical details (`rpm -qi`)
  - Dependencies (RPM provides, requires, recommends, ...)
  - Versions (installed and available ones from all active repositories)
  - File list (installed packages only)
  - Change log (installed packages only)

- Install, update and remove packages
- Pick a specific package version for installation or update
- Install and remove patterns
- Install and remove patches
- Browse and select packages by repository
- Browse and select language/locale-specific packages by language/locale
- Browse and select packages by classification:
  - Suggested packages
  - Recommended packages
  - Orphaned packages
  - Unneeded packages
  - Retracted packages
  - Retracted installed packages
  - All packages


### New Features in Myrlyn (No YaST Counterpart)

- "Updates" view that shows packages that could be updated (where a newer
  version than the installed one is available)

  - Update all packages that can be updated without a dependency problem (the
    same as `zypper up`)

  - Dist Upgrade (the same as `zypper dup`): Take package splits and renames,
    pattern updates and more into account and update all packages that can be
    updated without a dependency problem

- **Read-only mode** for non-root users: You can search packages, view their
  details, even experiment what would happen if you tried to install or remove
  them and see the dependency resolver results.

  You can do everything except actually apply any changes.

- During the commit phase (where packages are actually installed, updated or
  removed), in addition to the large progress bar, you can now also switch to a
  details view to see which packages are waiting, downloaded, being processed,
  or finished.

- You can decide during the commit phase whether or not you want to see a
  summary page as the next step. That summary page exits the program after a
  selectable countdown (30 seconds by default), but you can go back to the
  package selector to mark more packages for installation, update or removal.

  It's now very easy to keep working with the package selection and install,
  update or remove a few packages at a time and then do some more without fear
  that a lot of work might be lost when you get into a large dependency problem
  cycle.


### Fixes

- Improved performance during startup as well as during package selection.

- Improved visual appearance; we don't need to "pixel pinch" anymore with
  today's screen resolutions, unlike back when the YaST package selector
  was created.

- The annoying tendency to get too narrow columns in the package list is now
  fixed. You will find yourself very rarely adjusting the column widths
  manually.

- The package list is now filled automatically (as it should always have) when
  appropriate; you don't need to click on the left "filter" pane anymore to
  make that happen. See also
  [issue #10](https://github.com/shundhammer/myrlyn/issues/10).

- Similar with the details views (bottom right): They are now shown
  automatically when a package is selected, no matter if manually or
  programmatically. Previously, you had to click on the automatically selected
  package again to see anything there. This is also explained in
  [issue #10](https://github.com/shundhammer/myrlyn/issues/10).

- Reasonable initial window sizes for the main window as well as for the many
  pop-up dialogs.

- Streamlined keyboard shortcuts for most filter views and the tabs.

- Lots of other small fixes everywhere.



## Limitations

- ~~No GPG key import from repos yet for repos that were just added and never
  refreshed yet.~~  _This is now implemented_. See
  [issue #6](https://github.com/shundhammer/myrlyn/issues/6) for details.

- No repo operations like adding, removing, setting priority, enabling,
  disabling.  You get the repo view, though, where you can view and select
  packages by repo.


### Work in Progress

- ~Patches view~ _This is now implemented_.
See [issue #2](https://github.com/shundhammer/myrlyn/issues/2) for details.


## Development Progress

See [issue #1](https://github.com/shundhammer/myrlyn/issues/1).

Scroll down all the way to for the latest news.


## Stability

It's very stable and usable now.

Of course, since it's still in development, the occasional bug may appear, but
it should now only be minor ones. See also the
[open issues](https://github.com/shundhammer/myrlyn/issues)
in the GitHub issue tracker.

I have been using it exclusively since Christmas 2024 for all my package
operations, including keeping my Slowroll laptop, my Leap 15.6 desktop PC and
my Tumbleweed virtual machine up to date. There was _never_ any real problem,
especially not during the commit phase when packages are actually installed,
updated or removed.


## Ready-Made Packages

The latest version from Git: [myrlyn-git](https://software.opensuse.org/download/package?package=myrlyn&project=home%3Ashundhammer%3Amyrlyn-git)


## Motivation: Why?

_Explained in greater detail at the Hack Week 11/2024
[YQPkg project page](https://hackweek.opensuse.org/24/projects/yqpkg-bringing-the-single-package-selection-back-to-life)_

YaST will be phased out soon in favor of Agama and Cockpit, and then there will
be a huge gap between low-level `zypper in` and high-level application
installers of the desktop environments; those know and promote their own
desktop's video post-production software, but what about other packages?

How do you browse through all the existing packages of a large distro like
Tumbleweed or Leap? How do you find software about some specific topic?

How do you install a different kernel version from a specific repo when your
new laptop doesn't want to suspend?

How do you help a friend who asks you what software openSUSE has to offer for
his HAM radio or his music band?

How do you find language-specific packages for your native Estonian?

Right now, you use YaST sw_single. What if there is no YaST anymore? That's
where this project comes in.


## Is this an Official SUSE Project?

No. Maybe it will become one, though. It started as a one-man project during
the SUSE Hack Week in 11/2024, and it's been a mixture of private Open Source
project and a SUSE-sponsored one at the same time.

Hopefully SUSE will recognize the importance of this and keep sponsoring it.


## Build Requirements

You need at least a C++ and Qt5 development environment plus CMake and
libzypp-devel.

```
sudo zypper install -t pattern devel_C_C++ devel_qt5
sudo zypper install cmake libzypp-devel
```


## Building

```
make -f Makefile.repo
cd build
make
```

### Installing

After building, add

```
sudo make install
```

### Cleaning up a Build

```
rm -rf build
```

### Starting the Built Executable

From the build directory (without `sudo make install`):

```
cd build
src/myrlyn
```

After `sudo make install`:

```
myrlyn
```

(it's now in `/usr/bin/myrlyn`)


## Reference

- Development status and screenshots: [issue #1](https://github.com/shundhammer/myrlyn/issues/1)

- [Original project](https://github.com/libyui/libyui) that this was forked from
- [2024/11 SUSE Hack Week Project](https://hackweek.opensuse.org/24/projects/yqpkg-bringing-the-single-package-selection-back-to-life)
