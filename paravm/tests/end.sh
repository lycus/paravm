if [ -f "${out}" ]; then
    exp="${srcdir}/${name}.exp"
    diff -u "${out}" "${exp}"
fi
