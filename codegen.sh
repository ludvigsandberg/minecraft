#!/bin/bash
set -e

# Format: orig_h:orig_s:new_t:out_h:out_s:inc_p
VARIANTS=(
    "shared/inc/shared/vec_s32.h:shared/src/vec_s32.c:f32:gen/float32/vec_f32.h:gen/float32/vec_f32.c:shared/types_f32.h"
    "shared/inc/shared/vec_s32.h:shared/src/vec_s32.c:u32:gen/uint32/vec_u32.h:gen/uint32/vec_u32.c:shared/types_u32.h"
    "shared/inc/shared/arr_s32.h:shared/src/arr_s32.c:f32:gen/float32/arr_f32.h:gen/float32/arr_f32.c:shared/types_f32.h"
    "shared/inc/shared/arr_s32.h:shared/src/arr_s32.c:u32:gen/uint32/arr_u32.h:gen/uint32/arr_u32.c:shared/types_u32.h"
)

for entry in "${VARIANTS[@]}"; do
    IFS=':' read -r oh os nt nh ns inc <<< "$entry"
    
    echo "Instantiating $nt..."
    mkdir -p "$(dirname "$nh")" "$(dirname "$ns")"

    # We extract the original filename (e.g., vec_s32.h) directly from the path
    orig_name=$(basename "$oh")

    # Instantiate
    awk -v t="s32" -v n="$nt" -v inc="shared/types.h" -v ninc="$inc" \
        -v slf="$orig_name" -v nslf="$(basename "$nh")" '
        { gsub(t, n); gsub(inc, ninc); gsub(slf, nslf); print }' "$oh" > "$nh.tmp"

    awk -v t="s32" -v n="$nt" -v slf="$orig_name" -v nslf="$(basename "$nh")" '
        { gsub(t, n); gsub(slf, nslf); print }' "$os" > "$ns.tmp"

    # Finalize
    { printf "/* Generated. Do not edit. */\n\n"; cat "$nh.tmp"; } > "$nh"
    { printf "/* Generated. Do not edit. */\n\n"; cat "$ns.tmp"; } > "$ns"
    rm "$nh.tmp" "$ns.tmp"
done