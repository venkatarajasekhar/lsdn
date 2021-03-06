#!/bin/bash

NETNS="lsdn-test"
VETH_PREFIX="ltif"
MAC_PREFIX="00:00:00:00:00:"
IPV4_PREFIX="192.168.59."

ENDPOINTS="1 2 3 4 5 6 7"

MAIN_NS="ip netns exec $NETNS"

function IN_NS() {
	id=$1
	shift
	NSNAME="${NETNS}-sub$id"
	ip netns exec $NSNAME "$@"
}

function ping_from() {
	src=$1
	dst=$2
	IN_NS $src ping -c 1 -w 5 $IPV4_PREFIX$dst
}

function test_error() {
	echo "^^^ TEST FAILED ^^^"
	$cleanup_handler
	exit 1
}

function fail() {	
	if trace_command "$@"; then
		test_error
	fi
}

function test() {
	if ! trace_command "$@"; then
		test_error
	fi
}

function trace_command {
	echo "> $@"
	"$@"
	return $?
}
