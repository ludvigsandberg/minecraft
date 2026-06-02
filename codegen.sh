#!/bin/bash
set -e

# Configuration
VARIANTS=(
    "f32:shared/inc/shared/vec_s32.h:shared/src/vec_s32.c:shared/inc/shared/vec_f32.h:shared/src/vec_f32.c:shared/types.h"
    "u32:shared/inc/shared/arr_s32.h:shared/src/arr_s32.c:shared/inc/shared/arr_u32.h:shared/src/arr_u32.c:shared/types.h"
)

for entry in "${VARIANTS[@]}"; do
    IFS=':' read -r nt oh os nh ns inc <<< "$entry"
    
    # 1. Verification: Prevent silent failures if a source file is missing
    [ -f "$oh" ] || { echo "Error: Source $oh not found!"; exit 1; }

    # 2. Idempotency: Skip if output is newer than source (standard build practice)
    if [ -f "$nh" ] && [ "$nh" -nt "$oh" ] && [ "$ns" -nt "$os" ]; then
        continue 
    fi

    echo "$nh..."
    mkdir -p "$(dirname "$nh")" "$(dirname "$ns")"

    orig_name=$(basename "$oh")

    awk -v t="s32" -v n="$nt" -v T="S32" -v N="${nt^^}" \
        -v inc="shared/types.h" -v ninc="$inc" \
        -v slf="$orig_name" -v nslf="$(basename "$nh")" '
        { gsub(t, n); gsub(T, N); gsub(inc, ninc); gsub(slf, nslf); print }' "$oh" > "$nh.tmp"

    awk -v t="s32" -v n="$nt" -v T="S32" -v N="${nt^^}" \
        -v slf="$orig_name" -v nslf="$(basename "$nh")" '
        { gsub(t, n); gsub(T, N); gsub(slf, nslf); print }' "$os" > "$ns.tmp"

    { printf "/* This file was generated. Do not edit. */\n\n"; cat "$nh.tmp"; } > "$nh"
    { printf "/* This file was generated. Do not edit. */\n\n"; cat "$ns.tmp"; } > "$ns"

    rm -f "$nh.tmp" "$ns.tmp"
done

echo "Done."