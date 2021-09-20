import strutils ##! Official Package

import docopt ##! Dependencies Packages

import vcpkg ##! Local packages
import dependencies
import generate
import build
import bundle
import tests
import clean

let doc = """
Atomic Dex CI Tools.

Usage:
  ci_tools_bitcoinz_dex --install_vcpkg
  ci_tools_bitcoinz_dex --install_dependencies
  ci_tools_bitcoinz_dex build (release|debug) [--osx_sdk=<sdk_path>] [--compiler=<compiler_path>]
  ci_tools_bitcoinz_dex clean (full|dependencies|build_dir)
  ci_tools_bitcoinz_dex generate (release|debug) [--osx_sdk=<sdk_path>] [--compiler=<compiler_path>]
  ci_tools_bitcoinz_dex bundle (release|debug) [--osx_sdk=<sdk_path>] [--compiler=<compiler_path>]
  ci_tools_bitcoinz_dex tests (release|debug) [--osx_sdk=<sdk_path>] [--compiler=<compiler_path>]
  ci_tools_bitcoinz_dex --version
  ci_tools_bitcoinz_dex (-h | --help)

Options:
  -h --help     Show this screen.
  --version     Show version.
"""

proc main() =
  let args = docopt(doc, version = "Atomic Dex CI Tools 0.0.1")
  vcpkg_prepare()
  if args["--install_vcpkg"]:
    install_vcpkg()
  elif args["--install_dependencies"]:
    download_packages()
  elif args["generate"]:
    if args["release"]:
      generate_solution("Release", $args["--osx_sdk"], $args["--compiler"])
    elif args["debug"]:
      generate_solution("Debug", $args["--osx_sdk"], $args["--compiler"])
  elif args["build"]:
    if args["release"]:
      build_atomic_qt("Release", $args["--osx_sdk"], $args["--compiler"])
    elif args["debug"]:
      build_atomic_qt("Debug", $args["--osx_sdk"], $args["--compiler"])
  elif args["bundle"]:
    if args["release"]:
      bundle("Release", $args["--osx_sdk"], $args["--compiler"])
    elif args["debug"]:
      bundle("Debug", $args["--osx_sdk"], $args["--compiler"])
  elif args["tests"]:
    if args["release"]:
      run_tests("Release", $args["--osx_sdk"], $args["--compiler"])
    elif args["debug"]:
      run_tests("Debug", $args["--osx_sdk"], $args["--compiler"])
  elif args["clean"]:
    if args["full"]:
      clean("full")
    elif args["dependencies"]:
      clean("dependencies")
    elif args["build_dir"]:  
      clean("build_dir")

when isMainModule:
  main()
