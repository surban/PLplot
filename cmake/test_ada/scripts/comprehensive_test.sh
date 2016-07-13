#!/bin/bash

# Complete tests of test_ada for the two generic build types which
# consist of shared library and static library.  These complete tests
# that are run for each build type are (I) in the build tree, build
# the test_noninteractive target for the example and install the binary
# library and the source code for the example and (II) run the test_noninteractive
# target for the installed example.

usage () {
  local prog=`basename $0`
  echo "Usage: $prog [OPTIONS]
OPTIONS:
  The next option specifies the directory prefix which controls where
  all files produced by this script are located.
  [--prefix (defaults to the 'comprehensive_test_ada_disposeable'
                  subdirectory of the directory three levels up from the
                  top-level source-tree directory for test_ada.  Three levels are required
                  to make the parent of comprehensive_test_ada_disposeable the directory
                  one level above the PLplot top-level source-tree directory since the test_ada
                  directory is located two levels below the top-level source-tree directory for
                  PLplot.)]

  The next option controls whether the cmake --trace option is used
  This option produces lots of output so typically \"yes\" is specified only if there is some
  aspect of Ada language support that needs to be debugged on a particular platform.
  [--do_trace (yes/no, defaults to no)]

  The next option controls whether the shared and static
  subdirectories of the prefix tree are initially removed so the
  tarball of all results is guaranteed not to contain stale results.
  Only use no for this option if you want to preserve results from a
  previous run of this script that will not be tested for this run,
  (e.g., if you previously used the option --do_shared yes and are now
  using the option --do_shared no).
  [--do_clean_first (yes/no, defaults to yes)]

  The next three options control the tools used for the configurations, builds, and tests.
  [--cmake_command (defaults to cmake)]
  [--generator_string (defaults to 'Unix Makefiles')]
  [--build_command (defaults to 'make -j4')]

  The next two options control which of the two principal test_ada configurations are tested.
  [--do_shared (yes/no, defaults to yes)]
  [--do_static (yes/no, defaults to yes)]

  The next three control which of two kinds of tests are done for
  each kind of build.
  [--do_test_noninteractive (yes/no, defaults to yes)]
  [--do_test_build_tree (yes/no, defaults to yes)]
  [--do_test_install_tree (yes/no, defaults to yes)]

  [--help] Show this message.
"
  if [ $1 -ne 0 ]; then
      exit $1
  fi
}

collect_exit() {
    # Collect all information in a tarball and exit with return
    # code = $1

    # This function should only be used after prefix,
    # RELATIVE_COMPREHENSIVE_TEST_LOG and RELATIVE_ENVIRONMENT_LOG
    # have been defined.

    return_code=$1
    cd $prefix

    # Clean up stale results before appending new information to the tarball.
    TARBALL=$prefix/comprehensive_test.tar

    rm -f $TARBALL $TARBALL.gz

    # Collect relevant subset of $prefix information in the tarball
    if [ -f $RELATIVE_COMPREHENSIVE_TEST_LOG ] ; then
	tar rf $TARBALL $RELATIVE_COMPREHENSIVE_TEST_LOG
    fi

    if [ -f $RELATIVE_ENVIRONMENT_LOG ] ; then
	tar rf $TARBALL $RELATIVE_ENVIRONMENT_LOG
    fi

    for directory in shared/noninteractive static/noninteractive ; do
	if [ -d $directory/output_tree ] ; then
	    tar rf $TARBALL $directory/output_tree
	fi
	if [ -f $directory/build_tree/CMakeCache.txt ] ; then
	    tar rf $TARBALL $directory/build_tree/CMakeCache.txt
	fi
	if [ -f $directory/install_build_tree/CMakeCache.txt ] ; then
	    tar rf $TARBALL $directory/install_build_tree/CMakeCache.txt
	fi
    done

# Collect listing of every file generated by the script
    find . -type f |xargs ls -l >| comprehensive_test_listing.out
    tar rf $TARBALL comprehensive_test_listing.out

    gzip $TARBALL

    exit $return_code
}

echo_tee() {
# N.B. only useful after this script defines $COMPREHENSIVE_TEST_LOG
echo "$@" |tee -a $COMPREHENSIVE_TEST_LOG
}

