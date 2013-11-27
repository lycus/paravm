" Vim syntax file
" Language:      PVA (Parallella Virtual Assembly)
" Maintainer:    The Lycus Foundation <http://lycus.org>
" File Names:    *.pva

if exists("b:current_syntax")
    finish
endif

setlocal iskeyword=.

syn match   pvaDirective                "\.\(fun\|arg\|reg\|blk\|unw\)"

syn match   pvaInstruction              noop
syn match   pvaInstruction              copy
syn match   pvaInstruction              type
syn match   pvaInstruction              load.nil
syn match   pvaInstruction              load.int
syn match   pvaInstruction              load.flt
syn match   pvaInstruction              load.atom
syn match   pvaInstruction              load.bin
syn match   pvaInstruction              load.func
syn match   pvaInstruction              num.add
syn match   pvaInstruction              num.sub
syn match   pvaInstruction              num.mul
syn match   pvaInstruction              num.div
syn match   pvaInstruction              num.rem
syn match   pvaInstruction              num.pow
syn match   pvaInstruction              num.neg
syn match   pvaInstruction              num.and
syn match   pvaInstruction              num.or
syn match   pvaInstruction              num.xor
syn match   pvaInstruction              num.not
syn match   pvaInstruction              num.shl
syn match   pvaInstruction              num.shr
syn match   pvaInstruction              cmp.lt
syn match   pvaInstruction              cmp.gt
syn match   pvaInstruction              cmp.eq
syn match   pvaInstruction              cmp.neq
syn match   pvaInstruction              cmp.lteq
syn match   pvaInstruction              cmp.gteq
syn match   pvaInstruction              call.rem
syn match   pvaInstruction              call.func
syn match   pvaInstruction              call.up
syn match   pvaInstruction              tup.make
syn match   pvaInstruction              tup.get
syn match   pvaInstruction              tup.set
syn match   pvaInstruction              tup.del
syn match   pvaInstruction              tup.size
syn match   pvaInstruction              list.make
syn match   pvaInstruction              list.head
syn match   pvaInstruction              list.tail
syn match   pvaInstruction              list.cons
syn match   pvaInstruction              map.make
syn match   pvaInstruction              map.add
syn match   pvaInstruction              map.get
syn match   pvaInstruction              map.del
syn match   pvaInstruction              map.size
syn match   pvaInstruction              map.keys
syn match   pvaInstruction              map.vals
syn match   pvaInstruction              set.make
syn match   pvaInstruction              set.add
syn match   pvaInstruction              set.find
syn match   pvaInstruction              set.del
syn match   pvaInstruction              set.size
syn match   pvaInstruction              set.vals
syn match   pvaInstruction              bin.size
syn match   pvaInstruction              bin.ebin
syn match   pvaInstruction              bin.dbin
syn match   pvaInstruction              bin.efs
syn match   pvaInstruction              bin.efd
syn match   pvaInstruction              bin.dfs
syn match   pvaInstruction              bin.dfd
syn match   pvaInstruction              bin.eisu
syn match   pvaInstruction              bin.dis
syn match   pvaInstruction              bin.diu
syn match   pvaInstruction              jump.goto
syn match   pvaInstruction              jump.cond
syn match   pvaInstruction              jump.ret
syn match   pvaInstruction              exc.new
syn match   pvaInstruction              exc.get
syn match   pvaInstruction              exc.cont


syn match   pvaSpecialChar              contained +\\["\\']+
syn match   pvaBitChar                  contained +[01]+
syn region  pvaString                   start=+"+ end=+"+ contains=pvaSpecialChar
syn region  pvaAtom                     start=+'+ end=+'+ contains=pvaSpecialChar
syn region  pvaBits                     start=+:+ end=+:+ contains=pvaBitChar
syn match   pvaNumber                   "\<\(\d\+\.\)\=\d\+\([eE][-+]\=\d\+\)\=\>"

syn match   pvaComment                  ";.*$"

hi def link pvaDirective                Type
hi def link pvaInstruction              Statement

hi def link pvaSpecialChar              SpecialChar
hi def link pvaBitChar                  SpecialChar
hi def link pvaString                   String
hi def link pvaAtom                     Constant
hi def link pvaBits                     Macro
hi def link pvaNumber                   Number

hi def link pvaComment                  Comment

let b:current_syntax = "pva"
