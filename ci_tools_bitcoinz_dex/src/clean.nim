import os

proc remove_vcpkg()=
    if os.existsDir("vcpkg-repo"):
        os.removeDir("vcpkg-repo")

proc remove_build()=
    if os.existsDir("build-Debug"):
        os.removeDir("build-Debug")
    if os.existsDir("build-Release"):
        os.removeDir("build-Release")

proc clean*(clean_type: string) =
    echo "Cleaning"
    if clean_type == "dependencies":
        remove_vcpkg()
    elif clean_type == "build_dir":
        remove_build()
    elif clean_type == "full":
        remove_vcpkg()
        remove_build()