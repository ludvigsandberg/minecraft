#!/bin/bash
# Edit the configuration below, then run: ./gen.sh

# ============================================
# VEC family
# ============================================
VEC_HEADER="shared/inc/shared/vec_s32.h"
VEC_SOURCE="shared/src/vec_s32.c"
VEC_INCLUDE='#include "shared/types.h"'

# Each entry: "new_type:out_header:out_source:new_include"
VEC_VARIANTS=(
    "f32:gen/float32/vec_f32.h:gen/float32/vec_f32.c:#include \"shared/types_f32.h\""
    "u32:gen/uint32/vec_u32.h:gen/uint32/vec_u32.c:#include \"shared/types_u32.h\""
)

# ============================================
# ARR family
# ============================================
ARR_HEADER="shared/inc/shared/arr_s32.h"
ARR_SOURCE="shared/src/arr_s32.c"
ARR_INCLUDE='#include "shared/types.h"'

ARR_VARIANTS=(
    "f32:gen/float32/arr_f32.h:gen/float32/arr_f32.c:#include \"shared/types_f32.h\""
    "u32:gen/uint32/arr_u32.h:gen/uint32/arr_u32.c:#include \"shared/types_u32.h\""
)
# ============================================

set -e

process_family() {
    local name="$1"
    local orig_header="$2"
    local orig_source="$3"
    local orig_include="$4"
    shift 4
    local variants=("$@")

    echo "== $name =="
    for entry in "${variants[@]}"; do
        IFS=':' read -r new_type out_h out_s new_include <<< "$entry"
        echo "  $new_type: $out_h and $out_s"

        # Create output directories if needed
        mkdir -p "$(dirname "$out_h")" "$(dirname "$out_s")"

        # Copy originals
        cp "$orig_header" "$out_h"
        cp "$orig_source" "$out_s"

        # Replace type name
        sed "s/s32/$new_type/g" "$out_h" > "$out_h.tmp" && mv "$out_h.tmp" "$out_h"
        sed "s/s32/$new_type/g" "$out_s" > "$out_s.tmp" && mv "$out_s.tmp" "$out_s"

        # Replace include line (if new_include is not empty)
        if [ -n "$new_include" ]; then
            escaped_orig=$(echo "$orig_include" | sed 's/[\/&]/\\&/g')
            escaped_new=$(echo "$new_include" | sed 's/[\/&]/\\&/g')
            sed "s/$escaped_orig/$escaped_new/g" "$out_h" > "$out_h.tmp" && mv "$out_h.tmp" "$out_h"
            sed "s/$escaped_orig/$escaped_new/g" "$out_s" > "$out_s.tmp" && mv "$out_s.tmp" "$out_s"
        fi
    done
}

process_family "vec" "$VEC_HEADER" "$VEC_SOURCE" "$VEC_INCLUDE" "${VEC_VARIANTS[@]}"
process_family "arr" "$ARR_HEADER" "$ARR_SOURCE" "$ARR_INCLUDE" "${ARR_VARIANTS[@]}"

echo "Done."