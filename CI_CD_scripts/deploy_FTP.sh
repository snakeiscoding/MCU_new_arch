#!/bin/bash
#
# Deploy whichever targets built successfully, then fail the step if any
# expected target is missing. BUILD_STATUS.txt records the result per target.
#
# Usage: deploy_FTP.sh <target> [<target> ...]

set -euo pipefail

if [ "$#" -lt 1 ]; then
    echo "Usage: $0 <target> [<target> ...]" >&2
    exit 1
fi

FTP_ENV_FILE="${FTP_ENV_FILE:-/home/bitbucket_runner/.ci-private/ftp.env}"
if [ ! -r "${FTP_ENV_FILE}" ]; then
    echo "ERROR: FTP credential file not readable: ${FTP_ENV_FILE}" >&2
    exit 1
fi
source "${FTP_ENV_FILE}"
: "${FTP_USER:?}" "${FTP_PASSWORD:?}" "${FTP_HOST:?}" "${FTP_PORT:?}"

# The date is decided only here, once per deploy
BUILD_DATE=$(date +%Y-%m-%d)
BUILD_NUMBER="${BITBUCKET_BUILD_NUMBER:-local}"
COMMIT="${BITBUCKET_COMMIT:-$(git rev-parse HEAD 2>/dev/null || echo unknown)}"

CLONE_DIR="${BITBUCKET_CLONE_DIR:-$(pwd)}"
LOCAL_DIR="${CLONE_DIR}/ci_artifacts/${BUILD_NUMBER}"
REMOTE_DIR="/JHT/MCU_FW/${BUILD_DATE}"
STATUS_FILE="${LOCAL_DIR}/BUILD_STATUS.txt"

built_count=0
missing=""
status_lines=""
for target in "$@"; do
    if [ -n "$(find "${LOCAL_DIR}/${target}" -type f -name '*.bin' -print -quit 2>/dev/null)" ]; then
        built_count=$((built_count + 1))
        status_lines+="${target}: OK"$'\n'
    else
        missing+=" ${target}"
        status_lines+="${target}: FAILED (not deployed)"$'\n'
    fi
done

if [ "${built_count}" -eq 0 ]; then
    echo "ERROR: No target built successfully, nothing to deploy:${missing}" >&2
    exit 1
fi

{
    echo "Build date   : ${BUILD_DATE}"
    echo "Build number : ${BUILD_NUMBER}"
    echo "Commit       : ${COMMIT}"
    echo ""
    printf '%s' "${status_lines}"
} > "${STATUS_FILE}"

echo "=== Build status ==="
cat "${STATUS_FILE}"
echo "=== Files to deploy ==="
find "${LOCAL_DIR}" -type f -ls

# Pass the password via environment so it is never expanded inside the lftp script
export LFTP_PASSWORD="${FTP_PASSWORD}"

lftp <<EOF2
set cmd:fail-exit true
set net:max-retries 2
set ftp:ssl-allow true
set ftp:ssl-force true
set ftp:ssl-protect-data false
set ssl:verify-certificate no

open --env-password -u "${FTP_USER}" "ftp://${FTP_HOST}:${FTP_PORT}"

mirror -R --verbose --no-perms "${LOCAL_DIR}" "${REMOTE_DIR}"

bye
EOF2

if [ -n "${missing}" ]; then
    echo "ERROR: Deployed partial firmware set, missing target(s):${missing}" >&2
    exit 1
fi