comprehensive_test () {
    CMAKE_BUILD_TYPE_OPTION=$1
    TEST_TYPE=$2
    echo_tee "
Running comprehensive_test function with the following variables set:

The variables below are key CMake options which determine the entire
kind of build that will be tested.
CMAKE_BUILD_TYPE_OPTION = $CMAKE_BUILD_TYPE_OPTION
TEST_TYPE = ${TEST_TYPE}

The location below is where all the important *.out files will be found.
OUTPUT_TREE = $OUTPUT_TREE

The location below is the top-level build-tree directory.
BUILD_TREE = $BUILD_TREE

The location below is the top-level install-tree directory.
INSTALL_TREE = $INSTALL_TREE

The location below is the top-level directory of the build tree used
for the CMake-based build and test of the installed examples.
INSTALL_BUILD_TREE = $INSTALL_BUILD_TREE"

    echo_tee "
This variable specifies whether any windows platform has been detected
ANY_WINDOWS_PLATFORM=$ANY_WINDOWS_PLATFORM"

    echo_tee "
Each of the steps in this comprehensive test may take a while...."

    if [ "$CMAKE_BUILD_TYPE_OPTION" != "-DBUILD_SHARED_LIBS=OFF" -a "$IF_MANIPULATE_DYLD_LIBRARY_PATH" = "true" ] ; then
	# This is Mac OS X platform and printenv is available.
	DYLD_LIBRARY_PATH_SAVE=$(printenv | grep DYLD_LIBRARY_PATH)
	# Trim "DYLD_LIBRARY_PATH=" prefix from result.
	DYLD_LIBRARY_PATH_SAVE=${DYLD_LIBRARY_PATH_SAVE#DYLD_LIBRARY_PATH=}
    fi

    PATH_SAVE=$PATH
    mkdir -p "$OUTPUT_TREE"
    # Clean start with nonexistent install tree and empty build tree(s).
    rm -rf "$INSTALL_TREE"
    rm -rf "$BUILD_TREE"
    mkdir -p "$BUILD_TREE"
    if [ "$do_test_install_tree" = "yes" ] ; then
	rm -rf "$INSTALL_BUILD_TREE"
	mkdir -p "$INSTALL_BUILD_TREE"
    fi
    cd "$BUILD_TREE"
    if [ "$do_test_build_tree" = "yes" ] ; then
	BUILD_TEST_OPTION="-DBUILD_TEST=ON"
    else
	BUILD_TEST_OPTION=""
    fi

    if [ "$do_trace" = "yes" ] ; then
	TRACE_OPTION="--trace"
    else
	TRACE_OPTION=""
    fi

    output="$OUTPUT_TREE"/cmake.out
    rm -f "$output"

    if [ "$CMAKE_BUILD_TYPE_OPTION" != "-DBUILD_SHARED_LIBS=OFF" -a "$ANY_WINDOWS_PLATFORM" = "true" ] ; then
	echo_tee "Prepend $BUILD_TREE/dll to the original PATH"
	PATH=$BUILD_TREE/dll:$PATH_SAVE
    fi

    echo_tee "${cmake_command} in the build tree"
    ${cmake_command} "-DCMAKE_INSTALL_PREFIX=$INSTALL_TREE" $BUILD_TEST_OPTION \
		     $CMAKE_BUILD_TYPE_OPTION "$TRACE_OPTION" -G "$generator_string" \
		     "$SOURCE_TREE" >& "$output"
    cmake_rc=$?
    if [ "$cmake_rc" -ne 0 ] ; then
	echo_tee "ERROR: ${cmake_command} in the build tree failed"
	collect_exit 1
    fi

    if [ "$TEST_TYPE" = "noninteractive" ] ; then
	if [ "$do_test_build_tree" = "yes" -a "$do_test_noninteractive" = "yes" ] ; then
	    output="$OUTPUT_TREE"/make_noninteractive.out
	    rm -f "$output"
	    echo_tee "$build_command VERBOSE=1 test_noninteractive in the build tree"
	    $build_command VERBOSE=1 test_noninteractive >& "$output"
	    make_test_noninteractive_rc=$?
	    if [ "$make_test_noninteractive_rc" -ne 0 ] ; then
		echo_tee "ERROR: $build_command VERBOSE=1 test_noninteractive failed in the build tree"
		collect_exit 1
	    fi
	fi

	if [ "$do_test_install_tree" = "yes" ] ; then
	    rm -rf "$INSTALL_TREE"
	    output="$OUTPUT_TREE"/make_install.out
	    rm -f "$output"
	    echo_tee "$build_command VERBOSE=1 install in the build tree"
	    $build_command VERBOSE=1 install >& "$output"
	    make_install_rc=$?
	    if [ "$make_install_rc" -ne 0 ] ; then
		echo_tee "ERROR: $build_command VERBOSE=1 install failed in the build tree"
		collect_exit 1
	    fi
	fi

	if [ "$do_test_install_tree" = "yes" ] ; then
	    echo_tee "Prepend $INSTALL_TREE/bin to the original PATH"
	    PATH="$INSTALL_TREE/bin":$PATH_SAVE

	    if [ "$CMAKE_BUILD_TYPE_OPTION" != "-DBUILD_SHARED_LIBS=OFF" -a "$IF_MANIPULATE_DYLD_LIBRARY_PATH" = "true" ] ; then
		echo_tee "Prepend $INSTALL_TREE/lib to the original DYLD_LIBRARY_PATH"
		if [ -z "$DYLD_LIBRARY_PATH_SAVE" ] ; then
		    export DYLD_LIBRARY_PATH=$INSTALL_TREE/lib
		else
		    export DYLD_LIBRARY_PATH=$INSTALL_TREE/lib:$DYLD_LIBRARY_PATH_SAVE
		fi
		echo_tee "The result is DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH"
	    fi

	    if [ "$do_test_install_tree" = "yes" ] ; then
		cd "$INSTALL_BUILD_TREE"
		output="$OUTPUT_TREE"/installed_cmake.out
		rm -f "$output"
		echo_tee "${cmake_command} in the installed examples build tree"
		${cmake_command} -G "$generator_string" "$TRACE_OPTION" "$INSTALL_TREE"/share/test_ada/examples \
				 >& "$output"
		cmake_rc=$?
		if [ "$cmake_rc" -ne 0 ] ; then
		    echo_tee "ERROR: ${cmake_command} in the installed examples build tree failed"
		    collect_exit 1
		fi
		if [ "$do_test_noninteractive" = "yes" ] ; then
		    output="$OUTPUT_TREE"/installed_make_noninteractive.out
		    rm -f "$output"
		    echo_tee "$build_command VERBOSE=1 test_noninteractive in the installed examples build tree"
		    $build_command VERBOSE=1 test_noninteractive >& "$output"
		    make_rc=$?
		    if [ "$make_rc" -ne 0 ] ; then
			echo_tee "ERROR: $build_command VERBOSE=1 test_noninteractive failed in the installed examples build tree"
			collect_exit 1
		    fi
		fi
	    fi

	fi
    fi

    echo_tee "Restore PATH to its original form"
    PATH=$PATH_SAVE
    if [ "$CMAKE_BUILD_TYPE_OPTION" != "-DBUILD_SHARED_LIBS=OFF" -a "$IF_MANIPULATE_DYLD_LIBRARY_PATH" = "true" ] ; then
	echo_tee "Restore DYLD_LIBRARY_PATH to its original form"
	if [ -z "$DYLD_LIBRARY_PATH_SAVE" ] ; then
	    # For this case no environment variable existed originally.
	    export -n DYLD_LIBRARY_PATH=
	else
	    # For this case the environment variable existed and was equal to DYLD_LIBRARY_PATH_SAVE.
	    export DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH_SAVE
	fi
	echo_tee "The result is DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH"
    fi    
}

# Start of actual script after functions are defined.

# Find absolute PATH of script without using readlink (since readlink is
# not available on all platforms).  Followed advice at
# http://fritzthomas.com/open-source/linux/551-how-to-get-absolute-path-within-shell-script-part2/
ORIGINAL_PATH="$(pwd)"
cd "$(dirname $0)"
# Absolute Path of the script
SCRIPT_PATH="$(pwd)"
cd "${ORIGINAL_PATH}"

# Assumption: top-level source tree is parent directory of where script
# is located.
SOURCE_TREE="$(dirname ${SCRIPT_PATH})"
# This is the parent tree for the BUILD_TREE, INSTALL_TREE,
# INSTALL_BUILD_TREE, and OUTPUT_TREE.  It is disposable.

# Default values for options
prefix="${SOURCE_TREE}/../../../comprehensive_test_ada_disposeable"

do_trace=no

do_clean_first=yes

cmake_command="cmake"
generator_string="Unix Makefiles"
build_command="make -j4"

do_shared=yes
do_static=yes

do_test_noninteractive=yes
do_test_build_tree=yes
do_test_install_tree=yes

usage_reported=0

while test $# -gt 0; do

    case $1 in
        --prefix)
	    prefix=$2
	    shift
	    ;;
        --do_trace)
	    do_trace=$2
	    shift
	    ;;
	--do_clean_first)
	    case $2 in
		yes|no)
		    do_clean_first=$2
		    shift
		    ;;
		
		*)
		    usage 1 1>&2
		    ;;
	    esac
	    ;;
        --cmake_command)
	    cmake_command=$2
	    shift
	    ;;
        --generator_string)
	    generator_string=$2
	    shift
	    ;;
        --build_command)
	    build_command=$2
	    shift
	    ;;
	--do_shared)
	    case $2 in
		yes|no)
		    do_shared=$2
		    shift
		    ;;
		
		*)
		    usage 1 1>&2
		    ;;
	    esac
	    ;;
	--do_static)
	    case $2 in
		yes|no)
		    do_static=$2
		    shift
		    ;;
		*)
		    usage 1 1>&2
		    ;;
	    esac
	    ;;
	--do_test_noninteractive)
	    case $2 in
		yes|no)
		    do_test_noninteractive=$2
		    shift
		    ;;
		*)
		    usage 1 1>&2
		    ;;
	    esac
	    ;;
	--do_test_build_tree)
	    case $2 in
		yes|no)
		    do_test_build_tree=$2
		    shift
		    ;;
		*)
		    usage 1 1>&2
		    ;;
	    esac
	    ;;
	--do_test_install_tree)
	    case $2 in
		yes|no)
		    do_test_install_tree=$2
		    shift
		    ;;
		*)
		    usage 1 1>&2
		    ;;
	    esac
	    ;;
        --help)
            usage 0 1>&2
	    exit 0;
            ;;
        *)
            if [ $usage_reported -eq 0 ]; then
                usage_reported=1
                usage 0 1>&2
                echo " "
            fi
            echo "Unknown option: $1"
            ;;
    esac
    shift
