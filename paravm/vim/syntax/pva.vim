" Vim syntax file
" Language:      PVA (Parallella Virtual Assembly)
" Maintainer:    The Lycus Foundation <http://lycus.org>
" File Names:    *.pva

if exists("b:current_syntax")
    finish
endif

syn match   pvaDirective                "\.\(fun\|arg\|reg\|blk\)"
syn keyword pvaInstruction              app ret ldi ldf lda lds ldb mov add sub mul div rem
syn keyword pvaInstruction              pow neg and or xor not shl shr br brc clt cgt cne
syn keyword pvaInstruction              ceq cge cle upc eht ehc ehr

syn keyword pvaTodo                     contained TODO FIXME HACK UNDONE XXX NOTE
syn match   pvaComment                  ";.*$" contains=pvaTodo

syn match   pvaSpecialChar              contained +\\["\\']+
syn match   pvaBitChar                  contained +[01]+
syn region  pvaString                   start=+"+ end=+"+ contains=pvaSpecialChar
syn region  pvaAtom                     start=+'+ end=+'+ contains=pvaSpecialChar
syn region  pvaBits                     start=+:+ end=+:+ contains=pvaBitChar
syn match   pvaNumber                   "\<\(\d\+\.\)\=\d\+\([eE][-+]\=\d\+\)\=\>"

hi def link pvaDirective                Type
hi def link pvaInstruction              Statement

hi def link pvaTodo                     Todo
hi def link pvaComment                  Comment

hi def link pvaSpecialChar              SpecialChar
hi def link pvaBitChar                  SpecialChar
hi def link pvaString                   String
hi def link pvaAtom                     Constant
hi def link pvaBits                     Macro
hi def link pvaNumber                   Number

let b:current_syntax = "pva"
