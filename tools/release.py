#!/usr/bin/env python3

'''
Usage:
python3 release.py <new-version>

Updates the TerseLambda project with the new release version string.
'''

from argparse import ArgumentParser
from pathlib import Path
from itertools import chain
import os
from typing import Optional

from semver import VersionInfo


def undecorated(version: VersionInfo) -> str:
    return f'{version.major}.{version.minor}.{version.patch}'


def rewrite_version(s: str, old_version: VersionInfo, new_version: VersionInfo) -> str:
    r = s.replace(str(old_version), str(new_version))
    if r == s:
        r = r.replace(undecorated(old_version), undecorated(new_version))
    return r


def rewrite_file(f: str, old_version: VersionInfo, new_version: VersionInfo) -> Optional[str]:
    with open(f, 'r') as file:
        content = file.read()
    result = rewrite_version(
        content, old_version=old_version, new_version=new_version)
    if result != content:
        with open(f, 'w') as file:
            f.write_text(result)
        return f

    return None


def main(old_version: VersionInfo, new_version: VersionInfo, dir: Path):
    # Walk these subdirectories, gathering all the files in the tree:
    files = chain(
        os.walk(dir / 'examples'),
        os.walk(dir / 'cmake'),
        os.walk(dir / 'docs'),
    )
    # Convert os.walk's tuple format to just be a sequence of Paths.
    files = ((Path(root) / file for file in subfiles)
             for (root, _, subfiles) in files)
    files = chain.from_iterable(files)  # Flatten

    # Also include files in the root directory
    files = chain(files, dir.iterdir())

    # Only want files, not directories
    files = filter(Path.is_file, files)
    # Files inside build/ or _build/ directories should be ignored
    files = filter(lambda f: not {'build', '_build'} & set(f.parts), files)

    changes = set(rewrite_file(f, old_version, new_version) for f in files)
    if None in changes:
        changes.remove(None)

    for file in changes:
        print(file)


if __name__ == '__main__':
    parser = ArgumentParser(
        description='Updates the TerseLambda project with the new release version string')
    parser.add_argument('oldversion', type=VersionInfo.parse,
                        help='The old version string')
    parser.add_argument('newversion', type=VersionInfo.parse,
                        help='The new version string')
    parser.add_argument('--dir', default='.',
                        help='The directory to update')

    args = parser.parse_args()

    main(old_version=args.oldversion,
         new_version=args.newversion, dir=Path(args.dir))