done

if [ $usage_reported -eq 1 ]; then
    exit 1
fi

# Create $prefix directory if it does not exist already
mkdir -p $prefix

# Establish names of output files.  We do this here (as soon as
# possible after $prefix is determined) because
# $COMPREHENSIVE_TEST_LOG affects echo_tee results.
# The relative versions of these are needed for the tar command.
RELATIVE_COMPREHENSIVE_TEST_LOG=comprehensive_test.sh.out
RELATIVE_ENVIRONMENT_LOG=comprehensive_test_env.out
COMPREHENSIVE_TEST_LOG=$prefix/$RELATIVE_COMPREHENSIVE_TEST_LOG
ENVIRONMENT_LOG=$prefix/$RELATIVE_ENVIRONMENT_LOG

# Clean up stale results before appending new information to this file
# with the echo_tee command.
rm -f $COMPREHENSIVE_TEST_LOG

# Set up trap of user interrupts as soon as possible after the above variables
# have been determined and the stale $COMPREHENSIVE_TEST_LOG has been removed.
trap '{ echo_tee "Exiting because of user interrupt" ; collect_exit 1; }' INT

hash git
hash_rc=$?

if [ "$hash_rc" -ne 0 ] ; then
    echo_tee "WARNING: git not on PATH so cannot determine if SOURCE_TREE = 
$SOURCE_TREE is a git repository or not"
else
    cd $SOURCE_TREE
    git_commit_id=$(git rev-parse --short HEAD)
    git_rc=$?
    if [ "$git_rc" -ne 0 ] ; then
	echo_tee "WARNING: SOURCE_TREE = $SOURCE_TREE is not a git repository
 so cannot determine git commit id of the version of PLplot being tested"
    else
	echo_tee "git commit id for the PLplot version being tested = $git_commit_id"
    fi
fi

echo_tee "OSTYPE = ${OSTYPE}"

echo_tee "Summary of options used for these tests

--prefix \"$prefix\"

--do_trace \"$do_trace\"

--cmake_command \"$cmake_command\"
--generator_string \"$generator_string\"
--build_command \"$build_command\"

--do_shared $do_shared
--do_static $do_static

--do_test_noninteractive $do_test_noninteractive
--do_test_build_tree $do_test_build_tree
--do_test_install_tree $do_test_install_tree
"

if [ "$do_clean_first" = "yes" ] ; then
    echo_tee "WARNING: The shared and static subdirectory trees of 
$prefix
are about to be removed!
"
fi
ANSWER=
while [ "$ANSWER" != "yes" -a "$ANSWER" != "no" ] ; do
    echo_tee -n "Continue (yes/no)? "
    read ANSWER
    if [ -z "$ANSWER" ] ; then
	# default of no if no ANSWER
	ANSWER=no
    fi
done
echo_tee ""

if [ "$ANSWER" = "no" ] ; then
    echo_tee "Immediate exit specified!"
    exit
fi

if [ "$do_clean_first" = "yes" ] ; then
    rm -rf $prefix/shared $prefix/static
fi

# Discover if it is a Windows platform or not.
if [[ "$OSTYPE" =~ ^cygwin ]]; then
    ANY_WINDOWS_PLATFORM="true"
elif [[ "$OSTYPE" =~ ^msys ]]; then
    ANY_WINDOWS_PLATFORM="true"
elif [[ "$OSTYPE" =~ ^win ]]; then
    ANY_WINDOWS_PLATFORM="true"
else
    ANY_WINDOWS_PLATFORM="false"
fi

# Discover if it is a Mac OS X platform or not.
if [[ "$OSTYPE" =~ ^darwin ]]; then
    ANY_MAC_OSX_PLATFORM="true"
else
    ANY_MAC_OSX_PLATFORM="false"
fi
    
hash printenv
hash_rc=$?
if [ "$hash_rc" -ne 0 ] ; then
    HAS_PRINTENV=false
else
    HAS_PRINTENV=true
fi

if [ "$ANY_MAC_OSX_PLATFORM" = "true" ] ; then
    if [ "$HAS_PRINTENV" = "true" ] ; then
    IF_MANIPULATE_DYLD_LIBRARY_PATH=true
    else
	echo_tee "WARNING: printenv not on PATH for Mac OS X platform so will not be able to"
	echo_tee "correctly manipulate the DYLD_LIBRARY_PATH environment variable for that platform"
	IF_MANIPULATE_DYLD_LIBRARY_PATH=false
    fi
else
    IF_MANIPULATE_DYLD_LIBRARY_PATH=false
fi

if [ "$HAS_PRINTENV" = "false" ] ; then
    echo_tee "WARNING: printenv not on PATH so not collecting environment variables in $ENVIRONMENT_LOG"
    rm -f $ENVIRONMENT_LOG
else
# Collect selected important environment variable results prior to testing.
    echo "PATH=$PATH" >| $ENVIRONMENT_LOG
    echo "CC=$CC" >> $ENVIRONMENT_LOG
    echo "CXX=$CXX" >> $ENVIRONMENT_LOG
    echo "FC=$FC" >> $ENVIRONMENT_LOG
    printenv |grep -E 'CMAKE_.*PATH|FLAGS|PKG_CONFIG_PATH|LD_LIBRARY_PATH|PLPLOT' >> $ENVIRONMENT_LOG
fi

test_types=

if [ "$do_test_noninteractive" = "yes" ] ; then
    test_types="${test_types} noninteractive"
fi

for test_type in ${test_types} ; do
    # Shared
    if [ "$do_shared" = "yes" ] ; then
	OUTPUT_TREE="$prefix/shared/$test_type/output_tree"
	BUILD_TREE="$prefix/shared/$test_type/build_tree"
	INSTALL_TREE="$prefix/shared/$test_type/install_tree"
	INSTALL_BUILD_TREE="$prefix/shared/$test_type/install_build_tree"
	comprehensive_test "-DBUILD_SHARED_LIBS=ON" $test_type
    fi

    # Static
    if [ "$do_static" = "yes" ] ; then
	OUTPUT_TREE="$prefix/static/$test_type/output_tree"
	BUILD_TREE="$prefix/static/$test_type/build_tree"
	INSTALL_TREE="$prefix/static/$test_type/install_tree"
	INSTALL_BUILD_TREE="$prefix/static/$test_type/install_build_tree"
	comprehensive_test "-DBUILD_SHARED_LIBS=OFF" $test_type
    fi
done

collect_exit 0
